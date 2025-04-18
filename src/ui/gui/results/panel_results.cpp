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
#include <qevent.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <exception>
#include <filesystem>
#include <iostream>
#include <locale>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap.hpp"
#include "backend/helper/database/exporter/heatmap/export_heatmap_settings.hpp"
#include "backend/helper/database/exporter/r/exporter_r.hpp"
#include "backend/helper/database/exporter/xlsx/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/filter.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "ui/gui/container/container_button.hpp"
#include "ui/gui/container/container_label.hpp"
#include "ui/gui/container/panel_edit_base.hpp"
#include "ui/gui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/widget_generator.hpp"
#include "ui/gui/window_main/panel_results_info.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>
#include "dialog_column_settings.hpp"

namespace joda::ui::gui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelResults::PanelResults(WindowMain *windowMain) : PanelEdit(windowMain, nullptr, false, windowMain), mWindowMain(windowMain)
{
  // Drop downs
  createEditColumnDialog();
  createToolBar(&layout());

  //
  // Create Table
  //
  mTable = new PlaceholderTableWidget();
  mTable->setPlaceholderText("Click >Add column< to start.");
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
  mTable->verticalHeader()->setDefaultSectionSize(8);    // Set each row to 50 pixels height

  connect(mTable, &QTableWidget::currentCellChanged, this, &PanelResults::onTableCurrentCellChanged);
  connect(mTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
          [this](int logicalIndex) { onOpenNextLevel(logicalIndex, 0, mSelectedTable.data(logicalIndex, 0)); });

  connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) { onOpenNextLevel(row, 0, mSelectedTable.data(row, 0)); });
  connect(mTable, &QTableWidget::cellClicked, this, &PanelResults::onCellClicked);

  static const int32_t SELECTED_INFO_WIDTH   = 250;
  static const int32_t SELECTED_INFO_SPACING = 6;
  //
  // Heatmap
  //
  {
    mHeatmapContainer = new QHBoxLayout();
    mHeatmapContainer->setContentsMargins(0, 0, 0, 0);
    mHeatmapChart = new ChartHeatMap(this, mFilter);
    mHeatmapChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(mHeatmapChart, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
    connect(mHeatmapChart, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
    connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });
    connect(getWindowMain()->getPanelResultsInfo(), &joda::ui::gui::PanelResultsInfo::settingsChanged, [this]() {
      if(mIsActive) {
        refreshView();
      }
    });
    connect(this, &PanelResults::finishedLoading, this, &PanelResults::onFinishedLoading);

    auto *heatmapSidebar = new QWidget();
    heatmapSidebar->setContentsMargins(0, SELECTED_INFO_SPACING, SELECTED_INFO_SPACING, 0);
    heatmapSidebar->setMaximumWidth(SELECTED_INFO_WIDTH + SELECTED_INFO_SPACING);
    heatmapSidebar->setMinimumWidth(SELECTED_INFO_WIDTH + SELECTED_INFO_SPACING);
    auto *formLayout = new QVBoxLayout;
    formLayout->setContentsMargins(0, 0, 0, 0);
    heatmapSidebar->setLayout(formLayout);

    //
    // Column selector
    //
    mColumn = new QComboBox();
    connect(mColumn, &QComboBox::currentIndexChanged, this, &PanelResults::onColumnComboChanged);
    formLayout->addWidget(mColumn);
    formLayout->addLayout(std::get<2>(createHelpTextLabel("Table column to show", 0)));

    //
    // Well order matrix
    //
    mWellOrderMatrix = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");
    formLayout->addWidget(mWellOrderMatrix);
    formLayout->addLayout(std::get<2>(createHelpTextLabel("Well order matrix", 0)));
    connect(mWellOrderMatrix, &QLineEdit::editingFinished, [this]() { refreshView(); });

    //
    // Plate size
    //
    mPlateSize = new QComboBox();
    mPlateSize->addItem("1", 1);
    mPlateSize->addItem("2 x 3", 203);
    mPlateSize->addItem("3 x 4", 304);
    mPlateSize->addItem("4 x 6", 406);
    mPlateSize->addItem("6 x 8", 608);
    mPlateSize->addItem("8 x 12", 812);
    mPlateSize->addItem("16 x 24", 1624);
    mPlateSize->addItem("32 x 48", 3248);
    mPlateSize->addItem("48 x 72", 4872);
    formLayout->addWidget(mPlateSize);
    formLayout->addLayout(std::get<2>(createHelpTextLabel("Plate size", 0)));
    connect(mPlateSize, &QComboBox::currentIndexChanged, [this](int32_t index) { refreshView(); });

    //
    // Density map
    //
    mDensityMapSize = new QComboBox();
    mDensityMapSize->addItem("50", 50);
    mDensityMapSize->addItem(std::to_string(4096 / 64).data(), 4096 / 64);
    mDensityMapSize->addItem("100", 100);
    mDensityMapSize->addItem(std::to_string(4096 / 32).data(), 4096 / 32);
    mDensityMapSize->addItem("150", 150);
    mDensityMapSize->addItem("200", 200);
    mDensityMapSize->addItem("250", 250);
    mDensityMapSize->addItem(std::to_string(4096 / 16).data(), 4096 / 16);
    mDensityMapSize->addItem("300", 300);
    mDensityMapSize->addItem("350", 350);
    mDensityMapSize->addItem("400", 400);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem(std::to_string(4096 / 8).data(), 4096 / 8);
    mDensityMapSize->addItem("1000", 1000);
    mDensityMapSize->addItem(std::to_string(4096 / 4).data(), 4096 / 4);
    mDensityMapSize->addItem("2000", 2000);
    mDensityMapSize->addItem(std::to_string(4096 / 2).data(), 4096 / 2);
    mDensityMapSize->addItem("3000", 3000);
    mDensityMapSize->addItem("4000", 4000);
    mDensityMapSize->addItem(std::to_string(4096).data(), 4096);
    mDensityMapSize->setCurrentIndex(mDensityMapSize->count() - 1);
    connect(mDensityMapSize, &QComboBox::currentIndexChanged, [this](int32_t index) { refreshView(); });
    formLayout->addWidget(mDensityMapSize);
    formLayout->addLayout(std::get<2>(createHelpTextLabel("Density map size", 0)));

    formLayout->addStretch();

    mHeatmapContainer->addWidget(heatmapSidebar);
    mHeatmapContainer->addWidget(mHeatmapChart);
  }

  //
  // Breadcrump
  //
  auto *topBreadCrump = new QHBoxLayout();
  {
    //
    //
    //
    auto *grp    = new QButtonGroup();
    mTableButton = new QPushButton(generateSvgIcon("folder-table"), "");
    mTableButton->setCheckable(true);
    mTableButton->setChecked(true);
    grp->addButton(mTableButton);
    topBreadCrump->addWidget(mTableButton);

    mHeatmapButton = new QPushButton(generateSvgIcon("skg-chart-bubble"), "");
    mHeatmapButton->setCheckable(true);
    grp->addButton(mHeatmapButton);
    topBreadCrump->addWidget(mHeatmapButton);

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
    topBreadCrump->addWidget(mBreadCrumpPlate);
    connect(mBreadCrumpPlate, &QPushButton::clicked, [this]() { backTo(Navigation::PLATE); });

    mBreadCrumpWell = new QPushButton("Well (1)");
    topBreadCrump->addWidget(mBreadCrumpWell);
    connect(mBreadCrumpWell, &QPushButton::clicked, [this]() { backTo(Navigation::WELL); });

    mBreadCrumpImage = new QPushButton("Image (abcd.tif)");
    topBreadCrump->addWidget(mBreadCrumpImage);
    connect(mBreadCrumpImage, &QPushButton::clicked, [this]() { /*backTo(Navigation::IMAGE);*/ });

    topBreadCrump->addStretch();
  }

  //
  // Top infor widget
  //
  QLayout *topInfoLayout = new QHBoxLayout();
  {
    topInfoLayout->setSpacing(SELECTED_INFO_SPACING);

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
  }

  //
  // Add to layout
  //
  auto *tab = layout().addTab(
      "", [] {}, false, 0);
  auto *col = tab->addVerticalPanel();
  col->setContentsMargins(0, 6, 0, 0);
  col->setSpacing(4);
  col->addLayout(topBreadCrump);
  col->addLayout(topInfoLayout);
  col->addLayout(mHeatmapContainer);
  col->addWidget(mTable);

  onShowTable();
  refreshView();
}

PanelResults::~PanelResults()
{
}

void PanelResults::valueChangedEvent()
{
}

void PanelResults::setHeatmapVisible(bool visible)
{
  for(int i = 0; i < mHeatmapContainer->count(); ++i) {
    QWidget *w = mHeatmapContainer->itemAt(i)->widget();
    if(w) {
      w->setVisible(visible);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelResults::setActive(bool active)
{
  if(!active) {
    showToolBar(false);
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
  mTable->setVisible(true);
  setHeatmapVisible(false);

  mTableButton->blockSignals(false);
  mHeatmapButton->blockSignals(false);
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
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

  auto *exportR = exportMenu->addAction(generateSvgIcon("text-x-r"), "Save as R");
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
  auto *saveAsTemplate = exportMenu->addAction(generateSvgIcon("document-save-as-template"), "Save table settings");
  saveAsTemplate->setWhatsThis("Save results table settings");
  connect(saveAsTemplate, &QAction::triggered,
          [this]() { showFileSaveDialog("ImageC results table settings (*" + QString(joda::fs::EXT_RESULTS_TABLE_SETTINGS.data()) + ")"); });

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
  connect(copyTable, &QAction::triggered, [this]() { copyTableToClipboard(mTable); });
  copyTable->setStatusTip("Copy table to clipboard");
  toolbar->addItemToTopToolbar(copyTable);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  mTemplateMenu           = new QMenu();
  auto *templateGenerator = new QAction(generateSvgIcon("document-new"), "", this);
  templateGenerator->setStatusTip("Open table column generation wizard");
  connect(templateGenerator, &QAction::triggered, [this]() {
    if(mColumnTemplate->exec() == 0) {
      mFilter = mFilterTemplate.toSettings(mAnalyzeSettingsMeta, mAnalyzer->selectClasses());
      refreshView();
    }
    loadTemplates();
  });
  templateGenerator->setMenu(mTemplateMenu);
  toolbar->addItemToTopToolbar(templateGenerator);

  //
  //
  //
  auto *addColumn = new QAction(generateSvgIcon("edit-table-insert-column-right"), "");
  addColumn->setToolTip("Add column");
  connect(addColumn, &QAction::triggered, [this]() { columnEdit(mTable->columnCount()); });

  toolbar->addItemToTopToolbar(addColumn);

  auto *deleteColumn = new QAction(generateSvgIcon("edit-table-delete-column"), "");
  deleteColumn->setToolTip("Delete column");
  connect(deleteColumn, &QAction::triggered, [this]() {
    if(mSelectedTableColumn >= 0) {
      mFilter.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableColumn});
      if(mAutoSort->isChecked()) {
        mFilter.sortColumns();
      }
      refreshView();
    }
  });
  toolbar->addItemToTopToolbar(deleteColumn);

  auto *editColumn = new QAction(generateSvgIcon("document-edit"), "");
  editColumn->setToolTip("Edit column");
  connect(editColumn, &QAction::triggered, [this]() {
    if(mSelectedTableColumn >= 0) {
      columnEdit(mSelectedTableColumn);
    }
  });
  toolbar->addItemToTopToolbar(editColumn);

  toolbar->addSeparatorToTopToolbar();

  mAutoSort = new QAction(generateSvgIcon("view-sort-ascending-name"), "", this);
  mAutoSort->setCheckable(true);
  mAutoSort->setChecked(true);
  mAutoSort->setStatusTip("Sort columns");
  toolbar->addItemToTopToolbar(mAutoSort);
  connect(mAutoSort, &QAction::triggered, [this]() {
    if(mAutoSort->isChecked()) {
      mFilter.sortColumns();
      refreshView();
    }
  });

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

  loadTemplates();
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
      break;
    case Navigation::WELL:
      mBreadCrumpWell->setVisible(true);
      mBreadCrumpImage->setVisible(false);
      if(mSelectedDataSet.groupMeta.has_value()) {
        auto platePos =
            "Well (" + std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX) + ")";
        mBreadCrumpWell->setText(platePos.data());
      }
      break;
    case Navigation::IMAGE:
      mBreadCrumpWell->setVisible(true);
      mBreadCrumpImage->setVisible(true);
      if(mSelectedDataSet.imageMeta.has_value()) {
        mBreadCrumpImage->setText("Image (" + QString(mSelectedDataSet.imageMeta->filename.data()) + ")");
      }
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
void PanelResults::refreshView()
{
  const auto &wellOrder = getWellOrder();
  auto plateSize        = getPlateSize();
  uint16_t rows         = plateSize.height();
  uint16_t cols         = plateSize.width();

  auto form = static_cast<PanelResults::Navigation>(mNavigation) == PanelResults::Navigation::PLATE
                  ? joda::settings::DensityMapSettings::ElementForm::CIRCLE
                  : joda::settings::DensityMapSettings::ElementForm::RECTANGLE;

  mFilter.setFilter({.plateId = 0, .groupId = mActGroupId, .imageId = mActImageId},
                    {.rows = static_cast<uint16_t>(rows), .cols = static_cast<uint16_t>(cols), .wellImageOrder = wellOrder},
                    {.form               = form,
                     .heatmapRangeMode   = mFilter.getDensityMapSettings().heatmapRangeMode,
                     .heatmapRangeMin    = mFilter.getDensityMapSettings().heatmapRangeMin,
                     .heatmapRangeMax    = mFilter.getDensityMapSettings().heatmapRangeMax,
                     .densityMapAreaSize = static_cast<int32_t>(getDensityMapSize())});

  //
  //
  //
  if(mIsActive && mAnalyzer && !mIsLoading) {
    mIsLoading = true;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    std::thread([this, rows, cols, wellOrder = wellOrder] {
      storeResultsTableSettingsToDatabase();
    REFRESH_VIEW:
      switch(mNavigation) {
        case Navigation::PLATE: {
          mActListData = joda::db::StatsPerGroup::toTable(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_PLATE);
          if(!mActListData.empty() && mActListData.at(0).getRows() == 1) {
            // If there are no groups, switch directly to well view
            mNavigation                = Navigation::WELL;
            auto getID                 = mActListData.at(0).data(0, 0).getId();
            mActGroupId                = static_cast<uint16_t>(getID);
            mSelectedWellId            = getID;
            mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(getID);
            mFilter.setFilter({.plateId = 0, .groupId = mActGroupId, .imageId = mActImageId},
                              {.rows = static_cast<uint16_t>(rows), .cols = static_cast<uint16_t>(cols), .wellImageOrder = wellOrder},
                              {.densityMapAreaSize = static_cast<int32_t>(getDensityMapSize())});
            goto REFRESH_VIEW;
          }
          if(!mTable->isVisible()) {
            mActHeatmapData = joda::db::StatsPerGroup::toHeatmap(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_PLATE);
          }
        } break;
        case Navigation::WELL: {
          if(mTable->isVisible()) {
            mActListData = joda::db::StatsPerGroup::toTable(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_WELL);
          } else {
            mActHeatmapData = joda::db::StatsPerGroup::toHeatmap(mAnalyzer.get(), mFilter, db::StatsPerGroup::Grouping::BY_WELL);
          }
        } break;
        case Navigation::IMAGE: {
          if(mTable->isVisible()) {
            mActListData = joda::db::StatsPerImage::toTable(mAnalyzer.get(), mFilter);
          } else {
            mActHeatmapData = joda::db::StatsPerImage::toHeatmap(mAnalyzer.get(), mFilter);
          }
        } break;
      }
      if(mSelection.contains(mNavigation)) {
        auto col = mSelection[mNavigation].col;
        auto row = mSelection[mNavigation].row;
        mTable->setCurrentCell(row, col);
      } else {
        mTable->setCurrentCell(0, 0);
      }
      mIsLoading = false;
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
  if(!mActListData.empty()) {
    tableToQWidgetTable(mActListData[0]);
  } else {
    mTable->setRowCount(0);
    mTable->setColumnCount(0);
  }

  if(!mActHeatmapData.empty() && mActHeatmapData.contains(mColumn->currentData().toInt())) {
    tableToHeatmap(mActHeatmapData[mColumn->currentData().toInt()]);
  } else {
    paintEmptyHeatmap();
  }
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
void PanelResults::onElementSelected(int cellX, int cellY, table::TableCell value)
{
  QString headerTxt = mTable->horizontalHeaderItem(cellX)->text();
  switch(mNavigation) {
    case Navigation::PLATE: {
      mSelectedWellId            = value.getId();
      mSelectedDataSet.groupMeta = mAnalyzer->selectGroupInfo(value.getId());
      mSelectedDataSet.imageMeta.reset();
      mMarkAsInvalid->setEnabled(false);

      // Act data
      auto platePos = std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX);
      mSelectedRowInfo->setText(platePos.data());
    } break;
    case Navigation::WELL: {
      if(!mSelectedDataSet.groupMeta.has_value()) {
      }

      mSelectedImageId = value.getId();

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
      mSelectedTileId = value.getId();
      mMarkAsInvalid->setEnabled(false);
      mSelectedAreaPos.setX(cellX);
      mSelectedAreaPos.setY(cellY);

      auto platePos = std::string(1, ((char) (mSelectedDataSet.groupMeta->posY - 1) + 'A')) + std::to_string(mSelectedDataSet.groupMeta->posX) + "/" +
                      mSelectedDataSet.imageMeta->filename + "/" + std::to_string(value.getId());
      mSelectedRowInfo->setText(platePos.data());
      break;
  }
  mSelectedValue->setText(QString::number(value.getVal()) + " | " + headerTxt);
  mSelectedDataSet.value = DataSet::Value{.value = value.getVal()};
}

///
/// \brief      Open the next deeper level form the element with given id
/// \author     Joachim Danmayr
///
void PanelResults::onOpenNextLevel(int cellX, int cellY, table::TableCell value)
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
      mActGroupId = static_cast<uint16_t>(value.getId());
      break;
    case Navigation::IMAGE:
      mActImageId = value.getId();
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
  showToolBar(true);
  mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  mColumnEditDialog->updateClassesAndClasses(mAnalyzer.get());
  // Try to load settings if available
  try {
    if(mSelectedDataSet.analyzeMeta.has_value()) {
      auto selectedClasses = mAnalyzer->selectClasses();
      // auto imageChannels   = mAnalyzer->selectImageChannels();
      try {
        mAnalyzeSettingsMeta = nlohmann::json::parse(mAnalyzer->selectAnalyzeSettingsMeta(mSelectedDataSet.analyzeMeta->jobId));
      } catch(const std::exception &ex) {
        joda::log::logWarning("Could not load analyze settings meta from database, use fallback! Got: " + std::string(ex.what()));

        std::set<joda::enums::ClassId> outClasses;
        std::map<enums::ClassId, std::set<int32_t>> measureChannels;
        std::map<enums::ClassId, std::set<enums::ClassId>> intersectingClasses;
        for(const auto &cl : selectedClasses) {
          outClasses.emplace(cl.first);
          measureChannels[cl.first] = mAnalyzer->selectMeasurementChannelsForClasss(cl.first);
        }
        for(const auto &cl : selectedClasses) {
          intersectingClasses[cl.first] = outClasses;
        }

        mAnalyzeSettingsMeta = {.outputClasses = outClasses, .intersectingClasses = intersectingClasses, .measuredChannels = measureChannels};
      }

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

      try {
        mFilterTemplate = nlohmann::json::parse(mAnalyzer->selectResultsTableTemplateSettings(mSelectedDataSet.analyzeMeta->jobId));
      } catch(const std::exception &ex) {
        joda::log::logWarning("Could not load results table template settings from database, use fallback! Got: " + std::string(ex.what()));

        if(mFilterTemplate.columns.empty()) {
          mFilterTemplate.columns = {
              {.measureChannels = {enums::Measurement::COUNT, enums::Measurement::INTERSECTING}, .stats = {enums::Stats::SUM, enums::Stats::AVG}},
              {.measureChannels = {enums::Measurement::AREA_SIZE}, .stats = {enums::Stats::SUM, enums::Stats::AVG}},
              {.measureChannels = {enums::Measurement::INTENSITY_AVG, enums::Measurement::INTENSITY_SUM},
               .stats           = {enums::Stats::SUM, enums::Stats::AVG}}};
        }
      }

      // If filter is empty load default settings
      if(mFilter.getColumns().empty()) {
        mFilter = mFilterTemplate.toSettings(mAnalyzeSettingsMeta, selectedClasses);
      }

      const auto &plateSetup = mFilter.getPlateSetup();
      setWellOrder(plateSetup.wellImageOrder);
      setPlateSize({plateSetup.cols, plateSetup.rows});
      setDensityMapSize(mFilter.getDensityMapSettings().densityMapAreaSize);
    }
  } catch(...) {
  }
  mIsActive = true;
  mWindowMain->setSideBarVisible(false);

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
void PanelResults::setWellOrder(const std::vector<std::vector<int32_t>> &wellOrder)
{
  mWellOrderMatrix->blockSignals(true);
  mWellOrderMatrix->setText(joda::settings::vectorToString(wellOrder).data());
  mWellOrderMatrix->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::setPlateSize(const QSize &size)
{
  mPlateSize->blockSignals(true);
  uint32_t plateSizeCoded = (size.height() * 100) + size.width();
  auto idx                = mPlateSize->findData(plateSizeCoded);
  if(idx >= 0) {
    mPlateSize->setCurrentIndex(idx);
  }
  mPlateSize->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::setDensityMapSize(uint32_t densityMapSize)
{
  mDensityMapSize->blockSignals(true);
  auto idx = mDensityMapSize->findData(densityMapSize);
  if(idx >= 0) {
    mDensityMapSize->setCurrentIndex(idx);
  }
  mDensityMapSize->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelResults::getWellOrder() const -> std::vector<std::vector<int32_t>>
{
  return joda::settings::stringToVector(mWellOrderMatrix->text().toStdString());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelResults::getPlateSize() const -> QSize
{
  auto value = mPlateSize->currentData().toUInt();
  QSize size;
  size.setWidth(value % 100);
  size.setHeight(value / 100);
  return size;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelResults::getDensityMapSize() const -> uint32_t
{
  return mDensityMapSize->currentData().toUInt();
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

  mTable->setVisible(true);
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
  mTable->setVisible(false);
  setHeatmapVisible(true);
  refreshView();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::tableToHeatmap(const joda::table::Table &table)
{
  if(mAnalyzer) {
    if(mSelectedTableColumn >= 0) {
      mHeatmapChart->setData(table, static_cast<int32_t>(mNavigation));
      return;
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
void PanelResults::paintEmptyHeatmap()
{
  joda::table::Table table;
  const auto &wellOrder = mFilter.getPlateSetup().wellImageOrder;
  uint16_t rows         = mFilter.getPlateSetup().rows;
  uint16_t cols         = mFilter.getPlateSetup().cols;
  for(int row = 0; row < rows; row++) {
    table.getMutableRowHeader()[row] = "";
    for(int col = 0; col < cols; col++) {
      table.getMutableColHeader()[col] = "";
      table::TableCell data;
      table.setData(row, col, data);
    }
  }

  mHeatmapChart->setData(table, static_cast<int32_t>(mNavigation));
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
  mColumnTemplate   = new DialogColumnTemplate(&mFilterTemplate, mWindowMain);
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
  mColumnEditDialog->exec(colIdx);
  if(mAutoSort->isChecked()) {
    mFilter.sortColumns();
  }
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::tableToQWidgetTable(const joda::table::Table &tableIn)
{
  std::lock_guard<std::mutex> lock(mSelectMutex);
  mSelectedTable = tableIn;
  if(tableIn.getCols() > 0) {
    mTable->setColumnCount(tableIn.getCols());
    if(tableIn.getRows() == 0) {
      // We print at least one empty row
      mTable->setRowCount(1);
    } else {
      mTable->setRowCount(tableIn.getRows());
    }

  } else {
    mTable->setColumnCount(0);
    mTable->setRowCount(0);
  }

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
    widget->setStatusTip(data);
    return widget;
  };

  mColumn->blockSignals(true);
  auto actData = mColumn->currentData();
  mColumn->clear();
  // Header
  for(int col = 0; col < mTable->columnCount(); col++) {
    char txt      = col + 'A';
    auto colCount = QString(std::string(1, txt).data());

    if(tableIn.getCols() > col) {
      QString headerText = tableIn.getColHeader(col).data();
      mColumn->addItem(headerText, col);
      headerText = headerText.replace("[", "\n[");
      mTable->setHorizontalHeaderItem(col, createTableWidget(headerText));

    } else {
      mTable->setHorizontalHeaderItem(col, createTableWidget(colCount));
      mColumn->addItem(colCount, col);
    }
  }
  auto idx = mColumn->findData(actData);
  if(idx >= 0) {
    mColumn->setCurrentIndex(idx);
  }
  mColumn->blockSignals(false);

  // Row
  for(int row = 0; row < mTable->rowCount(); row++) {
    if(tableIn.getRows() > row) {
      mTable->setVerticalHeaderItem(row, createTableWidget(tableIn.getRowHeader(row).data()));
    } else {
      mTable->setVerticalHeaderItem(row, createTableWidget(QString(std::to_string(row).data())));
    }
  }

  for(int col = 0; col < mTable->columnCount(); col++) {
    for(int row = 0; row < mTable->rowCount(); row++) {
      QTableWidgetItem *item = mTable->item(row, col);
      if(item == nullptr) {
        item = createTableWidget(" ");
        mTable->setItem(row, col, item);
      }
      if(item) {
        if(tableIn.getRows() > row && tableIn.getCols() > col) {
          if(tableIn.data(row, col).isNAN()) {
            item->setText("-");
          } else {
            item->setText(QString::number((double) tableIn.data(row, col).getVal()));
          }
          QFont font = item->font();
          font.setStrikeOut(!tableIn.data(row, col).isValid());
          item->setFont(font);
        } else {
          item->setText(" ");
        }
      }
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
void PanelResults::onColumnComboChanged()
{
  onTableCurrentCellChanged(mSelectedTableRow, mColumn->currentData().toInt(), mSelectedTableRow, mSelectedTableColumn);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  mSelectedTableColumn = currentColumn;
  mSelectedTableRow    = currentRow;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onCellClicked(int rowSelected, int columnSelcted)
{
  // Update table
  mSelection[mNavigation] = {rowSelected, columnSelcted};
  auto selectedData       = mActListData.at(0).data(rowSelected, columnSelcted);
  onElementSelected(columnSelcted, rowSelected, selectedData);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::copyTableToClipboard(QTableWidget *table)
{
  QStringList data;
  QStringList header;
  for(int row = 0; row < table->rowCount(); ++row) {
    QStringList rowData;
    for(int col = 0; col < table->columnCount(); ++col) {
      if(row == 0) {
        header << table->horizontalHeaderItem(col)->text().replace("\n", " ");
      }
      if(col == 0) {
        rowData << table->verticalHeaderItem(row)->text().replace("\n", " ");
      }
      rowData << table->item(row, col)->text();
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = "\t" + header.join("\t") + "\n" + data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
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
                                                  "ImageC results or template files (*" + QString(joda::fs::EXT_DATABASE.data()) + " *" +
                                                      QString(joda::fs::EXT_RESULTS_TABLE_SETTINGS.data()) + ");;ImageC results files (*" +
                                                      QString(joda::fs::EXT_DATABASE.data()) + ");;ImageC results table settings (*" +
                                                      QString(joda::fs::EXT_RESULTS_TABLE_SETTINGS.data()) + ");;ImageC results table template (*" +
                                                      QString(joda::fs::EXT_RESULTS_TABLE_TEMPLATE.data()) + ")");
  // Select save option
  if(filename.endsWith(joda::fs::EXT_DATABASE.data())) {
    openFromFile(filename);
  } else if(filename.endsWith(joda::fs::EXT_RESULTS_TABLE_SETTINGS.data())) {
    openTableSettings(filename.toStdString());
  } else if(filename.endsWith(joda::fs::EXT_RESULTS_TABLE_TEMPLATE.data())) {
    openTemplate(filename);
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
    } else if(filter.contains("(*" + QString(joda::fs::EXT_RESULTS_TABLE_SETTINGS.data()) + ")")) {
      endian = joda::fs::EXT_RESULTS_TABLE_SETTINGS;
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
    mHeatmapChart->exportToSVG(filename.data());
  } else if(filename.ends_with(".png")) {
    mHeatmapChart->exportToPNG(filename.data());
  } else if(filename.ends_with(joda::fs::EXT_RESULTS_TABLE_SETTINGS)) {
    saveTemplate(filename);
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
    if(format == joda::ctrl::ExportSettings::ExportType::XLSX) {
      if(!mTable->isVisible()) {
        joda::db::BatchExporter::startExportHeatmap(mActHeatmapData, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                                    mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                    fileName);
      } else {
        joda::db::BatchExporter::startExportList(mActListData, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                                 mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                 fileName);
      }
    } else {
      db::StatsPerGroup::Grouping grouping = db::StatsPerGroup::Grouping::BY_PLATE;
      switch(mNavigation) {
        case Navigation::PLATE:
          grouping = db::StatsPerGroup::Grouping::BY_PLATE;
          break;
        case Navigation::WELL:
          grouping = db::StatsPerGroup::Grouping::BY_WELL;
          break;
        case Navigation::IMAGE:
          grouping = db::StatsPerGroup::Grouping::BY_IMAGE;
          break;
      }
      joda::db::RExporter::startExport(mFilter, grouping, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                       mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish, fileName);
    }

    QString folderPath = std::filesystem::path(fileName).parent_path().string().data();
    QDesktopServices::openUrl(QUrl("file:///" + folderPath));
  }).detach();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::saveTemplate(const std::string &fileName)
{
  if(fileName.empty()) {
    return;
  }
  nlohmann::json json = mFilter;
  joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(fileName), joda::fs::EXT_RESULTS_TABLE_SETTINGS);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::openTableSettings(const std::string &pathToOpenFileFrom)
{
  if(pathToOpenFileFrom.empty()) {
    return;
  }
  try {
    auto json      = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(pathToOpenFileFrom));
    mFilter        = json;
    auto *resPanel = getWindowMain()->getPanelResultsInfo();
    refreshView();
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-error").pixmap(48, 48));
    messageBox.setWindowTitle("Error...");
    messageBox.setText("Error in opening template got >" + QString(ex.what()) + "<.");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {"templates/results", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_RESULTS_TABLE_TEMPLATE);

  mTemplateMenu->clear();
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateMenu->addSeparator();
        }
      }
      QAction *action;
      if(!data.icon.isNull()) {
        action = mTemplateMenu->addAction(QIcon(data.icon.scaled(28, 28)), data.title.data());
      } else {
        action = mTemplateMenu->addAction(generateSvgIcon("favorite"), data.title.data());
      }
      connect(action, &QAction::triggered, [this, path = data.path]() { openTemplate(path.data()); });
    }
    addedPerCategory = dataInCategory.size();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::openTemplate(const QString &path)
{
  try {
    joda::settings::ResultsTemplate settings = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(path.toStdString()));
    mFilterTemplate                          = settings;
    mFilter                                  = settings.toSettings(mAnalyzeSettingsMeta, mAnalyzer->selectClasses());
    refreshView();
  } catch(const std::exception &ex) {
    joda::log::logWarning("Could not load template >" + path.toStdString() + "<. What: " + std::string(ex.what()));
  }
}

}    // namespace joda::ui::gui
