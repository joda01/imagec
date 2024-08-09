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
/// \brief     A short description what happens here.
///

#include "panel_results_info.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/helper/directory_iterator.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

PanelResultsInfo::PanelResultsInfo(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout();

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

  setLayout(layout);
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
    addTitle("Group");
    addItem("Id", data.groupMeta->groupId, "");
    addStringItem("name", data.groupMeta->name);
    addItem("Well pos. x", data.groupMeta->wellPosX, "");
    addItem("Well pos. y", data.groupMeta->wellPosY, "");
  }

  if(data.imageMeta.has_value()) {
    addTitle("Image");
    addItem("Id", data.imageMeta->groupId, "");
    addStringItem("Name", data.imageMeta->originalImagePath.filename().string());
    addItem("Width", data.imageMeta->width, "px");
    addItem("Height", data.imageMeta->height, "px");
  }

  if(data.channelMeta.has_value()) {
    addTitle("Channel");
    addStringItem("Id", toString(data.channelMeta->channelId));
    addStringItem("Name", data.channelMeta->name);
  }

  if(data.imageChannelMeta.has_value()) {
    addTitle("Image channel");
    addStringItem("Control image", data.imageChannelMeta->controlImagePath.string());
    addStringItem("Valid", toString(data.imageChannelMeta->validity));
  }
  mResultsProperties->setRowCount(row);
}

}    // namespace joda::ui::qt
