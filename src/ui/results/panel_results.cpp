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
#include "backend/helper/database/plugins/control_image.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_plate.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/container/setting/setting_combobox_classification_unmanaged.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/results/dialog_export_data.hpp"
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

  //
  // Create Table
  //
  mTable = new QTableWidget();
  mTable->setVisible(false);

  // Middle layout
  auto *tab  = layout().addTab("", [] {});
  auto *col  = tab->addVerticalPanel();
  mHeatmap01 = new ChartHeatMap(this);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
  connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
  connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });

  col->addWidget(mHeatmap01);
  col->addWidget(mTable);

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
  // Back button
  mBackButton = new QPushButton(QIcon(":/icons/outlined/icons8-left-50.png"), "");
  mBackButton->setEnabled(false);
  connect(mBackButton, &QPushButton::pressed, this, &PanelResults::onBackClicked);
  toolbar->addItemToTopToolbar(mBackButton);

  QButtonGroup *grp = new QButtonGroup();
  mHeatmapButton    = new QPushButton(QIcon(":/icons/outlined/icons8-heat-map-50.png"), "");
  mHeatmapButton->setCheckable(true);
  mHeatmapButton->setChecked(true);
  grp->addButton(mHeatmapButton);
  toolbar->addItemToTopToolbar(mHeatmapButton);

  mTableButton = new QPushButton(QIcon(":/icons/outlined/icons8-table-50.png"), "");
  mTableButton->setCheckable(true);
  grp->addButton(mTableButton);
  toolbar->addItemToTopToolbar(mTableButton);

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

  //
  // Copy Button
  //
  auto *copy = new QPushButton(QIcon(":/icons/outlined/icons8-copy-50.png"), "Copy table");
  copy->setToolTip("Copy table");
  connect(copy, &QPushButton::pressed, [this]() { copyTableToClipboard(mTable); });
  toolbar->addItemToTopToolbar(copy);

  //
  // Export button
  //
  auto *exportData = new QPushButton(QIcon(":/icons/outlined/icons8-export-excel-50.png"), "Export");
  exportData->setToolTip("Export data");
  connect(exportData, &QPushButton::pressed, [this]() {
    std::map<settings::ClassificatorSettingOut, QString> clustersAndClasses;
    for(int i = 0; i < mClusterClassSelector->count(); ++i) {
      clustersAndClasses.emplace(
          SettingComboBoxClassificationUnmanaged::fromInt(mClusterClassSelector->itemData(i).toUInt()),
          mClusterClassSelector->itemText(i));
    }

    DialogExportData exportData(mAnalyzer, mFilter, clustersAndClasses, mWindowMain);
    exportData.exec();
  });
  toolbar->addItemToTopToolbar(exportData);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  mClusterClassSelector = new QComboBox();
  connect(mClusterClassSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onClusterAndClassesChanged);
  toolbar->addItemToTopToolbar(mClusterClassSelector);

  mMeasurementSelector = new QComboBox();
  mMeasurementSelector->addItem("Count", (int32_t) joda::enums::Measurement::COUNT);
  mMeasurementSelector->addItem("Confidence", (int32_t) joda::enums::Measurement::CONFIDENCE);
  mMeasurementSelector->addItem("Area size", (int32_t) joda::enums::Measurement::AREA_SIZE);
  mMeasurementSelector->addItem("Perimeter", (int32_t) joda::enums::Measurement::PERIMETER);
  mMeasurementSelector->addItem("Circularity", (int32_t) joda::enums::Measurement::CIRCULARITY);
  mMeasurementSelector->addItem("Cross channel count", (int32_t) joda::enums::Measurement::INTERSECTING_CNT);
  mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
  mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
  mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
  mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);
  connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mMeasurementSelector);

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
  toolbar->addItemToTopToolbar(mStatsSelector);

  toolbar->addSeparatorToTopToolbar();

  toolbar->addItemToTopToolbar(new QLabel("Cross channel: "));

  mCrossChannelStackC = new QComboBox();
  connect(mCrossChannelStackC, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  mActionCrossChannelCStack = toolbar->addItemToTopToolbar(mCrossChannelStackC);
  mActionCrossChannelCStack->setEnabled(false);

  mCrossChannelClusterAndClassesSelector = new QComboBox();
  connect(mCrossChannelClusterAndClassesSelector, &QComboBox::currentIndexChanged, this,
          &PanelResults::onMeasurementChanged);
  mActionCrossChannelCluster = toolbar->addItemToTopToolbar(mCrossChannelClusterAndClassesSelector);
  mActionCrossChannelCluster->setEnabled(false);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  mMarkAsInvalid = new QComboBox();
  mMarkAsInvalid->addItem("Valid", false);
  mMarkAsInvalid->addItem("Invalid", true);
  mMarkAsInvalidAction = toolbar->addItemToTopToolbar(mMarkAsInvalid);
  mMarkAsInvalidAction->setVisible(false);
  connect(mMarkAsInvalid, &QComboBox::currentIndexChanged, this, &PanelResults::onMarkAsInvalidClicked);

  connect(getWindowMain()->getPanelResultsInfo(), &joda::ui::PanelResultsInfo::settingsChanged, this,
          &PanelResults::onMeasurementChanged);
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
                                                        {static_cast<enums::ClusterIdIn>(clusterId), classId}));
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
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channel.name.data()) + ")",
                                   channelId);
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
  auto clusterClassSelected =
      SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());

  //
  // Select cross channel count
  //

  {
    auto clusters = mAnalyzer->selectCrossChannelCountForClusterAndClass(
        static_cast<enums::ClusterId>(clusterClassSelected.clusterId), clusterClassSelected.classId);
    mCrossChannelClusterAndClassesSelector->blockSignals(true);
    auto currentChannel = mCrossChannelClusterAndClassesSelector->currentData().toUInt();
    mCrossChannelClusterAndClassesSelector->clear();
    for(const auto &[clusterId, cluster] : clusters) {
      for(const auto &[classId, classsName] : cluster.second) {
        std::string name = cluster.first + "@" + classsName;
        mCrossChannelClusterAndClassesSelector->addItem(
            name.data(),
            SettingComboBoxMultiClassificationIn::toInt({static_cast<enums::ClusterIdIn>(clusterId), classId}));
      }
      mCrossChannelClusterAndClassesSelector->insertSeparator(mCrossChannelClusterAndClassesSelector->count());

      auto idx = mCrossChannelClusterAndClassesSelector->findData(currentChannel);
      if(idx >= 0) {
        mCrossChannelClusterAndClassesSelector->setCurrentIndex(idx);
      }

      mCrossChannelClusterAndClassesSelector->blockSignals(false);
    }

    refreshView();
  }

  //
  // Select cross channel intensity
  //
  {
    auto imageChannels  = mAnalyzer->selectImageChannels();
    auto currentChannel = mCrossChannelStackC->currentData().toInt();
    auto channels       = mAnalyzer->selectMeasurementChannelsForClusterAndClass(
        static_cast<enums::ClusterId>(clusterClassSelected.clusterId), clusterClassSelected.classId);
    mCrossChannelStackC->blockSignals(true);
    mCrossChannelStackC->clear();
    for(const auto channelId : channels) {
      mCrossChannelStackC->addItem(
          "CH" + QString::number(channelId) + " (" + QString(imageChannels.at(channelId).name.data()) + ")", channelId);
    }
    auto idx = mCrossChannelStackC->findData(currentChannel);
    if(idx >= 0) {
      mCrossChannelStackC->setCurrentIndex(idx);
    }
    mCrossChannelStackC->blockSignals(false);
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
  auto clusterClassSelected =
      SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());

  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

  QString className;
  className = mClusterClassSelector->currentText();
  if(!className.isEmpty()) {
    auto splited = className.split("@");
    if(splited.size() > 1) {
      className = splited[1];
    }
  }

  auto crossChannelClusterAndClassSelected =
      SettingComboBoxMultiClassificationIn::fromInt(mCrossChannelClusterAndClassesSelector->currentData().toUInt());
  QString crossChannelClusterName;
  QString crossChannelClassName;
  crossChannelClusterName = mCrossChannelClusterAndClassesSelector->currentText();
  if(!crossChannelClusterName.isEmpty()) {
    auto splited = crossChannelClusterName.split("@");
    if(splited.size() > 1) {
      crossChannelClusterName = splited[0];
      crossChannelClassName   = splited[1];
    }
  }

  mFilter = db::QueryFilter{
      .analyzer                = mAnalyzer.get(),
      .plateRows               = static_cast<uint16_t>(size.height()),
      .plateCols               = static_cast<uint16_t>(size.width()),
      .plateId                 = 0,
      .actGroupId              = mActGroupId,
      .actImageId              = mActImageId,
      .clusterId               = static_cast<enums::ClusterId>(clusterClassSelected.clusterId),
      .classId                 = clusterClassSelected.classId,
      .className               = className.toStdString(),
      .measurementChannel      = static_cast<joda::enums::Measurement>(mMeasurementSelector->currentData().toInt()),
      .stats                   = static_cast<joda::enums::Stats>(mStatsSelector->currentData().toInt()),
      .wellImageOrder          = wellOrder,
      .densityMapAreaSize      = mWindowMain->getPanelResultsInfo()->getDensityMapSize(),
      .crossChanelStack_c      = static_cast<uint32_t>(mCrossChannelStackC->currentData().toInt()),
      .crossChannelStack_cName = mCrossChannelStackC->currentText().toStdString(),
      .crossChannelClusterId   = static_cast<enums::ClusterId>(crossChannelClusterAndClassSelected.clusterId),
      .crossChannelClusterName = crossChannelClusterName.toStdString(),
      .crossChannelClassId     = crossChannelClusterAndClassSelected.classId,
      .crossChannelClassName   = crossChannelClassName.toStdString()};

  if(mActionCrossChannelCStack != nullptr && mActionCrossChannelCluster != nullptr) {
    if(db::getType(mFilter.measurementChannel) == db::MeasureType::INTENSITY) {
      mActionCrossChannelCStack->setEnabled(true);
      mActionCrossChannelCluster->setEnabled(false);
    } else if(db::getType(mFilter.measurementChannel) == db::MeasureType::COUNT) {
      mActionCrossChannelCStack->setEnabled(false);
      mActionCrossChannelCluster->setEnabled(true);
    } else {
      mActionCrossChannelCStack->setEnabled(false);
      mActionCrossChannelCluster->setEnabled(false);
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

  mSelectedDataSet.value = {.value = value.getVal()};
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
      switch(mNavigation) {
        case Navigation::PLATE:
          paintPlate();
          tableToQWidgetTable(joda::db::StatsPerPlate::toTable(mFilter));
          break;
        case Navigation::WELL:
          paintWell();
          tableToQWidgetTable(joda::db::StatsPerGroup::toTable(mFilter));
          break;
        case Navigation::IMAGE:
          paintImage();
          tableToQWidgetTable(joda::db::StatsPerImage::toHeatmapList(mFilter));
          break;
      }
      update();
      mIsLoading = false;
    }).detach();
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::paintPlate()
{
  mBackButton->setEnabled(false);
  if(mAnalyzer) {
    mNavigation = Navigation::PLATE;
    auto result = joda::db::StatsPerPlate::toHeatmap(mFilter);
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  } else {
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
    mHeatmap01->setData(table, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::paintWell()
{
  mBackButton->setEnabled(true);
  if(mAnalyzer) {
    mNavigation = Navigation::WELL;
    auto result = joda::db::StatsPerGroup::toHeatmap(mFilter);
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::paintImage()
{
  mBackButton->setEnabled(true);
  if(mAnalyzer) {
    mNavigation = Navigation::IMAGE;
    auto result = joda::db::StatsPerImage::toHeatmap(mFilter);
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::YES,
                        static_cast<int32_t>(mNavigation));
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
  try {
    mAnalyzer = std::make_unique<joda::db::Database>();
    mAnalyzer->openDatabase(std::filesystem::path(pathToDbFile.toStdString()));
    setAnalyzer();
    if(mSelectedDataSet.analyzeMeta.has_value()) {
      getWindowMain()->getPanelResultsInfo()->addResultsFileToHistory(
          std::filesystem::path(pathToDbFile.toStdString()), mSelectedDataSet.analyzeMeta->experiment.experimentName,
          mSelectedDataSet.analyzeMeta->timestamp);
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
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
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResults::tableToQWidgetTable(const table::Table &table)
{
  mTable->setRowCount(table.getRowHeaderSize());
  mTable->setColumnCount(table.getCols());

  for(int n = 0; n < table.getColHeaderSize(); n++) {
    mTable->setHorizontalHeaderItem(n, new QTableWidgetItem(table.getColHeader(n).data()));
  }

  for(int n = 0; n < table.getRowHeaderSize(); n++) {
    mTable->setVerticalHeaderItem(n, new QTableWidgetItem(table.getRowHeader(n).data()));
  }

  for(int col = 0; col < table.getCols(); col++) {
    for(int row = 0; row < table.getRows(); row++) {
      mTable->setItem(row, col, new QTableWidgetItem(QString::number((double) table.data(row, col).getVal())));
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

}    // namespace joda::ui
