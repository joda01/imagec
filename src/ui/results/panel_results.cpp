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
  //
  //
  auto *exportData = new QPushButton(QIcon(":/icons/outlined/icons8-export-excel-50.png"), "Export");
  exportData->setToolTip("Export data");
  connect(exportData, &QPushButton::pressed, [this]() {
    DialogExportData exportData(mAnalyzer, mFilter, mWindowMain);
    exportData.exec();
  });
  toolbar->addItemToTopToolbar(exportData);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  mClusterClassSelector = new QComboBox();
  connect(mClusterClassSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
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

  mCrossChannelClusterSelector = new QComboBox();
  connect(mCrossChannelClusterSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  mActionCrossChannelCluster = toolbar->addItemToTopToolbar(mCrossChannelClusterSelector);
  mActionCrossChannelCluster->setEnabled(false);

  mCrossChannelClassSelector = new QComboBox();
  connect(mCrossChannelClassSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  mActionCrossChannelClass = toolbar->addItemToTopToolbar(mCrossChannelClassSelector);
  mActionCrossChannelClass->setEnabled(false);

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
    auto clusters = mAnalyzer->selectClassesForClusters();
    mClusterClassSelector->clear();
    for(const auto &[clusterId, cluster] : clusters) {
      mCrossChannelClusterSelector->addItem(cluster.first.data(), static_cast<uint32_t>(clusterId));

      for(const auto &[classId, classsName] : cluster.second) {
        std::string name = cluster.first + "/" + classsName;
        mClusterClassSelector->addItem(name.data(), SettingComboBoxMultiClassificationIn::toInt(
                                                        {static_cast<enums::ClusterIdIn>(clusterId), classId}));
      }
      mClusterClassSelector->insertSeparator(mClusterClassSelector->count());
    }
  }

  {
    // Classes
    auto classes = mAnalyzer->selectClasses();
    mCrossChannelClassSelector->clear();
    for(const auto &[classId, classs] : classes) {
      mCrossChannelClassSelector->addItem(classs.name.data(), static_cast<uint32_t>(classId));
    }
  }

  {
    // Image channels
    auto imageChannels = mAnalyzer->selectImageChannels();
    mCrossChannelStackC->clear();
    for(const auto &[channelId, channel] : imageChannels) {
      mCrossChannelStackC->addItem("CH" + QString::number(channelId) + " (" + QString(channel.name.data()) + ")",
                                   channelId);
    }
  }

  // Analyze meta
  {
    mSelectedDataSet.analyzeMeta = mAnalyzer->selectExperiment();
  }

  getWindowMain()->getPanelResultsInfo()->setData(mSelectedDataSet);
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
  auto clusterClass = SettingComboBoxMultiClassificationIn::fromInt(mClusterClassSelector->currentData().toUInt());

  {
    auto imageChannels  = mAnalyzer->selectImageChannels();
    auto currentChannel = mCrossChannelStackC->currentData().toInt();
    auto channels       = mAnalyzer->selectMeasurementChannelsForClusterAndClass(
        static_cast<enums::ClusterId>(clusterClass.clusterId), clusterClass.classId);
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

  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

  QString className;
  className = mClusterClassSelector->currentText();
  if(!className.isEmpty()) {
    auto splited = className.split("/");
    if(splited.size() > 1) {
      className = splited[1];
    }
  }

  mFilter = db::QueryFilter{
      .analyzer                = mAnalyzer.get(),
      .plateRows               = static_cast<uint16_t>(size.height()),
      .plateCols               = static_cast<uint16_t>(size.width()),
      .plateId                 = 0,
      .actGroupId              = mActGroupId,
      .actImageId              = mActImageId,
      .clusterId               = static_cast<enums::ClusterId>(clusterClass.clusterId),
      .classId                 = clusterClass.classId,
      .className               = className.toStdString(),
      .measurementChannel      = static_cast<joda::enums::Measurement>(mMeasurementSelector->currentData().toInt()),
      .stats                   = static_cast<joda::enums::Stats>(mStatsSelector->currentData().toInt()),
      .wellImageOrder          = wellOrder,
      .densityMapAreaSize      = mWindowMain->getPanelResultsInfo()->getDensityMapSize(),
      .crossChanelStack_c      = static_cast<uint32_t>(mCrossChannelStackC->currentData().toInt()),
      .crossChannelStack_cName = mCrossChannelStackC->currentText().toStdString(),
      .crossChannelClusterId = static_cast<joda::enums::ClusterId>(mCrossChannelClusterSelector->currentData().toInt()),
      .crossChannelClusterName = mCrossChannelClusterSelector->currentText().toStdString(),
      .crossChannelClassId     = static_cast<joda::enums::ClassId>(mCrossChannelClassSelector->currentData().toInt()),
      .crossChannelClassName   = mCrossChannelClassSelector->currentText().toStdString()};

  if(mActionCrossChannelCStack != nullptr && mActionCrossChannelCluster != nullptr &&
     mActionCrossChannelClass != nullptr) {
    if(db::getType(mFilter.measurementChannel) == db::MeasureType::INTENSITY) {
      mActionCrossChannelCStack->setEnabled(true);
      mActionCrossChannelCluster->setEnabled(false);
      mActionCrossChannelClass->setEnabled(false);
    } else if(db::getType(mFilter.measurementChannel) == db::MeasureType::COUNT) {
      mActionCrossChannelCStack->setEnabled(false);
      mActionCrossChannelCluster->setEnabled(true);
      mActionCrossChannelClass->setEnabled(true);
    } else {
      mActionCrossChannelCStack->setEnabled(false);
      mActionCrossChannelCluster->setEnabled(false);
      mActionCrossChannelClass->setEnabled(false);
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
      //  mSelectedDataSet.imageMeta.reset();
      //  mSelectedDataSet.channelMeta.reset();
      //  mSelectedDataSet.imageChannelMeta.reset();
      mMarkAsInvalidAction->setVisible(false);
    } break;
    case Navigation::WELL: {
      mSelectedImageId = value.getId();

      auto imageInfo = mAnalyzer->selectImageInfo(value.getId());

      // mSelectedDataSet.imageMeta        = image;
      // mSelectedDataSet.channelMeta      = channel;
      // mSelectedDataSet.imageChannelMeta = imageChannelMeta;

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
      // mSelectedDataSet.imageMeta.reset();
      // mSelectedDataSet.channelMeta.reset();
      // mSelectedDataSet.imageChannelMeta.reset();
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
  if(!mIsLoading) {
    mIsLoading = true;
    std::thread([this] {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      switch(mNavigation) {
        case Navigation::PLATE:
          paintPlate();
          if(mTableButton != nullptr && mTableButton->isChecked()) {
            tableToQWidgetTable(joda::db::StatsPerPlate::toTable(mFilter));
          }
          break;
        case Navigation::WELL:
          paintWell();
          if(mTableButton != nullptr && mTableButton->isChecked()) {
            tableToQWidgetTable(joda::db::StatsPerGroup::toTable(mFilter));
          }
          break;
        case Navigation::IMAGE:
          paintImage();
          if(mTableButton != nullptr && mTableButton->isChecked()) {
            tableToQWidgetTable(joda::db::StatsPerImage::toTable(mFilter));
          }
          break;
      }
      update();
      QApplication::restoreOverrideCursor();
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
  mTable->setRowCount(table.getRows());
  mTable->setColumnCount(table.getCols());

  for(int n = 0; n < table.getColHeaderSize(); n++) {
    mTable->setHorizontalHeaderItem(n, new QTableWidgetItem(table.getColHeader(n).data()));
  }

  for(int col = 0; col < table.getCols(); col++) {
    for(int row = 0; row < table.getRows(); row++) {
      mTable->setItem(row, col, new QTableWidgetItem(QString::number((double) table.data(row, col).getVal())));
    }
  }
}

}    // namespace joda::ui
