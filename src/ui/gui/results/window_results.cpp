///
/// \file      panel_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "window_results.hpp"
#include <qaction.h>
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdockwidget.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qtablewidget.h>
#include <qthread.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <mutex>
#include <random>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/database/data/heatmap/data_heatmap.hpp"
#include "backend/database/database.hpp"
#include "backend/database/database_interface.hpp"
#include "backend/database/exporter/r/exporter_r.hpp"
#include "backend/database/exporter/xlsx/exporter_xlsx.hpp"
#include "backend/database/query/filter.hpp"
#include "backend/database/query/query_for_image.hpp"
#include "backend/database/query/query_for_well.hpp"
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "backend/user_settings/user_settings.hpp"
#include "graphs/heatmap_widget.hpp"
#include "ui/gui/dialogs/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/dialogs/dialog_image_view/image_view_dock_widget.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/dialogs/widget_video_control_button_group/widget_video_control_button_group.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/helper/widget_generator.hpp"
#include "ui/gui/results/dashboard/dashboard.hpp"
#include "ui/gui/results/panel_classification_list.hpp"
#include "ui/gui/results/panel_graph_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
WindowResults::WindowResults(WindowMain *windowMain) : mWindowMain(windowMain), mDockWidgetImagePreview(new ImageViewDockWidget(this))
{
  // Drop downs
  auto *toolBar = createToolBar();
  addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
  setTabPosition(Qt::DockWidgetArea::LeftDockWidgetArea, QTabWidget::TabPosition::North);
  setWindowTitle("ImageC results");
  const QIcon myIcon(":/icons/icons/icon.png");
  setWindowIcon(myIcon);
  setMinimumSize(1500, 800);
  setObjectName("windowResult");

  // Add to dock
  mDockWidgetImagePreview->getImageWidget()->setShowCrossHairCursor(true);
  mDockWidgetImagePreview->getImageWidget()->setReadOnly(false);
  mDockWidgetImagePreview->getImageWidget()->removeVideoControl();

  static const int32_t SELECTED_INFO_WIDTH   = 250;
  static const int32_t SELECTED_INFO_SPACING = 6;

  //
  // Graph
  //
  {
    mDockWidgetGraphSettings = new PanelGraphSettings(this);
    mGraphContainer          = std::make_shared<HeatmapWidget>(this);
    mGraphContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(mGraphContainer.get(), &HeatmapWidget::onGraphClicked, [this](joda::table::TableCell cell) {
      std::lock_guard<std::mutex> lock(mLoadLock);
      setSelectedElement(cell);
    });

    connect(mGraphContainer.get(), &HeatmapWidget::onGraphDoubleClicked, [this](joda::table::TableCell cell) {
      std::lock_guard<std::mutex> lock(mLoadLock);
      openNextLevel({cell});
    });
    // connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });
    connect(this, &WindowResults::finishedLoading, this, &WindowResults::onFinishedLoading);
    connect(mDockWidgetGraphSettings, &PanelGraphSettings::settingsChanged, [this]() { onColumnComboChanged(); });
  }

  // CLASSIFICATION
  {
    mDockWidgetClassList = new PanelClassificationList(&mFilter);
    connect(mDockWidgetClassList, &PanelClassificationList::settingsChanged, [this]() { refreshView(); });

    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, mDockWidgetGraphSettings);
    addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, mDockWidgetImagePreview);
    splitDockWidget(mDockWidgetImagePreview, mDockWidgetClassList, Qt::Vertical);    // <- key line

    tabifyDockWidget(mDockWidgetImagePreview, mDockWidgetGraphSettings);
    mDockWidgetClassList->raise();
  }

  //
  // Breadcrump
  //
  {
    mBreadCrumpToolBar = new QToolBar();
    mBreadCrumpToolBar->setObjectName("MainWindowTopToolBar");

    mBreadCrumpToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mBreadCrumpToolBar->setMovable(false);
    addToolBar(Qt::ToolBarArea::TopToolBarArea, mBreadCrumpToolBar);

    mBreadCrumpToolBar->addSeparator();
    //
    //
    //
    auto *grp    = new QActionGroup(this);
    mTableButton = new QAction(generateSvgIcon<Style::REGULAR, Color::GREEN>("table"), "", mBreadCrumpToolBar);
    mTableButton->setCheckable(true);
    mTableButton->setChecked(true);
    grp->addAction(mTableButton);
    mBreadCrumpToolBar->addAction(mTableButton);

    mHeatmapButton = new QAction(generateSvgIcon<Style::REGULAR, Color::GREEN>("chart-scatter"), "", mBreadCrumpToolBar);
    mHeatmapButton->setCheckable(true);
    grp->addAction(mHeatmapButton);
    mBreadCrumpToolBar->addAction(mHeatmapButton);

    connect(mHeatmapButton, &QAction::triggered, [this](bool checked) {
      if(checked) {
        onShowHeatmap();
      }
    });
    connect(mTableButton, &QAction::triggered, [this](bool checked) {
      if(checked) {
        onShowTable();
      }
    });

    mBreadCrumpToolBar->addSeparator();

    mBreadCrumpPlate = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("house-simple"), "", mBreadCrumpToolBar);
    mBreadCrumpToolBar->addAction(mBreadCrumpPlate);
    connect(mBreadCrumpPlate, &QAction::triggered, [this]() { backTo(Navigation::PLATE); });

    mBreadCrumpWell = new QAction("Well (1)");
    mBreadCrumpToolBar->addAction(mBreadCrumpWell);
    connect(mBreadCrumpWell, &QAction::triggered, [this]() { backTo(Navigation::WELL); });

    mBreadCrumpImage = new QAction("Image (abcd.tif)");
    mBreadCrumpToolBar->addAction(mBreadCrumpImage);
    connect(mBreadCrumpImage, &QAction::triggered, [this]() { /*backTo(Navigation::IMAGE);*/ });

    // Open next level button
    mOpenNextLevel = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("caret-right"), "", mBreadCrumpToolBar);
    mOpenNextLevel->setStatusTip("Open selected wells/images");
    mBreadCrumpToolBar->addAction(mOpenNextLevel);
    connect(mOpenNextLevel, &QAction::triggered, [this]() {
      auto selectedRows = mDashboard->getSelectedRows();
      openNextLevel(selectedRows);
    });

    mBreadCrumpInfoText = new QLabel();
    mBreadCrumpToolBar->addWidget(mBreadCrumpInfoText);
  }

  //
  // Top infor widget
  //
  auto *topInfo = new QWidget();
  {
    QLayout *topInfoLayout = new QHBoxLayout();
    topInfo->setLayout(topInfoLayout);
    topInfoLayout->setSpacing(SELECTED_INFO_SPACING);
    topInfoLayout->setContentsMargins(0, 0, 0, 0);
    topInfoLayout->setContentsMargins(0, 0, 0, 0);

    mSelectedRowInfo = new QLabel();
    mSelectedRowInfo->setFrameShape(QFrame::StyledPanel);
    mSelectedRowInfo->setFrameShadow(QFrame::Plain);
    mSelectedRowInfo->setLineWidth(1);
    mSelectedRowInfo->setToolTip("Well/Image/Object");
    mSelectedRowInfo->setMaximumWidth(SELECTED_INFO_WIDTH);
    mSelectedRowInfo->setMinimumWidth(SELECTED_INFO_WIDTH);

    mSelectedValue = new QLineEdit();
    mSelectedValue->setReadOnly(true);

    topInfoLayout->addWidget(mSelectedRowInfo);
    topInfoLayout->addWidget(mSelectedValue);
    topInfo->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
  }

  //
  // Dashboard
  //
  {
    mDashboard = new Dashboard(this);
  }

  //
  // Add to layout
  //
  auto *centralWidget = new QWidget();
  auto *col           = new QVBoxLayout();
  col->setContentsMargins(0, 6, 0, 6);
  col->setSpacing(4);
  col->addWidget(topInfo);
  col->addWidget(mGraphContainer.get());
  col->addWidget(mDashboard);
  centralWidget->setLayout(col);

  setCentralWidget(centralWidget);

  onShowTable();
  refreshView();

  connect(mDockWidgetImagePreview->getImageWidget(), &DialogImageViewer::settingChanged, [this] {
    if(mFilter.getFilter().tStack != mVideoControlButton->getMaxTimeStacks()) {
      // If t stack has been changed, reload the results with the new t-stack
      refreshView();
    }
    loadPreview();
  });

  mPreviewThread = std::make_unique<std::thread>(&WindowResults::previewThread, this);
  loadLastOpened();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
WindowResults::~WindowResults()
{
  mStopped = true;
  mPreviewQue.stop();
  if(mPreviewThread != nullptr) {
    if(mPreviewThread->joinable()) {
      mPreviewThread->join();
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::closeEvent(QCloseEvent *event)
{
  mDockWidgetImagePreview->getImageWidget()->getImagePanel()->resetImage();
  mDockWidgetImagePreview->setFloating(false);
  resetSettings();
  refreshView();
  QMainWindow::closeEvent(event);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::valueChangedEvent()
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::setHeatmapVisible(bool visible)
{
  CHECK_GUI_THREAD(mGraphContainer)
  mGraphContainer->setVisible(visible);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::setWindowTitlePrefix(const QString &txt)
{
  if(!txt.isEmpty()) {
    setWindowTitle(QString(Version::getTitle().data()) + " Database - " + txt);
  } else {
    setWindowTitle(QString(Version::getTitle().data()) + " Database");
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void WindowResults::resetSettings()
{
  std::lock_guard<std::mutex> lock(mLoadLock);
  mSelectedDataSet.analyzeMeta.reset();
  mSelectedDataSet.imageMeta.reset();
  mSelectedDataSet.value.reset();
  if(mAnalyzer) {
    mAnalyzer->closeDatabase();
  }
  mAnalyzer.reset();
  goHome();
  mTableButton->blockSignals(true);
  mHeatmapButton->blockSignals(true);

  mHeatmapButton->setChecked(false);
  mTableButton->setChecked(true);
  CHECK_GUI_THREAD(mDashboard)
  mDashboard->setVisible(true);
  mDashboard->reset();
  setHeatmapVisible(false);

  mTableButton->blockSignals(false);
  mHeatmapButton->blockSignals(false);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
auto WindowResults::createToolBar() -> QToolBar *
{
  auto *mTopMenuBar = menuBar();
  auto *toolbar     = new QToolBar(this);
  toolbar->setObjectName("MainWindowTopToolBar");
  toolbar->setMovable(false);
  //
  // Open
  //
  mOpenProjectMenu = new QMenu("Recent results");
  mOpenProjectMenu->setIcon(generateSvgIcon<Style::REGULAR, Color::BLACK>("folder-simple"));

  auto *openDatabase = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("folder-open"), "Open", toolbar);
  openDatabase->setStatusTip("Open results file");
  connect(openDatabase, &QAction::triggered, [this]() { showOpenFileDialog(); });
  toolbar->addAction(openDatabase);

  //
  // Export buttons
  //
  auto *exportMenu = new QMenu("Export");
  // text-csv
  auto *exportData = exportMenu->addAction(generateSvgIcon<Style::REGULAR, Color::GREEN>("file-xls"), "Save as XLSX");
  exportData->setToolTip("Export XLSX");
  connect(exportData, &QAction::triggered, [this]() { showFileSaveDialog("Excel 2007-365 (*.xlsx)"); });

  auto *exportR = exportMenu->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("file-arrow-down"), "Save for R");
  exportR->setToolTip("Export R");
  connect(exportR, &QAction::triggered, [this]() { showFileSaveDialog("R-Script (*.r)"); });

  mExportPng = exportMenu->addAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("file-png"), "Save as PNG");
  mExportPng->setToolTip("Export PNG");
  CHECK_GUI_THREAD(mExportPng)
  mExportPng->setVisible(false);
  connect(mExportPng, &QAction::triggered, [this]() { showFileSaveDialog("PNG image (*.png)"); });

  mExportSvg = exportMenu->addAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("file-svg"), "Save as SVG");
  mExportSvg->setToolTip("Export SVG");
  CHECK_GUI_THREAD(mExportSvg)
  mExportSvg->setVisible(false);
  CHECK_GUI_THREAD(mExportSvg)
  mExportSvg->setEnabled(false);
  connect(mExportSvg, &QAction::triggered, [this]() { showFileSaveDialog("SVG image (*.svg)"); });

  exportMenu->addSeparator();

  //
  mExports = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("export"), "Export", toolbar);
  mExports->setStatusTip("Export table or heatmap to R or Excel");
  connect(mExports, &QAction::triggered, [this]() { showFileSaveDialog(); });
  mExports->setMenu(exportMenu);
  toolbar->addAction(mExports);

  auto *btn = qobject_cast<QToolButton *>(toolbar->widgetForAction(mExports));
  btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

  connect(this, &WindowResults::finishedExport, this, &WindowResults::onFinishedExport);

  //
  // Copy button
  //
  auto *copyTable = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("copy"), "Copy values", toolbar);
  connect(copyTable, &QAction::triggered, [this]() {
    if(mGraphContainer->isVisible()) {
      mGraphContainer->copyToClipboard();
    } else {
      mDashboard->copyToClipboard();
    }
  });
  copyTable->setStatusTip("Copy table to clipboard");
  toolbar->addAction(copyTable);

  QMenu *windowMenu = new QMenu("Window");

  void setWindowDisplayMode(bool);
  auto *windowMode = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("browsers"), "Window mode");
  windowMode->setCheckable(true);
  windowMode->setStatusTip("Switch to window mode");
  connect(windowMode, &QAction::triggered, [this](bool enabled) {
    CHECK_GUI_THREAD(mCascade)
    mCascade->setEnabled(enabled);
    CHECK_GUI_THREAD(mTile)
    mTile->setEnabled(enabled);
    CHECK_GUI_THREAD(mMinimize)
    mMinimize->setEnabled(enabled);
    CHECK_GUI_THREAD(mRestore)
    mRestore->setEnabled(enabled);
    mDashboard->setWindowDisplayMode(enabled);
  });
  windowMenu->addAction(windowMode);

  windowMenu->addSeparator();
  mCascade = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("browsers"), "Cascade");
  CHECK_GUI_THREAD(mCascade)
  mCascade->setEnabled(false);
  mCascade->setStatusTip("Cascade windows");
  connect(mCascade, &QAction::triggered, [this]() { mDashboard->cascadeSubWindows(); });
  windowMenu->addAction(mCascade);

  mTile = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("grid-four"), "Tile");
  CHECK_GUI_THREAD(mTile)
  mTile->setEnabled(false);
  mTile->setStatusTip("Tile windows");
  connect(mTile, &QAction::triggered, [this]() { mDashboard->tileSubWindows(); });
  windowMenu->addAction(mTile);

  mMinimize = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("minus"), "Minimize");
  CHECK_GUI_THREAD(mMinimize)
  mMinimize->setEnabled(false);
  mMinimize->setStatusTip("Minimize windows");
  connect(mMinimize, &QAction::triggered, [this]() { mDashboard->minimizeSubWindows(); });
  windowMenu->addAction(mMinimize);

  mRestore = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("app-window"), "Restore");
  CHECK_GUI_THREAD(mRestore)
  mRestore->setEnabled(false);
  mRestore->setStatusTip("Restore windows");
  connect(mRestore, &QAction::triggered, [this]() { mDashboard->restoreSubWindows(); });
  windowMenu->addAction(mRestore);

  mFilter.sortColumns();
  toolbar->addSeparator();

  //
  // Mark as invalid button
  //
  // edit-select-none
  // paint-none
  // video-off
  mMarkAsInvalid = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("funnel-x"), "");
  mMarkAsInvalid->setToolTip("Exclude selected image from statistics");
  mMarkAsInvalid->setCheckable(true);
  toolbar->addAction(mMarkAsInvalid);
  CHECK_GUI_THREAD(mMarkAsInvalid)
  mMarkAsInvalid->setEnabled(false);
  connect(mMarkAsInvalid, &QAction::triggered, this, &WindowResults::onMarkAsInvalidClicked);

  toolbar->addSeparator();
  mVideoControlButton = new VideoControlButtonGroup(
      [this] {
        refreshView();
        //  loadPreview();
      },
      toolbar);

  // =====================================
  // Toolbar
  // =====================================

  // =====================================
  // Menu bar
  // =====================================
  auto *fileMenu = mTopMenuBar->addMenu("File");
  fileMenu->addAction(openDatabase);
  fileMenu->addMenu(mOpenProjectMenu);
  fileMenu->addSeparator();
  fileMenu->addMenu(exportMenu);

  mTopMenuBar->addMenu(windowMenu);

  return toolbar;
}

///
/// \brief      Load last opened files
/// \author     Joachim Danmayr
///
void WindowResults::loadLastOpened()
{
  mOpenProjectMenu->clear();
  mOpenProjectMenu->addSection("Results");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedResult()) {
    auto *action = mOpenProjectMenu->addAction((path.path + " (" + path.title + ")").data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() { openFromFile(path.data()); });
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::storeResultsTableSettingsToDatabase()
{
  try {
    if(mAnalyzer != nullptr && mSelectedDataSet.analyzeMeta.has_value() && !mSelectedDataSet.analyzeMeta->jobId.empty()) {
      mAnalyzer->updateResultsTableSettings(mSelectedDataSet.analyzeMeta->jobId, nlohmann::json(mFilter).dump());
    }
  } catch(...) {
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::refreshBreadCrump()
{
  switch(mNavigation) {
    case Navigation::PLATE:
      CHECK_GUI_THREAD(mBreadCrumpWell)
      mBreadCrumpWell->setVisible(false);
      CHECK_GUI_THREAD(mBreadCrumpImage)
      mBreadCrumpImage->setVisible(false);
      CHECK_GUI_THREAD(mOpenNextLevel)
      mOpenNextLevel->setVisible(true);
      mDockWidgetImagePreview->setFloating(false);
      mVideoControlButton->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());
      break;
    case Navigation::WELL: {
      CHECK_GUI_THREAD(mBreadCrumpWell)
      mBreadCrumpWell->setVisible(true);
      CHECK_GUI_THREAD(mBreadCrumpImage)
      mBreadCrumpImage->setVisible(false);
      CHECK_GUI_THREAD(mOpenNextLevel)
      mOpenNextLevel->setVisible(true);
      mDockWidgetImagePreview->setFloating(false);
      mVideoControlButton->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());
      std::string groupName;
      if(mSelectedDataSet.groupMeta.has_value()) {
        groupName = "Well (" + std::string(1, (static_cast<char>(mSelectedDataSet.groupMeta->posY - 1) + 'A')) +
                    std::to_string(mSelectedDataSet.groupMeta->posX) + ")";
      }
      if(mActGroupId.size() > 1) {
        groupName = "";
        for(auto groupId : mActGroupId) {
          auto groupInfo = mAnalyzer->selectGroupInfo(groupId);
          groupName += groupInfo.groupName + ",";
        }
        if(!groupName.empty()) {
          groupName.erase(groupName.size() - 1);
        }
      }
      mBreadCrumpWell->setText(groupName.data());
    } break;
    case Navigation::IMAGE:
      CHECK_GUI_THREAD(mBreadCrumpWell)
      mBreadCrumpWell->setVisible(true);
      CHECK_GUI_THREAD(mBreadCrumpImage)
      mBreadCrumpImage->setVisible(true);
      CHECK_GUI_THREAD(mOpenNextLevel)
      mOpenNextLevel->setVisible(false);
      mVideoControlButton->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());    // Reset

      //
      std::string imageName;
      if(mSelectedDataSet.imageMeta.has_value()) {
        imageName = mSelectedDataSet.imageMeta->filename;
      }
      if(mActImageId.size() > 1) {
        imageName = "";
        for(auto imageId : mActImageId) {
          auto imageInfo = mAnalyzer->selectImageInfo(imageId);
          imageName += imageInfo.filename + ",";
        }
        if(!imageName.empty()) {
          imageName.erase(imageName.size() - 1);
        }
      }
      mBreadCrumpImage->setText("Image (" + QString(imageName.data()) + ")");

      break;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool WindowResults::showSelectWorkingDir(const QString & /*path*/)
{
  QFileDialog dialog(this);
  dialog.setWindowTitle("Select images Directory");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::ShowDirsOnly, true);
  dialog.setOption(QFileDialog::DontUseNativeDialog, true);
  if(dialog.exec() == QDialog::Accepted) {
    mImageWorkingDirectory = std::filesystem::path(dialog.selectedFiles().first().toStdString());
    return true;
  } else {
    return false;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::loadPreview()
{
  if(!mGeneratePreviewMutex.try_lock()) {
    return;
  }

  if(mImageWorkingDirectory.empty()) {
    // No working directory selected. Make the image preview invisible
    mGeneratePreviewMutex.unlock();
    return;
  }
  if(!mSelectedDataSet.analyzeMeta.has_value() || !mSelectedDataSet.imageMeta.has_value() || !mSelectedDataSet.objectInfo.has_value()) {
    mGeneratePreviewMutex.unlock();
    return;
  }
  // From relative file path
  std::filesystem::path imagePathRel = std::filesystem::path(mSelectedDataSet.imageMeta->imageFilePathRel);
  auto imagePath                     = mImageWorkingDirectory / imagePathRel;
  bool showDialog                    = !std::filesystem::exists(imagePath);
  if(std::filesystem::is_directory(imagePath)) {
    mGeneratePreviewMutex.unlock();
    return;
  }

  while(showDialog) {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Image not found");
    msgBox.setText("Image >" + QString(mSelectedDataSet.imageMeta->filename.data()) +
                   "< not found. Would you like to select the folder in which the images are located??");
    msgBox.setIcon(QMessageBox::Question);

    // Create custom buttons
    QPushButton *cancelButton       = msgBox.addButton("Cancel", QMessageBox::RejectRole);
    QPushButton *selectFolderButton = msgBox.addButton("Select Folder", QMessageBox::AcceptRole);
    QPushButton *dontAskAgainButton = msgBox.addButton("Don't Ask Again", QMessageBox::DestructiveRole);

    // Execute the message box
    msgBox.exec();

    // Determine which button was clicked
    if(msgBox.clickedButton() == cancelButton) {
      mGeneratePreviewMutex.unlock();
      return;
    } else if(msgBox.clickedButton() == selectFolderButton) {
      if(showSelectWorkingDir(mImageWorkingDirectory.string().data())) {
        imagePath  = mImageWorkingDirectory / imagePathRel;
        showDialog = !std::filesystem::exists(imagePath);
      } else {
        mGeneratePreviewMutex.unlock();
        return;
      }
    } else if(msgBox.clickedButton() == dontAskAgainButton) {
      mImageWorkingDirectory.clear();
      mGeneratePreviewMutex.unlock();
      return;
    }
  }

  mPreviewQue.push({.imagePath   = imagePath,
                    .analyzeMeta = mSelectedDataSet.analyzeMeta.value(),
                    .imageMeta   = mSelectedDataSet.imageMeta.value(),
                    .objectInfo  = mSelectedDataSet.objectInfo.value()});
  mGeneratePreviewMutex.unlock();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::previewThread()
{
  while(!mStopped) {
    auto previewData = mPreviewQue.pop();
    try {
      int32_t tileWidth      = static_cast<int32_t>(previewData.analyzeMeta.tileWidth);
      int32_t tileHeight     = static_cast<int32_t>(previewData.analyzeMeta.tileHeight);
      int32_t series         = static_cast<int32_t>(previewData.analyzeMeta.series);
      const auto &objectInfo = previewData.objectInfo;
      int32_t tileXNr        = static_cast<int32_t>(static_cast<float>(objectInfo.measCenterX) / static_cast<float>(tileWidth));
      int32_t tileYNr        = static_cast<int32_t>(static_cast<float>(objectInfo.measCenterY) / static_cast<float>(tileHeight));
      // int32_t resolution     = 0;

      auto plane = joda::enums::PlaneId{.tStack = static_cast<int32_t>(objectInfo.stackT),
                                        .zStack = static_cast<int32_t>(objectInfo.stackZ),
                                        .cStack = static_cast<int32_t>(objectInfo.stackC)};

      mDockWidgetImagePreview->getImageWidget()->setImagePlane({plane, series, tileWidth, tileHeight, tileXNr, tileYNr});
      mDockWidgetImagePreview->getImageWidget()->getImagePanel()->openImage(previewData.imagePath);
      mVideoControlButton->setValue(objectInfo.stackT);

      // ==============================================
      // Set cursor position
      // ==============================================

      QRect boungingBox{static_cast<int32_t>(objectInfo.measBoxX), static_cast<int32_t>(objectInfo.measBoxY),
                        static_cast<int32_t>(objectInfo.measBoxWidth), static_cast<int32_t>(objectInfo.measBoxHeight)};
      mDockWidgetImagePreview->getImageWidget()->getImagePanel()->setCursorPositionFromOriginalImageCoordinatesAndCenter(boungingBox);

    } catch(const std::exception &ex) {
      // No image selected
      joda::log::logError("Preview error: " + std::string(ex.what()));
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::refreshView()
{
  const auto &wellOrder = mDockWidgetGraphSettings->getWellOrder();
  auto plateSize        = mDockWidgetGraphSettings->getPlateSize();
  uint16_t rows         = static_cast<uint16_t>(plateSize.height());
  uint16_t cols         = static_cast<uint16_t>(plateSize.width());

  auto form = static_cast<WindowResults::Navigation>(mNavigation) == WindowResults::Navigation::PLATE
                  ? joda::settings::DensityMapSettings::ElementForm::CIRCLE
                  : joda::settings::DensityMapSettings::ElementForm::RECTANGLE;

  mFilter.setFilter({.plateId = 0, .groupId = mActGroupId, .imageId = mActImageId, .tStack = mVideoControlButton->value()},
                    {.rows = static_cast<uint16_t>(rows), .cols = static_cast<uint16_t>(cols), .wellImageOrder = wellOrder},
                    {.form               = form,
                     .heatmapRangeMode   = mFilter.getDensityMapSettings().heatmapRangeMode,
                     .heatmapRangeMin    = mFilter.getDensityMapSettings().heatmapRangeMin,
                     .heatmapRangeMax    = mFilter.getDensityMapSettings().heatmapRangeMax,
                     .densityMapAreaSize = static_cast<int32_t>(mDockWidgetGraphSettings->getDensityMapSize())});

  //
  //
  //
  if(mAnalyzer && !mIsLoading) {
    mIsLoading = true;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::thread([this, rows, cols, wellOrder = wellOrder] {
      try {
        joda::log::logTrace("Start refreshing view ...");

        std::lock_guard<std::mutex> lock(mLoadLock);
        if(!mAnalyzer) {
          return;
        }
        storeResultsTableSettingsToDatabase();
      REFRESH_VIEW:
        switch(mNavigation) {
          case Navigation::PLATE: {
            mActListData = std::make_shared<db::QueryResult>(
                joda::db::StatsPerGroup::toTable(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_PLATE, &mActFilter));
            if(mActListData->getNrOfRows() == 1) {
              // If there are no groups, switch directly to well view
              mNavigation                = Navigation::WELL;
              auto getID                 = mActListData->data(0, 0)->getId();
              mActGroupId                = {static_cast<uint16_t>(getID)};
              mSelectedWellId            = getID;
              mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(getID);
              mFilter.setFilter({.plateId = 0, .groupId = mActGroupId, .imageId = mActImageId, .tStack = mVideoControlButton->value()},
                                {.rows = static_cast<uint16_t>(rows), .cols = static_cast<uint16_t>(cols), .wellImageOrder = wellOrder},
                                {.densityMapAreaSize = static_cast<int32_t>(mDockWidgetGraphSettings->getDensityMapSize())});
              goto REFRESH_VIEW;
            }

          } break;
          case Navigation::WELL: {
            mActListData = std::make_shared<db::QueryResult>(
                joda::db::StatsPerGroup::toTable(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_WELL, &mActFilter));
          } break;
          case Navigation::IMAGE: {
            mActListData = std::make_shared<db::QueryResult>(joda::db::StatsPerImage::toTable(mAnalyzer.get(), mFilter, &mActFilter));

          } break;
        }
        mIsLoading = false;
        joda::log::logTrace("Finished refresh view.");
      } catch(const std::exception &ex) {
        mIsLoading = false;
        joda::log::logError(ex.what());
      }
      emit finishedLoading();
    }).detach();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::onFinishedLoading()
{
  if(mActListData == nullptr) {
    refreshBreadCrump();
    update();
    QApplication::restoreOverrideCursor();
    return;
  }

  std::lock_guard<std::mutex> lock(mLoadLock);
  // ===============================================
  // Data
  // ===============================================
  if(mSelectedDataSet.analyzeMeta.has_value()) {
    mDashboard->tableToQWidgetTable(mActListData, mTmpColocClasses, mNavigation == Navigation::IMAGE,
                                    mSelectedDataSet.analyzeMeta->physicalPixelSizeUnit);
  }

  // ===============================================
  // Heatmap
  // ===============================================
  int32_t cols           = mFilter.getPlateSetup().cols;
  int32_t rows           = mFilter.getPlateSetup().rows;
  int32_t densityMapSize = -1;
  switch(mNavigation) {
    case Navigation::PLATE:
      break;
    case Navigation::WELL:
      rows = mFilter.getPlateSetup().getRowsAndColsOfWell().first;
      cols = mFilter.getPlateSetup().getRowsAndColsOfWell().second;
      break;
    case Navigation::IMAGE:
      densityMapSize = mFilter.getDensityMapSettings().densityMapAreaSize;
      if(mSelectedDataSet.imageMeta.has_value()) {
        rows = static_cast<int32_t>(std::ceil(static_cast<float>(mSelectedDataSet.imageMeta->height) / static_cast<float>(densityMapSize)));
        cols = static_cast<int32_t>(std::ceil(static_cast<float>(mSelectedDataSet.imageMeta->width) / static_cast<float>(densityMapSize)));
      } else {
        rows           = 1;
        cols           = 1;
        densityMapSize = -1;
      }

      break;
  }
  if(mSelectedDataSet.analyzeMeta.has_value()) {
    mDockWidgetGraphSettings->setColumns(mActFilter.getColumns(), mSelectedDataSet.analyzeMeta->physicalPixelSizeUnit);
  }

  // CHANGED FROM ActFilter
  auto dataIn = joda::db::data::convertToHeatmap(mActListData.get(), static_cast<uint32_t>(rows), static_cast<uint32_t>(cols),
                                                 static_cast<uint32_t>(mDockWidgetGraphSettings->getSelectedColumn()), mActFilter.getFilter().tStack,
                                                 joda::db::data::PlotPlateSettings{.densityMapSize = densityMapSize});

  mGraphContainer->updateGraph(dataIn, mDockWidgetGraphSettings->getSelectedColorMap(), mDockWidgetGraphSettings->getColorMapRangeSetting(),
                               mDockWidgetGraphSettings->getColorMapRange(), mNavigation == Navigation::PLATE, mNavigation == Navigation::IMAGE);
  mDockWidgetGraphSettings->setColorMapRange(mGraphContainer->getColorMapRange());

  // ===============================================
  // Refresh
  // ===============================================
  refreshBreadCrump();
  update();
  QApplication::restoreOverrideCursor();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void WindowResults::onMarkAsInvalidClicked(bool isInvalid)
{
  if(isInvalid) {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->setImageValidity(mSelectedImageId, val);
  } else {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->unsetImageValidity(mSelectedImageId, val);
  }
  refreshView();
}

///
/// \brief      An element has been selected
/// \author     Joachim Danmayr
///
void WindowResults::setSelectedElement(table::TableCell value)
{
  if(!mAnalyzer) {
    return;
  }
  switch(mNavigation) {
    case Navigation::PLATE: {
      mSelectedWellId            = value.getId();
      mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(value.getId());
      mSelectedDataSet.imageMeta.reset();
      mSelectedDataSet.objectInfo.reset();
      CHECK_GUI_THREAD(mMarkAsInvalid)
      mMarkAsInvalid->setEnabled(false);

      // Act data
      auto platePos = mSelectedDataSet.groupMeta->groupName;
      mSelectedRowInfo->setText(platePos.data());
    } break;
    case Navigation::WELL: {
      mSelectedImageId = value.getId();
      mSelectedDataSet.objectInfo.reset();
      auto imageInfo             = mAnalyzer->selectImageInfo(value.getId());
      mSelectedDataSet.imageMeta = imageInfo;
      mMarkAsInvalid->blockSignals(true);

      if(imageInfo.validity.test(enums::ChannelValidityEnum::MANUAL_OUT_SORTED)) {
        mMarkAsInvalid->setChecked(true);
      } else {
        mMarkAsInvalid->setChecked(false);
      }
      mMarkAsInvalid->blockSignals(false);
      CHECK_GUI_THREAD(mMarkAsInvalid)
      mMarkAsInvalid->setEnabled(true);

      // Act data
      auto platePos = imageInfo.imageGroupName + "/" + imageInfo.filename;
      mSelectedRowInfo->setText(platePos.data());
    }

    break;
    case Navigation::IMAGE:
      mSelectedTileId = value.getObjectId();
      CHECK_GUI_THREAD(mMarkAsInvalid)
      mMarkAsInvalid->setEnabled(false);

      mSelectedDataSet.objectInfo = mAnalyzer->selectObjectInfo(mSelectedTileId);
      if(mSelectedDataSet.imageMeta->imageId != mSelectedDataSet.objectInfo->imageId) {
        mSelectedDataSet.imageMeta = mAnalyzer->selectImageInfo(mSelectedDataSet.objectInfo->imageId);
      }

      auto rowImageName = mSelectedDataSet.imageMeta->filename;
      if(mActImageId.size() > 1) {
        rowImageName = value.getRowName();
      }
      auto platePos = mSelectedDataSet.imageMeta->imageGroupName + "/" + rowImageName + "/" + std::to_string(value.getObjectId());
      mSelectedRowInfo->setText(platePos.data());

      loadPreview();
      break;
  }
  // QString headerTxt =  " | " + ;
  mSelectedValue->setText(QString::number(value.getVal()));
  mSelectedDataSet.value = DataSet::Value{.value = value.getVal()};
}

///
/// \brief      Open the next deeper level form the element with given id
/// \author     Joachim Danmayr
///
void WindowResults::openNextLevel(const std::vector<table::TableCell> &selectedRows)
{
  int actMenu     = static_cast<int>(mNavigation);
  auto oldActMenu = mNavigation;
  actMenu++;
  if(actMenu <= 2) {
    mNavigation = static_cast<Navigation>(actMenu);
  } else {
    // An area has been selected within an image -> trigger an export
    return;
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      break;
    case Navigation::WELL:
      if(!selectedRows.empty()) {
        std::set<uint16_t> act;
        for(const auto &row : selectedRows) {
          act.emplace(row.getObjectId());
        }
        mActGroupId = act;
      } else {
        mNavigation = oldActMenu;
      }
      break;
    case Navigation::IMAGE:
      if(selectedRows.empty()) {
        mNavigation = oldActMenu;
      } else {
        std::set<uint64_t> act;
        for(const auto &row : selectedRows) {
          act.emplace(row.getObjectId());
        }
        mActImageId = act;
      }
      break;
  }
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void WindowResults::backTo(Navigation backTo)
{
  // int actMenu = static_cast<int>(mNavigation);
  // actMenu--;
  // if(actMenu >= 0) {
  //   mNavigation = static_cast<Navigation>(actMenu);
  // }
  mNavigation = backTo;
  switch(mNavigation) {
    case Navigation::PLATE:
      mSelectedDataSet.imageMeta.reset();
      mSelection.erase(Navigation::WELL);
      break;
    case Navigation::WELL:
      mSelection.erase(Navigation::IMAGE);
      break;
    case Navigation::IMAGE:
      break;
  }

  refreshView();
}

void WindowResults::goHome()
{
  mNavigation = Navigation::PLATE;
  mSelectedDataSet.imageMeta.reset();
  mSelection.erase(Navigation::WELL);
  mSelection.erase(Navigation::IMAGE);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowResults::openFromFile(const QString &pathToDbFile)
{
  if(pathToDbFile.isEmpty()) {
    return;
  }
  resetSettings();
  mAnalyzer = std::make_unique<joda::db::Database>();
  mAnalyzer->openDatabase(std::filesystem::path(pathToDbFile.toStdString()));
  mDbFilePath = std::filesystem::path(pathToDbFile.toStdString());

  // Database results file is stored in <IMAGE-PATH>/imagec/results/<JOB-NAME>/results.icdb
  // We assume the images to be in the folder ../../../../<IMAGES>
  // If not the user will be asked to select the image working directory.
  mImageWorkingDirectory = mDbFilePath.parent_path().parent_path().parent_path().parent_path();

  mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  // Try to load settings if available
  try {
    if(mSelectedDataSet.analyzeMeta.has_value()) {
      auto selectedClasses = mAnalyzer->selectClasses();
      // auto imageChannels   = mAnalyzer->selectImageChannels();

      auto resultsSettings                = mAnalyzer->selectResultsTableSettings(mSelectedDataSet.analyzeMeta->jobId);
      settings::ResultsSettings filterTmp = nlohmann::json::parse(resultsSettings);

      const auto &t = filterTmp.getColumns();
      for(const auto &[idx, kex] : t) {
        if(kex.measureChannel == enums::Measurement::NONE) {
          joda::log::logWarning("This is a legacy setting!");
          throw std::invalid_argument("This is a legacy setting!");
        }
      }
      mFilter = filterTmp;

      const auto &plateSetup = mFilter.getPlateSetup();
      mDockWidgetGraphSettings->fromSettings(plateSetup.wellImageOrder, {plateSetup.cols, plateSetup.rows},
                                             mFilter.getDensityMapSettings().densityMapAreaSize);
    }
  } catch(...) {
  }
  // Load stuff
  mTmpColocClasses = mAnalyzer->selectColocalizingClasses();

  // Store last opened
  if(mSelectedDataSet.analyzeMeta.has_value()) {
    // joda::user_settings::UserSettings::addLastOpenedResult(pathToDbFile.toStdString(), mSelectedDataSet.analyzeMeta->jobName);
    mWindowMain->addToLastLoadedResults(pathToDbFile, mSelectedDataSet.analyzeMeta->jobName.data());
  }
  loadLastOpened();

  mVideoControlButton->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());

  mDockWidgetClassList->setDatabase(mAnalyzer.get());
  setWindowTitlePrefix(QString(mDbFilePath.filename().string().data()) + " (" + QString(mSelectedDataSet.analyzeMeta->jobName.data()) + ")");

  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::onShowTable()
{
  if(mExportSvg != nullptr) {
    CHECK_GUI_THREAD(mExportSvg)
    mExportSvg->setVisible(false);
    CHECK_GUI_THREAD(mExportPng)
    mExportPng->setVisible(false);
  }

  CHECK_GUI_THREAD(mDashboard)
  mDashboard->setVisible(true);
  CHECK_GUI_THREAD(mDockWidgetGraphSettings)
  mDockWidgetGraphSettings->setVisible(false);
  setHeatmapVisible(false);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::onShowHeatmap()
{
  if(mExportSvg != nullptr) {
    CHECK_GUI_THREAD(mExportSvg)
    mExportSvg->setVisible(true);
    CHECK_GUI_THREAD(mExportPng)
    mExportPng->setVisible(true);
  }
  CHECK_GUI_THREAD(mDashboard)
  mDashboard->setVisible(false);
  CHECK_GUI_THREAD(mDockWidgetGraphSettings)
  mDockWidgetGraphSettings->setVisible(true);
  mDockWidgetGraphSettings->raise();    // Make it the active tab
  setHeatmapVisible(true);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::columnEdit(int32_t /*colIdx*/)
{
  mFilter.sortColumns();
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::onColumnComboChanged()
{
  refreshView();
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::showOpenFileDialog()
{
  std::filesystem::path filePath = mDbFilePath.parent_path();

  QFileDialog::Options opt = QFileDialog::DontUseNativeDialog;
  QString filename         = QFileDialog::getOpenFileName(this, "Open File", filePath.string().data(),
                                                          "ImageC results files (*" + QString(joda::fs::EXT_DATABASE.data()) + ")", nullptr, opt);
  // Select save option
  if(filename.endsWith(joda::fs::EXT_DATABASE.data())) {
    openFromFile(filename);
  }
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::showFileSaveDialog(const QString &filter)
{
  QString templatePath = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();

  auto getEndianFromFilter = [](const QString &filterIn) -> std::string {
    std::string endian;
    if(filterIn.contains("(*.xlsx)")) {
      endian = ".xlsx";
    } else if(filterIn.contains("(*.r)")) {
      endian = ".r";
    } else if(filterIn.contains("(*.csv)")) {
      endian = ".csv";
    } else if(filterIn.contains("(*.svg)")) {
      endian = ".svg";
    } else if(filterIn.contains("(*.png)")) {
      endian = ".png";
    } else {
      return "";
    }
    return endian;
  };
  std::string defaultEndian = getEndianFromFilter(filter);

  std::filesystem::path filePath = mDbFilePath.parent_path();
  if(mSelectedDataSet.analyzeMeta.has_value()) {
    filePath = filePath / (mSelectedDataSet.analyzeMeta->jobName + defaultEndian);
  } else {
    filePath = filePath / ("results" + defaultEndian);
  }

  QString selectedFilter;
  QFileDialog::Options opt       = QFileDialog::DontUseNativeDialog;
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", filePath.string().data(), filter, &selectedFilter, opt);
  std::string filename           = filePathOfSettingsFile.toStdString();
  auto selectedEndian            = getEndianFromFilter(selectedFilter);
  if(!filename.ends_with(selectedEndian)) {
    filename += selectedEndian;
  }

  // Select save option
  if(filename.ends_with(".xlsx")) {
    saveData(filename, joda::exporter::xlsx::ExportSettings::ExportFormat::XLSX);
  } else if(filename.ends_with(".r")) {
    saveData(filename, joda::exporter::xlsx::ExportSettings::ExportFormat::R);
  } else if(filename.ends_with(".svg")) {
    // mHeatmapChart->exportToSVG(filename.data());
  } else if(filename.ends_with(".png")) {
    auto path = std::filesystem::path(filename);
    mGraphContainer->exportToPNG(path);
    QDesktopServices::openUrl(QUrl("file:///" + QString(path.parent_path().string().data())));
  }
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::saveData(const std::string &fileName, joda::exporter::xlsx::ExportSettings::ExportFormat format)
{
  if(fileName.empty()) {
    return;
  }

  CHECK_GUI_THREAD(mExports)
  mExports->setEnabled(false);
  mBreadCrumpInfoText->setText("Export running ...");
  std::thread([this, fileName, format] {
    joda::settings::AnalyzeSettings settings;
    try {
      settings = nlohmann::json::parse(mSelectedDataSet.analyzeMeta->analyzeSettingsJsonString);
    } catch(const std::exception &ex) {
      joda::log::logWarning("Could not parse settings from database. Reason: " + std::string(ex.what()));
    }

    if(format == joda::exporter::xlsx::ExportSettings::ExportFormat::XLSX) {
      if(mGraphContainer->isVisible()) {
        // Export heatmap
        int32_t imgWidth                                      = 0;
        int32_t imgHeight                                     = 0;
        joda::exporter::xlsx::ExportSettings::ExportView view = joda::exporter::xlsx::ExportSettings::ExportView::PLATE;
        switch(mNavigation) {
          case Navigation::PLATE:
            break;
          case Navigation::WELL:
            view = joda::exporter::xlsx::ExportSettings::ExportView::WELL;
            break;
          case Navigation::IMAGE:
            view      = joda::exporter::xlsx::ExportSettings::ExportView::IMAGE;
            imgHeight = mSelectedDataSet.imageMeta->height;
            imgWidth  = mSelectedDataSet.imageMeta->width;
            break;
        }

        joda::exporter::xlsx::Exporter::startHeatmapExport({mActListData.get()}, settings, mSelectedDataSet.analyzeMeta->jobName,
                                                           mSelectedDataSet.analyzeMeta->timestampStart,
                                                           mSelectedDataSet.analyzeMeta->timestampFinish, fileName, mActFilter, view, imgHeight,
                                                           imgWidth, mSelectedDataSet.analyzeMeta->physicalPixelSizeUnit);

      } else {
        joda::exporter::xlsx::Exporter::startExport(mDashboard->getExportables(), settings, mSelectedDataSet.analyzeMeta->jobName,
                                                    mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                    mSelectedDataSet.analyzeMeta->physicalPixelSizeUnit, fileName);
      }
    } else {
      joda::exporter::r::Exporter::startExport(mDashboard->getExportables(), settings, mSelectedDataSet.analyzeMeta->jobName,
                                               mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                               mSelectedDataSet.analyzeMeta->physicalPixelSizeUnit, fileName);
    }

    QString folderPath = std::filesystem::path(fileName).parent_path().string().data();
    QDesktopServices::openUrl(QUrl("file:///" + folderPath));
    emit finishedExport();
  }).detach();
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void WindowResults::onFinishedExport()
{
  CHECK_GUI_THREAD(mExports)
  mExports->setEnabled(true);
  mBreadCrumpInfoText->setText("");
}

}    // namespace joda::ui::gui
