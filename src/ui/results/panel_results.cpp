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
/// \brief     A short description what happens here.
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
#include <random>
#include <string>
#include <utility>
#include "backend/results/analyzer/plugins/control_image.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_image.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_well.hpp"
#include "backend/results/db_column_ids.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/container/panel_edit_base.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"
#include "ui/results/dialog_export_data.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelResults::PanelResults(WindowMain *windowMain) : PanelEdit(windowMain), mWindowMain(windowMain)
{
  helper::LayoutGenerator layout(this);
  // Drop downs
  createBreadCrump(&layout);

  // Middle layout
  auto *col  = layout.addVerticalPanel();
  mHeatmap01 = new ChartHeatMap(this);
  mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
  connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
  col->addWidget(mHeatmap01);

  repaintHeatmap();

  /*
  // Create and set up the grid layout
  auto [horizontalLayout, centerWidget] = joda::ui::qt::helper::createLayout(this, helper::SPACING);
  horizontalLayout->setContentsMargins(0, 0, 0, 0);


    mHeatmap01 = new ChartHeatMap(this);
    connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelResults::onElementSelected);
    connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelResults::onOpenNextLevel);
    auto *breadCrump = createBreadCrump(this);
    plateViewer->setContentsMargins(16, 0, 16, 16);
    plateViewer->addWidget(breadCrump);
    breadCrump->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    //
    // Plate
    //
    plateViewer->addWidget(mHeatmap01);
    mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plateViewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  */

  /*
    {
      auto [verticalLayoutContainer, _1] =
          joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 250, 16);

      //
      // Well edit
      //
      {
        auto [verticalLayoutMeta, _2] =
            joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");

        verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Well"));

        mWellName = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mWellName->getEditableWidget());

        mWellValue = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mWellValue->getEditableWidget());

        mWellMeta = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mWellMeta->getEditableWidget());

        _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
      }

      //
      // Image edit
      //
      {
        auto [verticalLayoutMeta, _2] =
            joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
        mImageInfoWidget = _2;
        verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Image"));

        mImageName = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mImageName->getEditableWidget());

        mImageValue = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mImageValue->getEditableWidget());

        mImageMeta = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mImageMeta->getEditableWidget());

        mMarkAsInvalid = std::shared_ptr<ContainerFunction<bool, bool>>(
            new ContainerFunction<bool, bool>("icons8-multiply-50.png", "Mark as invalid", "Mark as invalid", false,
                                              windowMain, "reporting_mark_as_invalid.imcjsproj"));
        verticalLayoutMeta->addWidget(mMarkAsInvalid->getEditableWidget());
        connect(mMarkAsInvalid.get(), &ContainerFunctionBase::valueChanged, this,
    &PanelResults::onMarkAsInvalidClicked);

        _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        mImageInfoWidget->setVisible(false);
      }

      //
      // Area edit
      //
      {
        auto [verticalLayoutMeta, _2] =
            joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
        mAreaInfoWidget = _2;
        verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Area"));

        mAreaName = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mAreaName->getEditableWidget());

        mAreaValue = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mAreaValue->getEditableWidget());

        mAreaMeta = new ContainerLabel("...", "", windowMain);
        verticalLayoutMeta->addWidget(mAreaMeta->getEditableWidget());

        auto saveImageButton = new ContainerButton("Export", "icons8-export-excel-50.png", windowMain);
        connect(saveImageButton, &ContainerButton::valueChanged, this, &PanelResults::onExportImageClicked);
        verticalLayoutMeta->addWidget(saveImageButton->getEditableWidget());

        _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        mAreaInfoWidget->setVisible(false);
      }
      verticalLayoutContainer->addStretch();
    }


  centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setLayout(horizontalLayout);*/
}

void PanelResults::valueChangedEvent()
{
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::createBreadCrump(joda::ui::qt::helper::LayoutGenerator *toolbar)
{
  // Back button
  mBackButton = new QPushButton(QIcon(":/icons/outlined/icons8-left-50.png"), "");
  mBackButton->setEnabled(false);
  connect(mBackButton, &QPushButton::pressed, this, &PanelResults::onBackClicked);
  toolbar->addItemToTopToolbar(mBackButton);

  //
  //
  mChannelSelector = new QComboBox();
  connect(mChannelSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onChannelChanged);
  toolbar->addItemToTopToolbar(mChannelSelector);

  //
  //
  mMeasurementSelector = new QComboBox();

  connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mMeasurementSelector);

  //
  //
  mStatsSelector = new QComboBox();
  mStatsSelector->addItem("AVG", (int32_t) joda::results::Stats::AVG);
  mStatsSelector->addItem("MEDIAN", (int32_t) joda::results::Stats::MEDIAN);
  mStatsSelector->addItem("MIN", (int32_t) joda::results::Stats::MIN);
  mStatsSelector->addItem("MAX", (int32_t) joda::results::Stats::MAX);
  mStatsSelector->addItem("STDDEV", (int32_t) joda::results::Stats::STDDEV);
  mStatsSelector->addItem("SUM", (int32_t) joda::results::Stats::SUM);
  mStatsSelector->addItem("CNT", (int32_t) joda::results::Stats::CNT);
  connect(mStatsSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
  toolbar->addItemToTopToolbar(mStatsSelector);
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
  std::string analysisId;
  {
    // Analysis
    auto analyses = mAnalyzer->getAnalyzes();
    std::vector<ContainerFunction<QString, int>::ComboEntry> entry;
    for(const auto &analyse : analyses) {
      if(analysisId.empty()) {
        analysisId = analyse.analyzeId;
      }
      entry.push_back(ContainerFunction<QString, int>::ComboEntry{
          .key = analyse.analyzeId.data(), .label = analyse.name.data(), .icon = ""});
    }
    if(!analyses.empty()) {
      mAnalyzeId = analysisId;
    } else {
      mAnalyzeId = "";
    }
  }
  {
    // Channels
    mChannelInfos = mAnalyzer->getChannelsForAnalyses(analysisId);
    mChannelSelector->clear();
    for(const auto &channel : mChannelInfos) {
      mChannelSelector->addItem(channel.name.data(), static_cast<uint32_t>(channel.channelId));
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
void PanelResults::onChannelChanged()
{
  for(const auto &channel : mChannelInfos) {
    if(static_cast<uint32_t>(channel.channelId) == mChannelSelector->currentData().toUInt()) {
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

      mMeasurementSelector->setCurrentIndex(lastSelectedKeyIdx);
      connect(mMeasurementSelector, &QComboBox::currentIndexChanged, this, &PanelResults::onMeasurementChanged);
      onMeasurementChanged();
      break;
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
void PanelResults::onMeasurementChanged()
{
  const auto &expSettings = mWindowMain->getExperimentSettings();

  mFilter = PanelResults::SelectedFilter{
      .analyzeId          = mAnalyzeId,
      .plateRows          = expSettings.plateSize.rows,
      .plateCols          = expSettings.plateSize.cols,
      .plateId            = 1,
      .channelIdx         = static_cast<joda::results::ChannelIndex>(mChannelSelector->currentData().toUInt()),
      .measureChannel     = joda::results::MeasureChannelId(mMeasurementSelector->currentData().toUInt()),
      .wellImageOrder     = expSettings.wellImageOrder,
      .stats              = static_cast<joda::results::Stats>(mStatsSelector->currentData().toInt()),
      .densityMapAreaSize = 200};
  setData(mFilter);
}

///
/// \brief      Export image
/// \author     Joachim Danmayr
///
void PanelResults::onExportImageClicked()
{
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
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelResults::onMarkAsInvalidClicked()
{
  if(mMarkAsInvalid->getValue()) {
    mAnalyzer->markImageChannelAsManualInvalid(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx,
                                               mSelectedImageId);
  } else {
    mAnalyzer->unMarkImageChannelAsManualInvalid(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx,
                                                 mSelectedImageId);
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
void PanelResults::onElementSelected(int cellX, int cellY, results::TableCell value)
{
  switch(mNavigation) {
    case Navigation::PLATE: {
      auto groupId = value.getId();
      auto [result, channel] =
          mAnalyzer->getGroupInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, groupId);
      mWellName->setText("Well: " + QString(result.name.data()));
      mWellValue->setText(QString(mFilter.measureChannel.toString().data()) + ": " + QString::number(value.getVal()));
      mWellMeta->setText(channel.name.data());
      mSelectedWellId = value.getId();
      mImageInfoWidget->setVisible(false);
      mAreaInfoWidget->setVisible(false);
    } break;
    case Navigation::WELL: {
      auto [image, channel, imageChannelMeta] =
          mAnalyzer->getImageInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, value.getId());

      mImageName->setText(image.originalImagePath.filename().string().data());
      mImageValue->setText(QString(mFilter.measureChannel.toString().data()) + ": " + QString::number(value.getVal()));
      mImageMeta->setText(channel.name.data());
      mSelectedImageId = value.getId();

      disconnect(mMarkAsInvalid.get(), &ContainerFunctionBase::valueChanged, this,
                 &PanelResults::onMarkAsInvalidClicked);

      if(imageChannelMeta.validity.test(results::ChannelValidityEnum::MANUAL_OUT_SORTED)) {
        mMarkAsInvalid->setValue(true);
      } else {
        mMarkAsInvalid->setValue(false);
      }
      connect(mMarkAsInvalid.get(), &ContainerFunctionBase::valueChanged, this, &PanelResults::onMarkAsInvalidClicked);
      mImageInfoWidget->setVisible(true);
      mAreaInfoWidget->setVisible(false);
    }

    break;
    case Navigation::IMAGE:
      mAreaName->setText("Tile: " + QString::number(value.getId()));
      mAreaValue->setText((std::to_string(cellX) + "x" + std::to_string(cellY)).data());
      mSelectedTileId = value.getId();
      mImageInfoWidget->setVisible(true);
      mAreaInfoWidget->setVisible(true);
      mSelectedAreaPos.x = cellX;
      mSelectedAreaPos.y = cellY;
      break;
  }
}

///
/// \brief      Open the next deeper level form the element with given id
/// \author     Joachim Danmayr
///
void PanelResults::onOpenNextLevel(int cellX, int cellY, results::TableCell value)
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
  repaintHeatmap();
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
    auto result = joda::results::analyze::plugins::HeatmapPerPlate::getData(
        *mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols, mFilter.channelIdx, mFilter.measureChannel,
        mFilter.stats);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  } else {
    joda::results::Table table;

    const auto &expSettings = mWindowMain->getExperimentSettings();
    uint16_t rows           = expSettings.plateSize.rows;
    uint16_t cols           = expSettings.plateSize.cols;
    for(int row = 0; row < rows; row++) {
      table.getMutableRowHeader()[row] = "";
      for(int col = 0; col < cols; col++) {
        table.getMutableColHeader()[col] = "";
        results::TableCell data;
        table.setData(row, col, data);
      }
    }

    std::cout << "Paint " << std::to_string(rows) << std::endl;
    mHeatmap01->setData(mAnalyzer, table, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO,
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
    auto result = joda::results::analyze::plugins::HeatmapForWell::getData(*mAnalyzer, mFilter.plateId, mActGroupId,
                                                                           mFilter.channelIdx, mFilter.measureChannel,
                                                                           mFilter.stats, mFilter.wellImageOrder);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::NO,
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
    auto result = joda::results::analyze::plugins::HeatmapForImage::getData(
        *mAnalyzer, mActImageId, mFilter.channelIdx, mFilter.measureChannel, mFilter.stats, mFilter.densityMapAreaSize);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::YES,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelResults::onExportClicked()
{
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
    const auto &expSettings = mWindowMain->getExperimentSettings();

    uint16_t rows = expSettings.plateSize.rows;
    uint16_t cols = expSettings.plateSize.cols;

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
          .wellImageOrder  = expSettings.wellImageOrder,
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
          .wellImageOrder  = expSettings.wellImageOrder,
          .exportType      = joda::results::exporter::BatchExporter::Settings::ExportType::LIST,
          .exportDetail    = exp};
      joda::results::exporter::BatchExporter::startExport(settings, filePath.toStdString());
    }

    QDesktopServices::openUrl(QUrl("file:///" + filePath));

    // emit exportFinished();
    //  joda::results::exporter::ExporterXlsx::exportAsHeatmap(mHeatmap->getData(), filePath.toStdString());
  }).detach();
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
    mAnalyzer = std::make_shared<joda::results::Analyzer>(std::filesystem::path(pathToDbFile.toStdString()));
    setAnalyzer();
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

}    // namespace joda::ui::qt
