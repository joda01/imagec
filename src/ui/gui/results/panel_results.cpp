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

#include "panel_results.hpp"
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
#include "graphs/graph_qt_backend.hpp"
#include "ui/gui/container/container_button.hpp"
#include "ui/gui/container/container_label.hpp"
#include "ui/gui/container/panel_edit_base.hpp"
#include "ui/gui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/helper/widget_generator.hpp"
#include "ui/gui/results/dashboard/dashboard.hpp"
#include "ui/gui/results/panel_classification_list.hpp"
#include "ui/gui/results/panel_graph_settings.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "dialog_column_settings.hpp"

namespace joda::ui::gui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelResults::PanelResults(WindowMain *windowMain) :
    PanelEdit(windowMain, nullptr, false, windowMain), mWindowMain(windowMain),
    mDockWidgetImagePreview(new DialogImageViewer(windowMain, false, windowMain))
{
  // Drop downs
  createEditColumnDialog();
  createToolBar(&layout());

  // Add to dock
  mDockWidgetImagePreview->setPreviewImageSizeVisble(false);
  mDockWidgetImagePreview->setPipelineResultsButtonVisible(false);
  mDockWidgetImagePreview->setVisible(false);
  mDockWidgetImagePreview->setShowCrossHairCursor(true);
  mDockWidgetImagePreview->setShowPixelInfo(false);
  mDockWidgetImagePreview->setShowOverlay(false);
  mDockWidgetImagePreview->setZProjectionButtonVisible(true);
  mWindowMain->addDockWidget(Qt::RightDockWidgetArea, mDockWidgetImagePreview);

  static const int32_t SELECTED_INFO_WIDTH   = 250;
  static const int32_t SELECTED_INFO_SPACING = 6;
  //
  // Graph
  //
  {
    mDockWidgetGraphSettings = new PanelGraphSettings(mWindowMain);
    mGraphContainer          = std::make_shared<QtBackend>(this);
    mGraphContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(mGraphContainer.get(), &QtBackend::onGraphClicked, [this](joda::table::TableCell cell) {
      std::lock_guard<std::mutex> lock(mLoadLock);
      setSelectedElement(cell);
    });

    connect(mGraphContainer.get(), &QtBackend::onGraphDoubleClicked, [this](joda::table::TableCell cell) {
      std::lock_guard<std::mutex> lock(mLoadLock);

      openNextLevel({cell});
    });
    connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });
    connect(this, &PanelResults::finishedLoading, this, &PanelResults::onFinishedLoading);
    connect(mDockWidgetGraphSettings, &PanelGraphSettings::settingsChanged, [this]() { onColumnComboChanged(); });

    mWindowMain->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, mDockWidgetGraphSettings);
  }

  // CLASSIFICATION
  {
    mDockWidgetClassList = new PanelClassificationList(mWindowMain, &mFilter);
    mDockWidgetClassList->setVisible(false);
    connect(mDockWidgetClassList, &PanelClassificationList::settingsChanged, [this]() { refreshView(); });
    connect(mDockWidgetClassList, &QDockWidget::visibilityChanged, [&](bool visible) {
      if(!visible) {
        mClassSelector->setChecked(false);
      } else {
        mClassSelector->setChecked(true);
      }
    });

    mWindowMain->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, mDockWidgetClassList);

    mWindowMain->tabifyDockWidget(mDockWidgetClassList, mDockWidgetGraphSettings);
  }

  //
  // Breadcrump
  //
  auto *topBreadCrump = new QWidget();
  {
    auto *topBreadCrumpLayout = new QHBoxLayout();
    topBreadCrump->setLayout(topBreadCrumpLayout);
    topBreadCrump->setContentsMargins(0, 0, 0, 0);
    topBreadCrumpLayout->setContentsMargins(0, 0, 0, 0);

    //
    //
    //
    auto *grp    = new QButtonGroup();
    mTableButton = new QPushButton(generateSvgIcon("folder-table"), "");
    mTableButton->setCheckable(true);
    mTableButton->setChecked(true);
    grp->addButton(mTableButton);
    topBreadCrumpLayout->addWidget(mTableButton);

    mHeatmapButton = new QPushButton(generateSvgIcon("skg-chart-bubble"), "");
    mHeatmapButton->setCheckable(true);
    grp->addButton(mHeatmapButton);
    topBreadCrumpLayout->addWidget(mHeatmapButton);

    connect(mHeatmapButton, &QPushButton::clicked, [this](bool checked) {
      if(checked) {
        onShowHeatmap();
      }
    });
    connect(mTableButton, &QPushButton::clicked, [this](bool checked) {
      if(checked) {
        onShowTable();
      }
    });

    mBreadCrumpPlate = new QPushButton(generateSvgIcon("go-home"), "Plate");
    topBreadCrumpLayout->addWidget(mBreadCrumpPlate);
    connect(mBreadCrumpPlate, &QPushButton::clicked, [this]() { backTo(Navigation::PLATE); });

    mBreadCrumpWell = new QPushButton("Well (1)");
    topBreadCrumpLayout->addWidget(mBreadCrumpWell);
    connect(mBreadCrumpWell, &QPushButton::clicked, [this]() { backTo(Navigation::WELL); });

    mBreadCrumpImage = new QPushButton("Image (abcd.tif)");
    topBreadCrumpLayout->addWidget(mBreadCrumpImage);
    connect(mBreadCrumpImage, &QPushButton::clicked, [this]() { /*backTo(Navigation::IMAGE);*/ });

    // Open next level button
    mOpenNextLevel = new QPushButton(generateSvgIcon("go-next"), "");
    mOpenNextLevel->setStatusTip("Open selected wells/images");
    topBreadCrumpLayout->addWidget(mOpenNextLevel);
    connect(mOpenNextLevel, &QPushButton::clicked, [this]() {
      // std::vector<table::TableCell> selected;
      // for(const QModelIndex &index : selectedIndexes) {
      //   selected.emplace_back(mActListData.data(index.row(), 0));
      // }
      // openNextLevel(selected);
    });

    topBreadCrumpLayout->addStretch();
    topBreadCrump->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
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
    mDashboard = new Dashboard(this, windowMain);
  }

  //
  // Add to layout
  //
  auto *tab = layout().addTab(
      "", [] {}, false, 0);
  auto *col = tab->addVerticalPanel();
  col->setContentsMargins(0, 6, 0, 0);
  col->setSpacing(4);
  col->addWidget(topBreadCrump);
  col->addWidget(topInfo);
  col->addWidget(mGraphContainer.get());
  col->addWidget(mDashboard);

  onShowTable();
  refreshView();

  connect(mDockWidgetImagePreview, &DialogImageViewer::tileClicked, [this] { loadPreview(); });
  connect(mDockWidgetImagePreview, &DialogImageViewer::onSettingChanged, [this] {
    if(mFilter.getFilter().tStack != mDockWidgetImagePreview->getActualTimeStackPosition()) {
      // If t stack has been changed, reload the results with the new t-stack
      refreshView();
    }
    loadPreview();
  });
}

PanelResults::~PanelResults()
{
}

void PanelResults::valueChangedEvent()
{
}

void PanelResults::setHeatmapVisible(bool visible)
{
  mGraphContainer->setVisible(visible);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelResults::setActive(bool active)
{
  if(!active) {
    showToolBar(false);
    mDockWidgetImagePreview->setPlayBackToolbarVisible(false);
    mDockWidgetImagePreview->setVisible(false);
    mDockWidgetImagePreview->resetImage();
    mDockWidgetGraphSettings->setVisible(false);
    mDockWidgetClassList->setVisible(false);
    resetSettings();
    refreshView();
    mIsActive = active;
    mWindowMain->setSideBarVisible(true);
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::resetSettings()
{
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
void PanelResults::createToolBar(joda::ui::gui::helper::LayoutGenerator *toolbar)
{
  //
  // Open
  //
  auto *openBookmark = new QAction(generateSvgIcon("folder-open"), "Open", toolbar);
  connect(openBookmark, &QAction::triggered, [this]() { showOpenFileDialog(); });
  toolbar->addItemToTopToolbar(openBookmark);

  //
  // Export buttons
  //
  auto *exportMenu = new QMenu("Export");
  // text-csv
  auto *exportData = exportMenu->addAction(generateSvgIcon("x-office-spreadsheet"), "Save as XLSX");
  exportData->setToolTip("Export XLSX");
  connect(exportData, &QAction::triggered, [this]() { showFileSaveDialog("Excel 2007-365 (*.xlsx)"); });

  auto *exportR = exportMenu->addAction(generateSvgIcon("text-x-r"), "Save for R");
  exportR->setToolTip("Export R");
  connect(exportR, &QAction::triggered, [this]() { showFileSaveDialog("R-Script (*.r)"); });

  mExportSvg = exportMenu->addAction(generateSvgIcon("image-x-generic"), "Save as SVG");
  mExportSvg->setToolTip("Export SVG");
  mExportSvg->setVisible(false);
  connect(mExportSvg, &QAction::triggered, [this]() { showFileSaveDialog("SVG image (*.svg)"); });

  mExportPng = exportMenu->addAction(generateSvgIcon("image-png"), "Save as PNG");
  mExportPng->setToolTip("Export PNG");
  mExportPng->setVisible(false);
  connect(mExportPng, &QAction::triggered, [this]() { showFileSaveDialog("PNG image (*.png)"); });

  exportMenu->addSeparator();

  //
  auto *exports = new QAction(generateSvgIcon("folder-download"), "Export", toolbar);
  exports->setStatusTip("Export table or heatmap to R or Excel");
  connect(exports, &QAction::triggered, [this]() { showFileSaveDialog(); });

  exports->setMenu(exportMenu);
  toolbar->addItemToTopToolbar(exports);

  //
  // Copy button
  //
  auto *copyTable = new QAction(generateSvgIcon("edit-copy"), "Copy values", toolbar);
  connect(copyTable, &QAction::triggered, [this]() { mDashboard->copyToClipboard(); });
  copyTable->setStatusTip("Copy table to clipboard");
  toolbar->addItemToTopToolbar(copyTable);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  mClassSelector = new QAction(generateSvgIcon("edit-table-insert-column-right"), "");
  mClassSelector->setCheckable(true);
  mClassSelector->setChecked(true);
  mClassSelector->setToolTip("Class selector");
  connect(mClassSelector, &QAction::triggered, [this](bool checked) {
    if(checked) {
      mDockWidgetClassList->setVisible(true);
      mDockWidgetClassList->raise();    // Bring the dock widget to front
    } else {
      mDockWidgetClassList->setVisible(false);
    }
  });
  toolbar->addItemToTopToolbar(mClassSelector);
  /*
    auto *addColumn = new QAction(generateSvgIcon("edit-table-insert-column-right"), "");
    addColumn->setToolTip("Add column");
    connect(addColumn, &QAction::triggered, [this]() {
      columnEdit(-1);
      mDockWidgetClassList->fromSettings();
    });

    toolbar->addItemToTopToolbar(addColumn);

    mDeleteCol = new QAction(generateSvgIcon("edit-table-delete-column"), "");
    mDeleteCol->setToolTip("Delete column");
    connect(mDeleteCol, &QAction::triggered, [this]() {
      if(mSelectedTableColumnIdx >= 0) {
        auto colIdx = mActFilter.getColumn({.colIdx = mSelectedTableColumnIdx});
        mFilter.eraseColumn(colIdx);
        if(mAutoSort->isChecked()) {
          mFilter.sortColumns();
        }
        refreshView();
        mDockWidgetClassList->fromSettings();
      }
    });
    toolbar->addItemToTopToolbar(mDeleteCol);

    mEditCol = new QAction(generateSvgIcon("document-edit"), "");
    mEditCol->setToolTip("Edit column");
    connect(mEditCol, &QAction::triggered, [this]() {
      if(mSelectedTableColumnIdx >= 0) {
        columnEdit(mSelectedTableColumnIdx);
      }
    });
    toolbar->addItemToTopToolbar(mEditCol);
  */
  toolbar->addSeparatorToTopToolbar();
  mFilter.sortColumns();
  toolbar->addSeparatorToTopToolbar();

  //
  // Mark as invalid button
  //
  // edit-select-none
  // paint-none
  // video-off
  mMarkAsInvalid = new QAction(generateSvgIcon("gnumeric-autofilter-delete"), "");
  mMarkAsInvalid->setToolTip("Exclude selected image from statistics");
  mMarkAsInvalid->setCheckable(true);
  toolbar->addItemToTopToolbar(mMarkAsInvalid);
  mMarkAsInvalid->setEnabled(false);
  connect(mMarkAsInvalid, &QAction::triggered, this, &PanelResults::onMarkAsInvalidClicked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::storeResultsTableSettingsToDatabase()
{
  try {
    if(mIsActive && mAnalyzer != nullptr && mSelectedDataSet.analyzeMeta.has_value() && !mSelectedDataSet.analyzeMeta->jobId.empty()) {
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
void PanelResults::refreshBreadCrump()
{
  switch(mNavigation) {
    case Navigation::PLATE:
      mBreadCrumpWell->setVisible(false);
      mBreadCrumpImage->setVisible(false);
      mOpenNextLevel->setVisible(true);
      mDockWidgetImagePreview->setVisible(false);
      mDockWidgetImagePreview->setPlayBackToolbarVisible(false);
      mDockWidgetImagePreview->setFloating(false);
      mDockWidgetImagePreview->resetImage();
      mDockWidgetImagePreview->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());
      break;
    case Navigation::WELL:
      mBreadCrumpWell->setVisible(true);
      mBreadCrumpImage->setVisible(false);
      mOpenNextLevel->setVisible(true);
      mDockWidgetImagePreview->setVisible(false);
      mDockWidgetImagePreview->setPlayBackToolbarVisible(false);
      mDockWidgetImagePreview->setFloating(false);
      mDockWidgetImagePreview->resetImage();
      mDockWidgetImagePreview->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());
      if(mSelectedDataSet.groupMeta.has_value()) {
        auto platePos =
            "Well (" + std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX) + ")";
        mBreadCrumpWell->setText(platePos.data());
      }
      break;
    case Navigation::IMAGE:
      mBreadCrumpWell->setVisible(true);
      mBreadCrumpImage->setVisible(true);
      mOpenNextLevel->setVisible(false);
      mDockWidgetImagePreview->resetMaxtimeStacks();
      if(!mImageWorkingDirectory.empty() && mDashboard->isVisible()) {
        mDockWidgetImagePreview->setVisible(true);
      } else {
        mDockWidgetImagePreview->setVisible(false);
      }
      mDockWidgetImagePreview->setPlayBackToolbarVisible(true);

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
bool PanelResults::showSelectWorkingDir(const QString &path)
{
  QFileDialog dialog(mWindowMain);
  dialog.setWindowTitle("Select images Directory");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::ShowDirsOnly, true);
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
void PanelResults::loadPreview()
{
  if(!mGeneratePreviewMutex.try_lock()) {
    return;
  }

  if(mImageWorkingDirectory.empty()) {
    // No working directory selected. Make the image preview invisible
    mDockWidgetImagePreview->setVisible(false);
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
    QMessageBox msgBox(mWindowMain);
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
        if(mImageWorkingDirectory.empty()) {
          mDockWidgetImagePreview->setVisible(false);
        }
        mGeneratePreviewMutex.unlock();
        return;
      }
    } else if(msgBox.clickedButton() == dontAskAgainButton) {
      mDockWidgetImagePreview->setVisible(false);
      mImageWorkingDirectory.clear();
      mGeneratePreviewMutex.unlock();
      return;
    }
  }

  mGeneratePreviewMutex.unlock();
  std::thread([this, imagePath = imagePath]() {
    mLoadLock.lock();
    try {
      mDockWidgetImagePreview->setWaiting(true);
      int32_t tileWidth  = mSelectedDataSet.analyzeMeta->tileWidth;
      int32_t tileHeight = mSelectedDataSet.analyzeMeta->tileHeight;

      int32_t series     = mSelectedDataSet.analyzeMeta->series;
      int32_t resolution = 0;

      const auto &objectInfo = mSelectedDataSet.objectInfo.value();
      int32_t tileXNr        = objectInfo.measCenterX / tileWidth;
      int32_t tileYNr        = objectInfo.measCenterY / tileHeight;

      auto &previewResult = mDockWidgetImagePreview->getPreviewObject();

      auto log = std::to_string(tileXNr) + "," + std::to_string(tileYNr) + "," + std::to_string(tileWidth) + "," + std::to_string(tileHeight) + "," +
                 std::to_string(objectInfo.stackC);
      joda::log::logTrace("Preview for image >" + imagePath.string() + "< " + log);

      joda::ctrl::Controller::loadImage(imagePath, series,
                                        joda::image::reader::ImageReader::Plane{.z = static_cast<int32_t>(objectInfo.stackZ),
                                                                                .c = static_cast<int32_t>(objectInfo.stackC),
                                                                                .t = static_cast<int32_t>(objectInfo.stackT)},
                                        joda::ome::TileToLoad{tileXNr, tileYNr, tileWidth, tileHeight}, previewResult, mImgProps, objectInfo,
                                        mDockWidgetImagePreview->getSelectedZProjection());
      auto imgWidth    = mImgProps.getImageInfo(series).resolutions.at(0).imageWidth;
      auto imageHeight = mImgProps.getImageInfo(series).resolutions.at(0).imageHeight;
      if(imgWidth > tileWidth || imageHeight > tileHeight) {
        tileWidth  = tileWidth;
        tileHeight = tileHeight;
      } else {
        tileWidth  = imgWidth;
        tileHeight = imageHeight;
      }
      auto [tileNrX, tileNrY] = mImgProps.getImageInfo(series).resolutions.at(resolution).getNrOfTiles(tileWidth, tileHeight);

      auto measBoxX = objectInfo.measBoxX - tileXNr * tileWidth;
      auto measBoxY = objectInfo.measBoxY - tileYNr * tileHeight;
      QRect boungingBox{(int32_t) measBoxX, (int32_t) measBoxY, (int32_t) objectInfo.measBoxWidth, (int32_t) objectInfo.measBoxHeight};
      mDockWidgetImagePreview->setCrossHairCursorPositionAndCenter(boungingBox);
      mDockWidgetImagePreview->setThumbnailPosition(PanelImageView::ThumbParameter{.nrOfTilesX          = tileNrX,
                                                                                   .nrOfTilesY          = tileNrY,
                                                                                   .tileWidth           = tileWidth,
                                                                                   .tileHeight          = tileHeight,
                                                                                   .originalImageWidth  = imgWidth,
                                                                                   .originalImageHeight = imageHeight,
                                                                                   .selectedTileX       = tileXNr,
                                                                                   .selectedTileY       = tileYNr});
      mDockWidgetImagePreview->imageUpdated(previewResult.results, {});
    } catch(const std::exception &ex) {
      // No image selected
      joda::log::logError("Preview error: " + std::string(ex.what()));
    }
    mLoadLock.unlock();
    mDockWidgetImagePreview->setWaiting(false);
  }).detach();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::refreshView()
{
  const auto &wellOrder = mDockWidgetGraphSettings->getWellOrder();
  auto plateSize        = mDockWidgetGraphSettings->getPlateSize();
  uint16_t rows         = plateSize.height();
  uint16_t cols         = plateSize.width();

  auto form = static_cast<PanelResults::Navigation>(mNavigation) == PanelResults::Navigation::PLATE
                  ? joda::settings::DensityMapSettings::ElementForm::CIRCLE
                  : joda::settings::DensityMapSettings::ElementForm::RECTANGLE;

  mFilter.setFilter({.plateId = 0,
                     .groupId = static_cast<uint16_t>(mActGroupId),
                     .imageId = mActImageId,
                     .tStack  = mDockWidgetImagePreview->getActualTimeStackPosition()},
                    {.rows = static_cast<uint16_t>(rows), .cols = static_cast<uint16_t>(cols), .wellImageOrder = wellOrder},
                    {.form               = form,
                     .heatmapRangeMode   = mFilter.getDensityMapSettings().heatmapRangeMode,
                     .heatmapRangeMin    = mFilter.getDensityMapSettings().heatmapRangeMin,
                     .heatmapRangeMax    = mFilter.getDensityMapSettings().heatmapRangeMax,
                     .densityMapAreaSize = static_cast<int32_t>(mDockWidgetGraphSettings->getDensityMapSize())});

  //
  //
  //
  if(mIsActive && mAnalyzer && !mIsLoading) {
    mIsLoading = true;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::thread([this, rows, cols, wellOrder = wellOrder] {
      try {
        joda::log::logTrace("Start refreshing view ...");

        std::lock_guard<std::mutex> lock(mLoadLock);
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
              mActGroupId                = getID;
              mSelectedWellId            = getID;
              mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(getID);
              mFilter.setFilter({.plateId = 0,
                                 .groupId = static_cast<uint16_t>(mActGroupId),
                                 .imageId = mActImageId,
                                 .tStack  = mDockWidgetImagePreview->getActualTimeStackPosition()},
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
void PanelResults::onFinishedLoading()
{
  auto i = DurationCount::start("tbl");
  mDashboard->tableToQWidgetTable(mActListData, mTmpColocClasses, mNavigation == Navigation::IMAGE);
  DurationCount::stop(i);

  tableToHeatmap(mActListData);
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
        rows = static_cast<int32_t>(std::ceil((float) mSelectedDataSet.imageMeta->height / (float) densityMapSize));
        cols = static_cast<int32_t>(std::ceil((float) mSelectedDataSet.imageMeta->width / (float) densityMapSize));
      } else {
        rows           = 1;
        cols           = 1;
        densityMapSize = -1;
      }

      break;
  }

  auto data = joda::db::data::convertToHeatmap(mActListData, rows, cols, mDockWidgetGraphSettings->getSelectedColumn(),
                                               joda::db::data::PlotPlateSettings{.densityMapSize = densityMapSize});
  mGraphContainer->updateGraph(std::move(data));

  refreshBreadCrump();
  update();
  QApplication::restoreOverrideCursor();
}

///
/// \brief      Export image
/// \author     Joachim Danmayr
///
void PanelResults::onExportImageClicked()
{
  /*
  cv::Rect rectangle;
  rectangle.x      = mSelectedAreaPos.y * mFilter.densityMapAreaSize;    // Images are mirrored in the coordinates
  rectangle.y      = mSelectedAreaPos.x * mFilter.densityMapAreaSize;    // Images are mirrored in the coordinates
  rectangle.width  = mFilter.densityMapAreaSize;
  rectangle.height = mFilter.densityMapAreaSize;

  auto retImage = joda::results::analyze::plugins::ControlImage::getControlImage(
      *mAnalyzer, mActImageId, mFilter.channelIdx, mSelectedTileId, rectangle);

  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", mAnalyzer->getBasePath().string().data(), "PNG Files (*.png)");
  if(filePath.isEmpty()) {
    return;
  }
  if(!filePath.endsWith(".png")) {
    filePath += ".png";
  }
  bool isSuccess = cv::imwrite(filePath.toStdString(), retImage);

  QDesktopServices::openUrl(QUrl("file:///" + filePath));
  */
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::onMarkAsInvalidClicked(bool isInvalid)
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
void PanelResults::setSelectedElement(table::TableCell value)
{
  switch(mNavigation) {
    case Navigation::PLATE: {
      mSelectedWellId            = value.getId();
      mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(value.getId());
      mSelectedDataSet.imageMeta.reset();
      mSelectedDataSet.objectInfo.reset();
      mMarkAsInvalid->setEnabled(false);

      // Act data
      auto platePos = std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX);
      mSelectedRowInfo->setText(platePos.data());
    } break;
    case Navigation::WELL: {
      if(!mSelectedDataSet.groupMeta.has_value()) {
      }

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
      mMarkAsInvalid->setEnabled(true);

      // Act data
      auto platePos = std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX) + "/" +
                      imageInfo.filename;
      mSelectedRowInfo->setText(platePos.data());
    }

    break;
    case Navigation::IMAGE:
      mSelectedTileId = value.getObjectId();
      mMarkAsInvalid->setEnabled(false);

      if(mSelectedTileId >= 0) {
        mSelectedDataSet.objectInfo = mAnalyzer->selectObjectInfo(mSelectedTileId);
        if(mSelectedDataSet.imageMeta->imageId != mSelectedDataSet.objectInfo->imageId) {
          mSelectedDataSet.imageMeta = mAnalyzer->selectImageInfo(mSelectedDataSet.objectInfo->imageId);
        }
      }

      auto rowImageName = mSelectedDataSet.imageMeta->filename;
      if(mActImageId.size() > 1) {
        rowImageName = value.getRowName();
      }
      auto platePos = std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX) + "/" +
                      rowImageName + "/" + std::to_string(value.getObjectId());
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
void PanelResults::openNextLevel(const std::vector<table::TableCell> &value)
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu++;
  if(actMenu <= 2) {
    mNavigation = static_cast<Navigation>(actMenu);
  } else {
    // An area has been selected within an image -> trigger an export
    onExportImageClicked();
    return;
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      break;
    case Navigation::WELL:
      if(!value.empty()) {
        mActGroupId = value.at(0).getId();
      }
      break;
    case Navigation::IMAGE:
      std::set<uint64_t> act;
      for(const auto &row : value) {
        act.emplace(row.getObjectId());
      }
      mActImageId = act;
      break;
  }
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::backTo(Navigation backTo)
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

void PanelResults::goHome()
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
void PanelResults::openFromFile(const QString &pathToDbFile)
{
  if(pathToDbFile.isEmpty()) {
    return;
  }
  resetSettings();
  mAnalyzer = std::make_unique<joda::db::Database>();
  mAnalyzer->openDatabase(std::filesystem::path(pathToDbFile.toStdString()));
  mDbFilePath = std::filesystem::path(pathToDbFile.toStdString());

  // We assume the images to be in the folder ../../../<IMAGES>
  // If not the user will be asked to select the image working directory.
  mImageWorkingDirectory = mDbFilePath.parent_path().parent_path().parent_path();

  mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  mColumnEditDialog->updateClassesAndClasses(mAnalyzer.get());
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
  mDockWidgetImagePreview->setMaxTimeStacks(mAnalyzer->selectNrOfTimeStacks());

  // Make visible
  showToolBar(true);
  mIsActive = true;
  mWindowMain->setSideBarVisible(false);
  mDockWidgetClassList->setDatabase(mAnalyzer.get());
  if(mClassSelector->isChecked()) {
    mDockWidgetClassList->setVisible(true);
  }

  refreshView();

  if(mSelectedDataSet.analyzeMeta.has_value()) {
    getWindowMain()->addToLastLoadedResults(pathToDbFile, mSelectedDataSet.analyzeMeta->jobName.data());
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onShowTable()
{
  if(mExportSvg != nullptr) {
    mExportSvg->setVisible(false);
    mExportPng->setVisible(false);
  }

  mDashboard->setVisible(true);
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
void PanelResults::onShowHeatmap()
{
  if(mExportSvg != nullptr) {
    mExportSvg->setVisible(true);
    mExportPng->setVisible(true);
  }
  mDashboard->setVisible(false);
  mDockWidgetGraphSettings->setVisible(true);
  mDockWidgetGraphSettings->raise();    // Make it the active tab
  mDockWidgetImagePreview->setVisible(false);
  setHeatmapVisible(true);
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///

//  // mActHeatmapData.at(mDockWidgetImagePreview->getActualTimeStackPosition()).at(mColumn->currentData().toInt())
void PanelResults::tableToHeatmap(const std::shared_ptr<joda::table::Table> table)
{
  if(mAnalyzer) {
    mDockWidgetGraphSettings->setColumns(mActFilter.getColumns());
  }
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::paintEmptyHeatmap()
{
  joda::table::Table table;
  const auto &wellOrder = mFilter.getPlateSetup().wellImageOrder;
  uint16_t rows         = mFilter.getPlateSetup().rows;
  uint16_t cols         = mFilter.getPlateSetup().cols;
  for(int row = 0; row < rows; row++) {
    for(int col = 0; col < cols; col++) {
#warning "What shall we do"
      // table.getMutableColHeader()[col] = "";
      table::TableCell data;
      table.setData(row, col, data);
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
void PanelResults::createEditColumnDialog()
{
  mColumnEditDialog = new DialogColumnSettings(&mFilter, mWindowMain);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::columnEdit(int32_t colIdx)
{
  if(colIdx >= 0) {
    mColumnEditDialog->exec(mActFilter.getColumn({.colIdx = colIdx}), false);
  } else {
    mColumnEditDialog->exec({}, true);
  }
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
void PanelResults::onColumnComboChanged()
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
void PanelResults::showOpenFileDialog()
{
  std::filesystem::path filePath = mDbFilePath.parent_path();

  QString filename = QFileDialog::getOpenFileName(this, "Open File", filePath.string().data(),
                                                  "ImageC results files (*" + QString(joda::fs::EXT_DATABASE.data()) + ")");
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
void PanelResults::showFileSaveDialog(const QString &filter)
{
  QString templatePath = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();

  auto getEndianFromFilter = [](const QString &filter) -> std::string {
    std::string endian;
    if(filter.contains("(*.xlsx)")) {
      endian = ".xlsx";
    } else if(filter.contains("(*.r)")) {
      endian = ".r";
    } else if(filter.contains("(*.csv)")) {
      endian = ".csv";
    } else if(filter.contains("(*.svg)")) {
      endian = ".svg";
    } else if(filter.contains("(*.png)")) {
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
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", filePath.string().data(), filter, &selectedFilter);
  std::string filename           = filePathOfSettingsFile.toStdString();
  auto selectedEndian            = getEndianFromFilter(selectedFilter);
  if(!filename.ends_with(selectedEndian)) {
    filename += selectedEndian;
  }

  // Select save option
  if(filename.ends_with(".xlsx")) {
    saveData(filename, joda::ctrl::ExportSettings::ExportType::XLSX);
  } else if(filename.ends_with(".r")) {
    saveData(filename, joda::ctrl::ExportSettings::ExportType::R);
  } else if(filename.ends_with(".svg")) {
    // mHeatmapChart->exportToSVG(filename.data());
  } else if(filename.ends_with(".png")) {
    // mHeatmapChart->exportToPNG(filename.data());
  }
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::saveData(const std::string &fileName, joda::ctrl::ExportSettings::ExportType format)
{
  if(fileName.empty()) {
    return;
  }

  std::thread([this, fileName, format] {
    joda::settings::AnalyzeSettings settings;
    try {
      settings = nlohmann::json::parse(mSelectedDataSet.analyzeMeta->analyzeSettingsJsonString);
    } catch(const std::exception &ex) {
      joda::log::logWarning("Could not parse settings from database. Reason: " + std::string(ex.what()));
    }

    if(format == joda::ctrl::ExportSettings::ExportType::XLSX) {
      if(mGraphContainer->isVisible()) {
        // Export heatmap
        /*joda::exporter::xlsx::Exporter::startExport(mActHeatmapData, settings, mSelectedDataSet.analyzeMeta->jobName,
                                                    mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                    fileName);*/
      } else {
        joda::exporter::xlsx::Exporter::startExport(mDashboard->getExportables(), settings, mSelectedDataSet.analyzeMeta->jobName,
                                                    mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                    fileName);
      }
    } else {
      joda::exporter::r::Exporter::startExport(mDashboard->getExportables(), settings, mSelectedDataSet.analyzeMeta->jobName,
                                               mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish, fileName);
    }

    QString folderPath = std::filesystem::path(fileName).parent_path().string().data();
    QDesktopServices::openUrl(QUrl("file:///" + folderPath));
  }).detach();
}

}    // namespace joda::ui::gui
