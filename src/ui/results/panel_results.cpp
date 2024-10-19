///
/// \file      panel_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_results.hpp"
#include <qaction.h>
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
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
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/database.hpp"
#include "backend/helper/database/exporter/xlsx/exporter.hpp"
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/window_main/panel_results_info.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelResults::PanelResults(WindowMain *windowMain) : PanelEdit(windowMain, nullptr, false), mWindowMain(windowMain)
{
  // Drop downs
  createBreadCrump(&layout());
  auto *navigationBar = new QToolBar();
  createNavigationBar(navigationBar);

  //
  // Create Table
  //
  mTable = new QTableWidget();
  mTable->setRowCount(1);
  mTable->setColumnCount(1);
  mTable->verticalHeader()->setDefaultSectionSize(8);    // Set each row to 50 pixels height

  connect(mTable, &QTableWidget::currentCellChanged, this, &PanelResults::onTableCurrentCellChanged);
  connect(mTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
          [this](int logicalIndex) { onOpenNextLevel(logicalIndex, 0, mSelectedTable.data(logicalIndex, 0)); });

  connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) { onOpenNextLevel(row, 0, mSelectedTable.data(row, 0)); });
  connect(mTable, &QTableWidget::cellClicked, this, &PanelResults::onCellClicked);

  // Middle layout
  auto *tab  = layout().addTab("", [] {});
  auto *col  = tab->addVerticalPanel();
  mHeatmap01 = new ChartHeatMap(this);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
  connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
  connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });

  col->setContentsMargins(0, 0, 0, 0);
  col->setSpacing(0);
  col->addWidget(navigationBar);
  col->addWidget(mHeatmap01);
  col->addWidget(mTable);

  onShowTable();
  repaintHeatmap();
}

void PanelResults::valueChangedEvent()
{
}

///
/// \brief
/// \author     Joachim Danmayr
///
void PanelResults::setActive(bool active)
{
  if(!active) {
    getWindowMain()->getPanelResultsInfo()->setData({});
    mAnalyzer.reset();
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::createBreadCrump(joda::ui::helper::LayoutGenerator *toolbar)
{
  auto *grp          = new QActionGroup(toolbar);
  auto *mTableButton = new QAction(generateIcon("table"), "");
  mTableButton->setCheckable(true);
  mTableButton->setChecked(true);
  grp->addAction(mTableButton);
  toolbar->addItemToTopToolbar(mTableButton);

  auto *mHeatmapButton = new QAction(generateIcon("heat-map"), "");
  mHeatmapButton->setCheckable(true);
  grp->addAction(mHeatmapButton);
  toolbar->addItemToTopToolbar(mHeatmapButton);

  connect(mHeatmapButton, &QAction::toggled, [this](bool checked) {
    if(checked) {
      onShowHeatmap();
    }
  });
  connect(mTableButton, &QAction::toggled, [this](bool checked) {
    if(checked) {
      onShowTable();
    }
  });

  toolbar->addSeparatorToTopToolbar();

  //
  // Copy Button
  //
  auto *copy = new QAction(generateIcon("copy"), "");
  copy->setToolTip("Copy table");
  connect(copy, &QAction::triggered, [this]() { copyTableToClipboard(mTable); });
  toolbar->addItemToTopToolbar(copy);

  //
  // Export button
  //
  auto *exportData = new QAction(generateIcon("excel"), "");
  exportData->setToolTip("Export XLSX");
  connect(exportData, &QAction::triggered, [this]() { onExportClicked(ExportFormat::XLSX); });

  toolbar->addItemToTopToolbar(exportData);

  auto *exportR = new QAction(generateIcon("r-studio"), "");
  exportR->setToolTip("Export R");
  toolbar->addItemToTopToolbar(exportR);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::createNavigationBar(QToolBar *toolbar)
{
  toolbar->setContentsMargins(0, 0, 0, 0);
  toolbar->setStyleSheet("QToolBar { border-bottom: 1px solid rgb(170, 170, 170); }");

  //
  //
  // Back button
  mBackButton = new QPushButton(generateIcon("arrow-left"), "");
  mBackButton->setEnabled(false);

  connect(mBackButton, &QPushButton::clicked, this, &PanelResults::onBackClicked);
  toolbar->addWidget(mBackButton);

  //
  mClusterClassSelector = new QComboBox();
  mClusterClassSelector->setMinimumWidth(150);
  connect(mClusterClassSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onClusterAndClassesChanged);
  toolbar->addWidget(mClusterClassSelector);

  mColumn = new QComboBox();
  mColumn->setMinimumWidth(150);
  connect(mColumn, &QComboBox::currentIndexChanged, this, &PanelResults::onColumnComboChanged);
  toolbar->addWidget(mColumn);

  mMeasurementSelector = new QComboBox();
  mMeasurementSelector->addItem("None", -1);
  mMeasurementSelector->addItem("Count", (int32_t) joda::enums::Measurement::COUNT);
  mMeasurementSelector->addItem("Confidence", (int32_t) joda::enums::Measurement::CONFIDENCE);
  mMeasurementSelector->addItem("Area size", (int32_t) joda::enums::Measurement::AREA_SIZE);
  mMeasurementSelector->addItem("Perimeter", (int32_t) joda::enums::Measurement::PERIMETER);
  mMeasurementSelector->addItem("Circularity", (int32_t) joda::enums::Measurement::CIRCULARITY);
  mMeasurementSelector->addItem("Origin object ID", (int32_t) joda::enums::Measurement::ORIGIN_OBJECT_ID);
  mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
  mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
  mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
  mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);
  connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addWidget(mMeasurementSelector);

  //
  //
  mStatsSelector = new QComboBox();
  mStatsSelector->addItem("AVG", (int32_t) joda::enums::Stats::AVG);
  mStatsSelector->addItem("MEDIAN", (int32_t) joda::enums::Stats::MEDIAN);
  mStatsSelector->addItem("MIN", (int32_t) joda::enums::Stats::MIN);
  mStatsSelector->addItem("MAX", (int32_t) joda::enums::Stats::MAX);
  mStatsSelector->addItem("STDDEV", (int32_t) joda::enums::Stats::STDDEV);
  mStatsSelector->addItem("SUM", (int32_t) joda::enums::Stats::SUM);
  mStatsSelector->addItem("CNT", (int32_t) joda::enums::Stats::CNT);
  connect(mStatsSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addWidget(mStatsSelector);

  toolbar->addSeparator();

  toolbar->addWidget(new QLabel("Intensity: "));

  mCrossChannelStackC = new QComboBox();
  connect(mCrossChannelStackC, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  mActionCrossChannelCStack = toolbar->addWidget(mCrossChannelStackC);
  mActionCrossChannelCStack->setEnabled(false);

  //
  //
  //
  mMarkAsInvalid = new QComboBox();
  mMarkAsInvalid->addItem("Valid", false);
  mMarkAsInvalid->addItem("Invalid", true);
  mMarkAsInvalidAction = toolbar->addWidget(mMarkAsInvalid);
  mMarkAsInvalidAction->setVisible(false);
  connect(mMarkAsInvalid, &QComboBox::currentIndexChanged, this, &PanelResults::onMarkAsInvalidClicked);

  connect(getWindowMain()->getPanelResultsInfo(), &joda::ui::PanelResultsInfo::settingsChanged, this, &PanelResults::onMeasurementChanged);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::setAnalyzer()
{
  {
    // Clusters/Class
    mClusterClassSelector->blockSignals(true);
    auto clusters = mAnalyzer->selectClassesForClusters();
    mClusterClassSelector->clear();
    for(const auto &[clusterId, cluster] : clusters) {
      for(const auto &[classId, classsName] : cluster.second) {
        std::string name = cluster.first + "@" + classsName;
        mClusterClassSelector->addItem(name.data(), SettingComboBoxMultiClassificationIn::toInt(
                                                        {static_cast<enums::ClusterIdIn>(clusterId), static_cast<enums::ClassIdIn>(classId)}));
      }
      mClusterClassSelector->insertSeparator(mClusterClassSelector->count());
    }
    mClusterClassSelector->blockSignals(false);
  }

  {
    // Image channels
    mCrossChannelStackC->blockSignals(true);
    auto imageChannels = mAnalyzer->selectImageChannels();
    mCrossChannelStackC->clear();
    for(const auto &[channelId, channel] : imageChannels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channel.name.data()) + ")", channelId);
    }
    mCrossChannelStackC->blockSignals(false);
  }

  // Analyze meta
  {
    mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  }

  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onMeasurementChanged()
{
  assignFilterSelectionToTableColumn();
  refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onClusterAndClassesChanged()
{
  auto clusterClassSelected = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());

  //
  // Select cross channel intensity
  //
  {
    auto imageChannels  = mAnalyzer->selectImageChannels();
    auto currentChannel = mCrossChannelStackC->currentData().toInt();
    auto channels       = mAnalyzer->selectMeasurementChannelsForClusterAndClass(static_cast<enums::ClusterId>(clusterClassSelected.clusterId),
                                                                                 static_cast<enums::ClassId>(clusterClassSelected.classId));
    mCrossChannelStackC->blockSignals(true);
    mCrossChannelStackC->clear();
    for(const auto channelId : channels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(imageChannels.at(channelId).name.data()) + ")", channelId);
    }
    auto idx = mCrossChannelStackC->findData(currentChannel);
    if(idx >= 0) {
      mCrossChannelStackC->setCurrentIndex(idx);
    }
    mCrossChannelStackC->blockSignals(false);
  }

  auto key = settings::ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(clusterClassSelected.clusterId),
                                               .classId   = static_cast<joda::enums::ClassId>(clusterClassSelected.classId)};

  mSelectedTableColumn = 0;
  mSelectedTableRow    = 0;
  if(!mFilter.clustersToExport.contains(key)) {
    mMeasurementSelector->setCurrentIndex(0);
    mStatsSelector->setCurrentIndex(0);
    mCrossChannelStackC->setCurrentIndex(0);
    mFilter.clustersToExport.emplace(key, db::QueryFilter::Columns{});
  } else {
    onTableCurrentCellChanged(mSelectedTableRow, mSelectedTableColumn, mSelectedTableRow, mSelectedTableColumn);
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
void PanelResults::refreshView()
{
  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

  mFilter.analyzer = mAnalyzer.get(), mFilter.filter = {.plateId         = 0,
                                                        .groupId         = mActGroupId,
                                                        .imageId         = mActImageId,
                                                        .plateRows       = static_cast<uint16_t>(size.height()),
                                                        .plateCols       = static_cast<uint16_t>(size.width()),
                                                        .heatmapAreaSize = mWindowMain->getPanelResultsInfo()->getDensityMapSize(),
                                                        .wellImageOrder  = wellOrder};

  if(mActionCrossChannelCStack != nullptr) {
    if(db::getType(static_cast<joda::enums::Measurement>(mMeasurementSelector->currentData().toInt())) == db::MeasureType::INTENSITY) {
      mActionCrossChannelCStack->setEnabled(true);
    } else {
      mActionCrossChannelCStack->setEnabled(false);
    }
  }
  repaintHeatmap();
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
void PanelResults::onMarkAsInvalidClicked()
{
  if(mMarkAsInvalid->currentData().toBool()) {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->setImageValidity(mSelectedImageId, val);
  } else {
    enums::ChannelValidity val;
    val.set(enums::ChannelValidityEnum::MANUAL_OUT_SORTED);
    mAnalyzer->unsetImageValidity(mSelectedImageId, val);
  }
  onMeasurementChanged();
}

///
/// \brief      An element has been selected
/// \author     Joachim Danmayr
///
void PanelResults::onElementSelected(int cellX, int cellY, table::TableCell value)
{
  switch(mNavigation) {
    case Navigation::PLATE: {
      mSelectedWellId = value.getId();

      //  auto groupId = value.getId();
      //  auto [result, channel] =
      //      mAnalyzer->getGroupInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, groupId);
      //  mSelectedDataSet.groupMeta   = result;
      //  mSelectedDataSet.channelMeta = channel;
      mSelectedDataSet.imageMeta.reset();
      mMarkAsInvalidAction->setVisible(false);
    } break;
    case Navigation::WELL: {
      mSelectedImageId = value.getId();

      auto imageInfo             = mAnalyzer->selectImageInfo(value.getId());
      mSelectedDataSet.imageMeta = imageInfo;
      mMarkAsInvalid->blockSignals(true);

      if(imageInfo.validity.test(enums::ChannelValidityEnum::MANUAL_OUT_SORTED)) {
        mMarkAsInvalid->setCurrentIndex(1);
      } else {
        mMarkAsInvalid->setCurrentIndex(0);
      }
      mMarkAsInvalid->blockSignals(false);
      mMarkAsInvalidAction->setVisible(true);
    }

    break;
    case Navigation::IMAGE:
      mSelectedTileId = value.getId();
      mMarkAsInvalidAction->setVisible(false);
      mSelectedAreaPos.x = cellX;
      mSelectedAreaPos.y = cellY;
      break;
  }

  mSelectedDataSet.value = PanelResultsInfo::DataSet::Value{.value = value.getVal()};
  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
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

  onMeasurementChanged();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::onBackClicked()
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu--;
  if(actMenu >= 0) {
    mNavigation = static_cast<Navigation>(actMenu);
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      mSelectedDataSet.imageMeta.reset();
      break;
    case Navigation::WELL:
      break;
    case Navigation::IMAGE:
      break;
  }

  onMeasurementChanged();
  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::repaintHeatmap()
{
  if(mAnalyzer && !mIsLoading) {
    mIsLoading = true;
    std::thread([this] {
      auto clusterClassSelected = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());
      auto key                  = settings::ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(clusterClassSelected.clusterId),
                                                                    .classId   = static_cast<joda::enums::ClassId>(clusterClassSelected.classId)};

      if(!mFilter.clustersToExport.contains(key)) {
        mIsLoading = false;
        return;
      }
      db::QueryFilter tmpFilterTable;
      tmpFilterTable.analyzer = mFilter.analyzer;
      tmpFilterTable.filter   = mFilter.filter;
      tmpFilterTable.clustersToExport.emplace(key, mFilter.clustersToExport.at(key));

      db::QueryFilter tmpFilterHeatmap;
      tmpFilterHeatmap.analyzer = mFilter.analyzer;
      tmpFilterHeatmap.filter   = mFilter.filter;
      auto measTmp              = static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt());
      auto statsTmp             = static_cast<enums::Stats>(mStatsSelector->currentData().toInt());
      auto cStackTmp            = -1;
      if(mCrossChannelStackC->count() > 0 && db::getType(measTmp) == db::MeasureType::INTENSITY) {
        mCrossChannelStackC->currentData().toInt();
      }

      for(const auto &col : mFilter.clustersToExport.at(key).second) {
        if(col.stats == statsTmp && col.measureChannel == measTmp && (col.crossChannelStacksC == cStackTmp || cStackTmp < 0)) {
          tmpFilterHeatmap.clustersToExport.emplace(key, joda::db::QueryFilter::Columns{mFilter.clustersToExport.at(key).first, {col}});
          break;
        }
      }

      switch(mNavigation) {
        case Navigation::PLATE:
          mBackButton->setEnabled(false);
          {
            auto table = joda::db::StatsPerGroup::toTable(tmpFilterTable, db::StatsPerGroup::Grouping::BY_PLATE);
            if(!table.empty()) {
              tableToQWidgetTable(table[0]);
            }
          }
          {
            auto table = joda::db::StatsPerGroup::toHeatmap(tmpFilterHeatmap, db::StatsPerGroup::Grouping::BY_PLATE);
            if(!table.empty()) {
              tableToHeatmap(table[0]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
        case Navigation::WELL:
          mBackButton->setEnabled(true);
          {
            auto table = joda::db::StatsPerGroup::toTable(tmpFilterTable, db::StatsPerGroup::Grouping::BY_WELL);
            if(!table.empty()) {
              tableToQWidgetTable(table[0]);
            }
          }
          {
            auto table = joda::db::StatsPerGroup::toHeatmap(tmpFilterHeatmap, db::StatsPerGroup::Grouping::BY_WELL);
            if(!table.empty()) {
              tableToHeatmap(table[0]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
        case Navigation::IMAGE:
          mBackButton->setEnabled(true);
          {
            auto table = joda::db::StatsPerImage::toHeatmapList(tmpFilterTable);
            if(!table.empty()) {
              tableToQWidgetTable(table[0]);
            }
          }
          {
            auto table = joda::db::StatsPerImage::toHeatmap(tmpFilterHeatmap);
            if(!table.empty()) {
              tableToHeatmap(table[0]);
            } else {
              paintEmptyHeatmap();
            }
          }
          break;
      }
      update();
      mIsLoading = false;
    }).detach();
  }
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
  mDbFilePath = std::filesystem::path(pathToDbFile.toStdString());
  try {
    mAnalyzer = std::make_unique<joda::db::Database>();
    mAnalyzer->openDatabase(std::filesystem::path(pathToDbFile.toStdString()));
    setAnalyzer();
    if(mSelectedDataSet.analyzeMeta.has_value()) {
      getWindowMain()->getPanelResultsInfo()->addResultsFileToHistory(
          std::filesystem::path(pathToDbFile.toStdString()), mSelectedDataSet.analyzeMeta->jobName, mSelectedDataSet.analyzeMeta->timestampStart);
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
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
void PanelResults::onShowTable()
{
  mTable->setVisible(true);
  mHeatmap01->setVisible(false);
  repaintHeatmap();
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
  mTable->setVisible(false);
  mHeatmap01->setVisible(true);
  repaintHeatmap();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::tableToHeatmap(const joda::table::Table &table)
{
  if(mAnalyzer) {
    if(mSelectedTableColumn >= 0) {
      mHeatmap01->setData(table, mNavigation == Navigation::PLATE ? ChartHeatMap::MatrixForm::CIRCLE : ChartHeatMap::MatrixForm::RECTANGLE,
                          ChartHeatMap::PaintControlImage::NO, static_cast<int32_t>(mNavigation));
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
  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();
  uint16_t rows         = size.height();
  uint16_t cols         = size.width();
  for(int row = 0; row < rows; row++) {
    table.getMutableRowHeader()[row] = "";
    for(int col = 0; col < cols; col++) {
      table.getMutableColHeader()[col] = "";
      table::TableCell data;
      table.setData(row, col, data);
    }
  }
  mHeatmap01->setData(table, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO, static_cast<int32_t>(mNavigation));
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
    mTable->setColumnCount(tableIn.getCols() + 1);
    mTable->setRowCount(tableIn.getRowHeaderSize() + 1);

  } else {
    mTable->setColumnCount(1);
    mTable->setRowCount(1);
  }

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
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
    if(tableIn.getRowHeaderSize() > row) {
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
        if(tableIn.getRowHeaderSize() > row && tableIn.getCols() > col) {
          item->setText(QString::number((double) tableIn.data(row, col).getVal()));
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
auto PanelResults::getClusterAndClassFromCombo() const -> std::pair<std::string, std::string>
{
  QString clusterName;
  QString className;
  className = mClusterClassSelector->currentText();
  if(!className.isEmpty()) {
    auto splited = className.split("@");
    if(splited.size() > 1) {
      clusterName = splited[0];
      className   = splited[1];
    }
  }

  return {clusterName.toStdString(), className.toStdString()};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::addClusterAndClassToFilter()
{
  auto clusterClassSelected = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());
  auto key                  = settings::ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(clusterClassSelected.clusterId),
                                                                .classId   = static_cast<joda::enums::ClassId>(clusterClassSelected.classId)};

  if(!mFilter.clustersToExport.contains(key)) {
    auto [clusterName, className] = getClusterAndClassFromCombo();

    mFilter.clustersToExport.emplace(key, db::QueryFilter::Columns{db::QueryFilter::Naming{.clusterName = clusterName, .className = className},
                                                                   std::vector<db::QueryFilter::Column>(0)});
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
void PanelResults::assignFilterSelectionToTableColumn()
{
  if(mSelectedTableColumn < 0 && mMeasurementSelector->currentData().toInt() >= 0) {
    return;
  }
  auto clusterClassSelected = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());
  auto key                  = settings::ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(clusterClassSelected.clusterId),
                                                                .classId   = static_cast<joda::enums::ClassId>(clusterClassSelected.classId)};
  addClusterAndClassToFilter();
  auto &colIn = mFilter.clustersToExport.at(key).second;

  auto measTmp      = static_cast<enums::Measurement>(mMeasurementSelector->currentData().toInt());
  int32_t cStackTmp = -1;
  if(mCrossChannelStackC->count() > 0) {
    cStackTmp = mCrossChannelStackC->currentData().toInt();
  }

  if(measTmp == enums::Measurement::NONE || (cStackTmp < 0 && db::getType(measTmp) == db::MeasureType::INTENSITY)) {
    if(colIn.size() > mSelectedTableColumn) {
      colIn.erase(colIn.begin() + mSelectedTableColumn);
    }
    return;
  }

  if(mSelectedTableColumn >= colIn.size()) {
    colIn.emplace_back();
  }

  auto &col               = colIn[mSelectedTableColumn];
  col.crossChannelName    = mCrossChannelStackC->currentText().toStdString();
  col.crossChannelStacksC = cStackTmp;

  col.measureChannel = measTmp;
  col.stats          = static_cast<enums::Stats>(mStatsSelector->currentData().toInt());
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
  if(currentColumn < 0) {
    return;
  }

  mTable->blockSignals(true);
  mClusterClassSelector->blockSignals(true);
  mMeasurementSelector->blockSignals(true);
  mStatsSelector->blockSignals(true);
  mCrossChannelStackC->blockSignals(true);
  mColumn->blockSignals(true);

  {
    auto idx = mColumn->findData(currentColumn);
    if(idx >= 0) {
      mColumn->setCurrentIndex(idx);
    }
  }

  auto clusterClassSelected = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());
  auto key                  = settings::ClassificatorSettingOut{.clusterId = static_cast<joda::enums::ClusterId>(clusterClassSelected.clusterId),
                                                                .classId   = static_cast<joda::enums::ClassId>(clusterClassSelected.classId)};

  mSelectedTableColumn = currentColumn;
  mSelectedTableRow    = currentRow;

  mTable->setCurrentCell(currentRow, currentColumn);

  if(!mFilter.clustersToExport.contains(key) || (mFilter.clustersToExport.at(key).second.size() <= currentColumn)) {
    mMeasurementSelector->setCurrentIndex(0);
    mStatsSelector->setCurrentIndex(0);
    mCrossChannelStackC->setCurrentIndex(0);

    mClusterClassSelector->blockSignals(false);
    mMeasurementSelector->blockSignals(false);
    mStatsSelector->blockSignals(false);
    mCrossChannelStackC->blockSignals(false);
    mTable->blockSignals(false);

    return;
  }
  const auto &col = mFilter.clustersToExport.at(key).second[currentColumn];

  auto idx = mMeasurementSelector->findData(static_cast<int32_t>(col.measureChannel));
  if(idx >= 0) {
    mMeasurementSelector->setCurrentIndex(idx);
  } else {
    mMeasurementSelector->setCurrentIndex(0);
  }

  {
    auto idx = mStatsSelector->findData(static_cast<int32_t>(col.stats));
    if(idx >= 0) {
      mStatsSelector->setCurrentIndex(idx);
    } else {
      mStatsSelector->setCurrentIndex(0);
    }
  }

  {
    auto idx = mCrossChannelStackC->findData(col.crossChannelStacksC);
    if(idx >= 0) {
      mCrossChannelStackC->setCurrentIndex(idx);
    } else {
      if(mCrossChannelStackC->count() > 0) {
        mCrossChannelStackC->setCurrentIndex(0);
      }
    }
  }
  mColumn->blockSignals(false);
  mClusterClassSelector->blockSignals(false);
  mMeasurementSelector->blockSignals(false);
  mStatsSelector->blockSignals(false);
  mCrossChannelStackC->blockSignals(false);
  mTable->blockSignals(false);
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
  for(int row = 0; row < table->rowCount(); ++row) {
    QStringList rowData;
    for(int col = 0; col < table->columnCount(); ++col) {
      rowData << table->item(row, col)->text();
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
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
}

///
/// \brief      Export data
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::onExportClicked(ExportFormat format)
{
  QString filePathOfSettingsFile;
  switch(format) {
    case ExportFormat::XLSX:
      filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", mDbFilePath.parent_path().string().data(), "Spreadsheet (*.xlsx)");
      break;
    case ExportFormat::R:
      filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save File", mDbFilePath.parent_path().string().data(), "R-Script (*.r)");
      break;
  }

  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  std::thread([this, filePathOfSettingsFile, format] {
    if(!mTable->isVisible()) {
      std::vector<joda::table::Table> tableDataHeatmap;

      joda::db::BatchExporter::startExportHeatmap(tableDataHeatmap, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                                  mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                                  filePathOfSettingsFile.toStdString());
    } else {
      std::vector<joda::table::Table> tableData;
      joda::db::BatchExporter::startExportList(tableData, mWindowMain->getSettings(), mSelectedDataSet.analyzeMeta->jobName,
                                               mSelectedDataSet.analyzeMeta->timestampStart, mSelectedDataSet.analyzeMeta->timestampFinish,
                                               filePathOfSettingsFile.toStdString());
    }
  }).detach();
}

}    // namespace joda::ui
