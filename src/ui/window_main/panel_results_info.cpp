///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_results_info.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/results/panel_results.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

PanelResultsInfo::PanelResultsInfo(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout();

  auto addSeparator = [&layout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };

  // layout->setContentsMargins(0, 0, 0, 0);
  {
      // auto *mSearchField = new QLineEdit();
      // mSearchField->setPlaceholderText("Search ...");
      // layout->addWidget(mSearchField);
      // connect(mSearchField, &QLineEdit::editingFinished, this, &PanelResultsInfo::filterResults);
  }

  {
    mLastLoadedResults = new PlaceholderTableWidget(0, 2);
    mLastLoadedResults->setPlaceholderText("Select a working directory");
    mLastLoadedResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mLastLoadedResults->verticalHeader()->setVisible(false);
    mLastLoadedResults->setHorizontalHeaderLabels({"Path", "Results"});
    mLastLoadedResults->setAlternatingRowColors(true);
    mLastLoadedResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    mLastLoadedResults->setColumnHidden(0, true);
    mLastLoadedResults->setMaximumHeight(150);

    connect(mLastLoadedResults, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
      // Open results
      mWindowMain->showPanelResults();
      mWindowMain->getPanelResults()->openFromFile(mLastLoadedResults->item(row, 0)->text());
    });

    layout->addWidget(mLastLoadedResults);
  }

  {
    mResultsProperties = new PlaceholderTableWidget(0, 2);
    mResultsProperties->setPlaceholderText("Open a results file");
    mResultsProperties->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mResultsProperties->verticalHeader()->setVisible(false);
    mResultsProperties->setHorizontalHeaderLabels({"Name", "Value"});
    mResultsProperties->setAlternatingRowColors(true);
    mResultsProperties->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(mResultsProperties);
  }
  addSeparator();

  {
    QFormLayout *formLayout = new QFormLayout;
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
    mPlateSize->setCurrentIndex(6);

    //
    // Well order matrix
    //
    mWellOrderMatrix = new QLineEdit("[[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]");

    //
    // Densitymap size
    //
    mDensityMapSize = new QComboBox();
    mDensityMapSize->addItem("50", 50);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 64).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 64);
    mDensityMapSize->addItem("100", 100);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 32).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 32);
    mDensityMapSize->addItem("150", 150);
    mDensityMapSize->addItem("200", 200);
    mDensityMapSize->addItem("250", 250);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 16).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 16);
    mDensityMapSize->addItem("300", 300);
    mDensityMapSize->addItem("350", 350);
    mDensityMapSize->addItem("400", 400);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem("450", 450);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 8).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 8);
    mDensityMapSize->addItem("1000", 1000);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 4).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 4);
    mDensityMapSize->addItem("2000", 2000);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 2).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH / 2);
    mDensityMapSize->addItem("3000", 3000);
    mDensityMapSize->addItem(std::to_string(joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH).data(),
                             joda::processor::PipelineInitializer::COMPOSITE_TILE_WIDTH);
    mDensityMapSize->setCurrentIndex(13);

    formLayout->addRow(new QLabel(tr("Well order:")), mWellOrderMatrix);
    formLayout->addRow(new QLabel(tr("Plate size:")), mPlateSize);
    formLayout->addRow(new QLabel(tr("Density map size:")), mDensityMapSize);

    layout->addLayout(formLayout);

    connect(mPlateSize, &QComboBox::currentIndexChanged, this, &PanelResultsInfo::settingsChanged);
    connect(mDensityMapSize, &QComboBox::currentIndexChanged, this, &PanelResultsInfo::settingsChanged);
    connect(mWellOrderMatrix, &QLineEdit::editingFinished, this, &PanelResultsInfo::settingsChanged);
  }

  setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::filterResults()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::clearHistory()
{
  mLastLoadedResults->setRowCount(0);
  mAddedPaths.clear();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::addResultsFileToHistory(const std::filesystem::path &dbFile, const std::string &jobName,
                                               const std::chrono::system_clock::time_point &time)
{
  if(mAddedPaths.contains(dbFile.string())) {
    return;
  }
  mAddedPaths.emplace(dbFile.string());
  mLastLoadedResults->insertRow(0);
  auto *index = new QTableWidgetItem(dbFile.string().data());
  index->setFlags(index->flags() & ~Qt::ItemIsEditable);
  mLastLoadedResults->setItem(0, 0, index);

  auto *item = new QTableWidgetItem(QString((jobName + " (" + joda::helper::timepointToIsoString(time) + ")").data()));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  mLastLoadedResults->setItem(0, 1, item);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelResultsInfo::getWellOrder() const -> std::vector<std::vector<int32_t>>
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
[[nodiscard]] auto PanelResultsInfo::getPlateSize() const -> QSize
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
[[nodiscard]] auto PanelResultsInfo::getDensityMapSize() const -> uint32_t
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
void PanelResultsInfo::setData(const DataSet &data)
{
  mResultsProperties->setRowCount(50);

  int32_t row  = 0;
  auto addItem = [this, &row](const std::string name, int64_t value, const std::string &prefix) {
    auto *title = new QTableWidgetItem(name.data());
    title->setFlags(title->flags() & ~Qt::ItemIsEditable);
    mResultsProperties->setItem(row, 0, title);

    auto *valueItem = new QTableWidgetItem(QString::number(value) + " " + QString(prefix.data()));
    valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    mResultsProperties->setItem(row, 1, valueItem);
    row++;
  };

  auto addStringItem = [this, &row](const std::string name, const std::string &value) {
    auto *title = new QTableWidgetItem(name.data());
    title->setFlags(title->flags() & ~Qt::ItemIsEditable);
    mResultsProperties->setItem(row, 0, title);

    auto *valueItem = new QTableWidgetItem(value.data());
    valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
    mResultsProperties->setItem(row, 1, valueItem);
    row++;
  };

  auto addTitle = [this, &row](const std::string name) {
    auto *item = new QTableWidgetItem(name.data());
    QFont font;
    font.setBold(true);
    item->setFont(font);
    mResultsProperties->setItem(row, 0, item);
    mResultsProperties->setItem(row, 1, new QTableWidgetItem(""));
    row++;
  };
  /*
    if(data.analyzeMeta.has_value()) {
      addTitle("Experiment");
      addStringItem("Name", data.analyzeMeta->name);
      addStringItem("Scientist", data.analyzeMeta->scientists[0]);
      addStringItem("Organization", data.analyzeMeta->addressOrganization);
      addStringItem("Notes", data.analyzeMeta->notes);
    }

    if(data.plateMeta.has_value()) {
      addTitle("Plate");
      addItem("Id", data.plateMeta->plateId, "");
      addStringItem("Notes", data.plateMeta->notes);
    }

    if(data.groupMeta.has_value()) {
      addTitle("Group/Well");
      addItem("Id", data.groupMeta->groupId, "");
      addStringItem("Name", data.groupMeta->name);
      addItem("Well pos. x", data.groupMeta->wellPosX, "");
      addItem("Well pos. y", data.groupMeta->wellPosY, "");
    }

    if(data.channelMeta.has_value()) {
      addTitle("Channel");
      addStringItem("Id", toString(data.channelMeta->channelId));
      addStringItem("Name", data.channelMeta->name);
    }

    if(data.imageMeta.has_value()) {
      addTitle("Image");
      addItem("Id", data.imageMeta->groupId, "");
      addStringItem("Name", data.imageMeta->originalImagePath.filename().string());
      addItem("Width", data.imageMeta->width, "px");
      addItem("Height", data.imageMeta->height, "px");
    }
    if(data.imageChannelMeta.has_value()) {
      addStringItem("Control image", data.imageChannelMeta->controlImagePath.filename().string());
      addStringItem("Valid", toString(data.imageChannelMeta->validity));
    }
  */
  mResultsProperties->setRowCount(row);
}

}    // namespace joda::ui
