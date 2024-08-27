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
#include <qcombobox.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
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
#include "backend/helper/database/plugins/heatmap_for_image.hpp"
#include "backend/helper/database/plugins/heatmap_for_plate.hpp"
#include "backend/helper/database/plugins/heatmap_for_well.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
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

  // Middle layout
  auto *tab  = layout().addTab("", [] {});
  auto *col  = tab->addVerticalPanel();
  mHeatmap01 = new ChartHeatMap(this);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
  connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
  connect(layout().getBackButton(), &QAction::triggered, [this] { mWindowMain->showPanelStartPage(); });

  col->addWidget(mHeatmap01);

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

  //
  //
  mClusterSelector = new QComboBox();
  connect(mClusterSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mClusterSelector);

  mClassSelector = new QComboBox();
  connect(mClassSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mClassSelector);

  mMeasurementSelector = new QComboBox();
  mMeasurementSelector->addItem("Count", (int32_t) joda::enums::Measurement::COUNT);
  mMeasurementSelector->addItem("Confidence", (int32_t) joda::enums::Measurement::CONFIDENCE);
  mMeasurementSelector->addItem("Area size", (int32_t) joda::enums::Measurement::AREA_SIZE);
  mMeasurementSelector->addItem("Perimeter", (int32_t) joda::enums::Measurement::PERIMETER);
  mMeasurementSelector->addItem("Circularity", (int32_t) joda::enums::Measurement::CIRCULARITY);
  mMeasurementSelector->addItem("Intensity sum.", (int32_t) joda::enums::Measurement::INTENSITY_SUM);
  mMeasurementSelector->addItem("Intensity avg.", (int32_t) joda::enums::Measurement::INTENSITY_AVG);
  mMeasurementSelector->addItem("Intensity min.", (int32_t) joda::enums::Measurement::INTENSITY_MIN);
  mMeasurementSelector->addItem("Intensity max.", (int32_t) joda::enums::Measurement::INTENSITY_MAX);

  connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mMeasurementSelector);

  mImageChannelSelector = new QComboBox();
  connect(mImageChannelSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mImageChannelSelector);

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

  //
  //
  //
  mMarkAsInvalid = new QComboBox();
  mMarkAsInvalid->addItem("Valid", false);
  mMarkAsInvalid->addItem("Invalid", true);
  mMarkAsInvalidAction = toolbar->addItemToTopToolbar(mMarkAsInvalid);
  mMarkAsInvalidAction->setVisible(false);
  connect(mMarkAsInvalid, &QComboBox::currentIndexChanged, this, &PanelResults::onMarkAsInvalidClicked);

  toolbar->addSeparatorToTopToolbar();

  //
  //
  //
  auto *exportData = new QPushButton(QIcon(":/icons/outlined/icons8-export-excel-50.png"), "Export");
  exportData->setToolTip("Export data");
  connect(exportData, &QPushButton::pressed, this, &PanelResults::onExportClicked);
  toolbar->addItemToTopToolbar(exportData);

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
    // Clusters
    auto clusters = mAnalyzer->selectClusters();
    mClusterSelector->clear();
    for(const auto &[clusterId, cluster] : clusters) {
      mClusterSelector->addItem(cluster.name.data(), static_cast<uint32_t>(clusterId));
    }
  }

  {
    // Classes
    auto classes = mAnalyzer->selectClasses();
    mClassSelector->clear();
    for(const auto &[classId, classs] : classes) {
      mClassSelector->addItem(classs.name.data(), static_cast<uint32_t>(classId));
    }
  }

  {
    // Image channels
    auto imageChannels = mAnalyzer->selectImageChannels();
    mImageChannelSelector->clear();
    for(const auto &[channelId, channel] : imageChannels) {
      mImageChannelSelector->addItem(QString(channel.name.data()) + (" ( CH" + QString::number(channelId) + ")"),
                                     channelId);
    }
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
void PanelResults::onChannelChanged()
{
  /*
  for(const auto &[clusterId, cluster] : mClusters) {
    if(static_cast<uint32_t>(clusterId) == mChannelSelector->currentData().toUInt()) {
      auto lastSelected = mMeasurementSelector->currentData().toUInt();
      disconnect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
      mMeasurementSelector->clear();
      int32_t lastSelectedKeyIdx = -1;
      uint32_t idx               = 0;
      for(const auto &measure : channel.measurements) {
        mMeasurementSelector->addItem(measure.toString().data(), measure.getKey());
        if(lastSelected == measure.getKey()) {
          lastSelectedKeyIdx = idx;
        }
        idx++;
      }
      if(lastSelectedKeyIdx < 0) {
        lastSelectedKeyIdx = 0;
      }

      mMeasurementSelector->setCurrentIndex(lastSelectedKeyIdx);
      connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
      onMeasurementChanged();
      break;
    }
  }*/
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
  const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
  const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

  mFilter = PanelResults::SelectedFilter{
      .plateRows          = static_cast<uint32_t>(size.height()),
      .plateCols          = static_cast<uint32_t>(size.width()),
      .plateId            = 0,
      .clusterId          = static_cast<joda::enums::ClusterId>(mClusterSelector->currentData().toInt()),
      .classId            = static_cast<joda::enums::ClassId>(mClassSelector->currentData().toInt()),
      .measurementChannel = static_cast<joda::enums::Measurement>(mMeasurementSelector->currentData().toInt()),
      .stats              = static_cast<joda::enums::Stats>(mStatsSelector->currentData().toInt()),
      .imageChannel       = static_cast<int32_t>(mImageChannelSelector->currentData().toInt()),
      .wellImageOrder     = wellOrder,
      .densityMapAreaSize = mDensityMapSize};
  setData(mFilter);
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
  repaintHeatmap();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::setData(const SelectedFilter &filter)
{
  mFilter = filter;
  repaintHeatmap();
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
  repaintHeatmap();
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

  repaintHeatmap();
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
          break;
        case Navigation::WELL:
          paintWell();
          break;
        case Navigation::IMAGE:
          paintImage();
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
    auto result = joda::db::HeatmapPerPlate::getData(*mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols,
                                                     mFilter.clusterId, mFilter.classId, mFilter.measurementChannel,
                                                     mFilter.imageChannel, mFilter.stats);
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
    auto result = joda::db::HeatmapForWell::getData(
        *mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols, mFilter.clusterId, mFilter.classId,
        mFilter.measurementChannel, mFilter.imageChannel, mFilter.stats, mActGroupId, mFilter.wellImageOrder);
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
    auto result = joda::db::HeatmapForImage::getData(
        *mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols, mFilter.clusterId, mFilter.classId,
        mFilter.measurementChannel, mFilter.imageChannel, mFilter.stats, mActImageId, mFilter.densityMapAreaSize);
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::YES,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelResults::onExportClicked()
{
  /*
  DialogExportData exportData(mWindowMain);
  auto measureChannelsToExport = exportData.execute();

  if(measureChannelsToExport.ret != 0) {
    return;
  }
  QString filePath = QFileDialog::getSaveFileName(mWindowMain, "Save File", mAnalyzer->getBasePath().string().data(),
                                                  "XLSX Files (*.xlsx)");
  if(filePath.isEmpty()) {
    return;
  }

  std::thread([this, measureChannelsToExport = measureChannelsToExport, filePath = filePath] {
    const auto &size      = mWindowMain->getPanelResultsInfo()->getPlateSize();
    const auto &wellOrder = mWindowMain->getPanelResultsInfo()->getWellOrder();

    uint16_t rows = size.height();
    uint16_t cols = size.width();

    std::map<joda::results::ChannelIndex, joda::results::exporter::BatchExporter::Settings::Channel> imageChannels;

    for(const auto &channel : mChannelInfos) {
      joda::results::exporter::BatchExporter::Settings::Channel channelData;
      channelData.name = channel.name;

      for(const auto &measureChannel : channel.measurements) {
        if(measureChannelsToExport.channelsToExport.contains(measureChannel.getMeasureChannel())) {
          channelData.measureChannels.emplace(
              measureChannel, measureChannelsToExport.channelsToExport.at(measureChannel.getMeasureChannel()));
        }
      }
      imageChannels.emplace(channel.channelId, channelData);
    }

    joda::results::exporter::BatchExporter::Settings::ExportDetail exp;
    switch(getActualNavigation()) {
      case Navigation::PLATE:
        exp = joda::results::exporter::BatchExporter::Settings::ExportDetail::PLATE;
        break;
      case Navigation::WELL:
        exp = joda::results::exporter::BatchExporter::Settings::ExportDetail::WELL;
        break;
      case Navigation::IMAGE:
        exp = joda::results::exporter::BatchExporter::Settings::ExportDetail::IMAGE;
        break;
    }

    if(measureChannelsToExport.exportHeatmap) {
      joda::results::exporter::BatchExporter::Settings settings{
          .imageChannels   = imageChannels,
          .analyzer        = *mAnalyzer,
          .plateId         = 1,
          .groupId         = getSelectedGroup(),
          .imageId         = getSelectedImage(),
          .plateRows       = rows,
          .plarteCols      = cols,
          .heatmapAreaSize = mDenesityMapSize,
          .wellImageOrder  = wellOrder,
          .exportType      = joda::results::exporter::BatchExporter::Settings::ExportType::HEATMAP,
          .exportDetail    = exp};
      joda::results::exporter::BatchExporter::startExport(settings, filePath.toStdString());
    }

    if(measureChannelsToExport.exportList) {
      joda::results::exporter::BatchExporter::Settings settings{
          .imageChannels   = imageChannels,
          .analyzer        = *mAnalyzer,
          .plateId         = 1,
          .groupId         = getSelectedGroup(),
          .imageId         = getSelectedImage(),
          .plateRows       = rows,
          .plarteCols      = cols,
          .heatmapAreaSize = mDenesityMapSize,
          .wellImageOrder  = wellOrder,
          .exportType      = joda::results::exporter::BatchExporter::Settings::ExportType::LIST,
          .exportDetail    = exp};
      joda::results::exporter::BatchExporter::startExport(settings, filePath.toStdString());
    }

    QDesktopServices::openUrl(QUrl("file:///" + filePath));

    // emit exportFinished();
    //  joda::results::exporter::ExporterXlsx::exportAsHeatmap(mHeatmap->getData(), filePath.toStdString());
  }).detach();
  */
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
      getWindowMain()->getPanelResultsInfo()->addResultsFileToHistory(std::filesystem::path(pathToDbFile.toStdString()),
                                                                      mSelectedDataSet.analyzeMeta->name,
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

}    // namespace joda::ui
