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

#include "panel_image.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/helper/directory_iterator.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

PanelImages::PanelImages(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout();
  // layout->setContentsMargins(0, 0, 0, 0);
  {
    mSearchField = new QLineEdit();
    mSearchField->setPlaceholderText("Search ...");
    layout->addWidget(mSearchField);
    connect(mSearchField, &QLineEdit::editingFinished, this, &PanelImages::filterImages);
  }

  {
    mImages = new PlaceholderTableWidget(0, 2);
    mImages->setPlaceholderText("Select a working directory");
    mImages->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mImages->verticalHeader()->setVisible(false);
    mImages->setHorizontalHeaderLabels({"Idx", "Images"});
    mImages->setAlternatingRowColors(true);
    mImages->setSelectionBehavior(QAbstractItemView::SelectRows);
    mImages->setColumnHidden(0, true);
    mImages->setMaximumHeight(150);

    connect(mImages, &QTableWidget::itemSelectionChanged, [&]() { updateImageMeta(); });

    layout->addWidget(mImages);
  }

  {
    mImageMeta = new PlaceholderTableWidget(0, 2);
    mImageMeta->setPlaceholderText("Select an image");
    mImageMeta->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mImageMeta->verticalHeader()->setVisible(false);
    mImageMeta->setHorizontalHeaderLabels({"Name", "Value"});
    mImageMeta->setAlternatingRowColors(true);
    mImageMeta->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(mImageMeta);
  }

  setLayout(layout);

  windowMain->getController()->registerWorkingDirectoryCallback([this](joda::helper::fs::State state) {
    if(state == joda::helper::fs::State::FINISHED) {
      mImages->setPlaceholderText("No images found");
      updateImagesList();
    } else if(state == joda::helper::fs::State::RUNNING) {
      mImages->setPlaceholderText("Looking for images...");
      mImages->setRowCount(0);
      mImageMeta->setRowCount(0);
    }
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImages::filterImages()
{
  updateImagesList();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImages::getSelectedImage() const -> std::tuple<int32_t, int32_t>
{
  int selectedRow = mImages->currentRow();
  if(selectedRow >= 0) {
    QTableWidgetItem *item = mImages->item(selectedRow, 0);
    if(item != nullptr) {
      return {item->text().toInt(), 0};
    }
  }

  return {-1, 0};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImages::updateImagesList()
{
  mImages->clearSelection();
  std::string searchPattern = mSearchField->text().toLower().toStdString();
  auto contains             = [&searchPattern](const QString &str) {
    if(searchPattern.empty()) {
      return true;
    }
    std::regex regex(".*" + searchPattern + ".*");
    return std::regex_match(str.toLower().toStdString(), regex);
  };

  const auto &foundImages = mWindowMain->getController()->getListOfFoundImages();
  mImages->setRowCount(foundImages.size());
  int row = 0;
  int idx = 0;
  for(const auto &image : foundImages) {
    std::string filename = image.getFilename();
    if(contains(filename.data())) {
      auto *index = new QTableWidgetItem(QString(std::to_string(idx).data()));
      index->setFlags(index->flags() & ~Qt::ItemIsEditable);
      mImages->setItem(row, 0, index);

      auto *item = new QTableWidgetItem(filename.data());
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      mImages->setItem(row, 1, item);
      row++;
    }

    idx++;
  }
  mImages->setRowCount(row);
  mSearchField->setPlaceholderText("Search (" + QString::number(row) + ") ...");
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImages::updateImageMeta()
{
  QList<QTableWidgetItem *> selectedItems = mImages->selectedItems();
  if(!selectedItems.isEmpty()) {
    auto [rowIdx, series] = getSelectedImage();

    auto [ome, imagePath] = mWindowMain->getController()->getImageProperties(rowIdx, series);
    const auto &imgInfo   = ome.getImageInfo();

    mImageMeta->setRowCount(20 + ome.getChannelInfos().size() * 7);

    int32_t row = 0;

    auto addItem = [this, &row](const std::string name, int64_t value, const std::string &prefix) {
      auto *title = new QTableWidgetItem(name.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);

      auto *valueItem = new QTableWidgetItem(QString::number(value) + " " + QString(prefix.data()));
      valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 1, valueItem);
      row++;
    };

    auto addStringItem = [this, &row](const std::string name, const std::string &value) {
      auto *title = new QTableWidgetItem(name.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);

      auto *valueItem = new QTableWidgetItem(value.data());
      valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 1, valueItem);
      row++;
    };

    auto addTitle = [this, &row](const std::string name) {
      auto *item = new QTableWidgetItem(name.data());
      QFont font;
      font.setBold(true);
      item->setFont(font);
      mImageMeta->setItem(row, 0, item);
      mImageMeta->setItem(row, 1, new QTableWidgetItem(""));
      row++;
    };

    addTitle("Image");
    addStringItem("Name", imagePath.filename().string());
    addItem("Width", imgInfo.resolutions.at(0).imageWidth, "px");
    addItem("Height", imgInfo.resolutions.at(0).imageHeight, "px");
    addItem("Bits", imgInfo.resolutions.at(0).bits, "");

    addItem("Tile width", imgInfo.resolutions.at(0).optimalTileWidth, "px");
    addItem("Tile height", imgInfo.resolutions.at(0).optimalTileHeight, "px");
    addItem("Tile count", imgInfo.resolutions.at(0).getTileCount(), "");

    addItem("Composite tile width", joda::pipeline::COMPOSITE_TILE_WIDTH, "px");
    addItem("Composite tile height", joda::pipeline::COMPOSITE_TILE_HEIGHT, "px");
    addItem("Composite tile count",
            imgInfo.resolutions.at(0).getTileCount(joda::pipeline::COMPOSITE_TILE_WIDTH,
                                                   joda::pipeline::COMPOSITE_TILE_HEIGHT),
            "");
    addItem("Series", ome.getNrOfSeries(), "");
    addItem("Pyramids", ome.getResolutionCount().size(), "");

    const auto &objectiveInfo = ome.getObjectiveInfo();
    addTitle("Objective");
    addStringItem("Manufacturer", objectiveInfo.manufacturer);
    addStringItem("Model", objectiveInfo.model);
    addStringItem("Medium", objectiveInfo.medium);
    addStringItem("Magnification", "x" + std::to_string(objectiveInfo.magnification));

    QString channelInfoStr;
    for(const auto &[idx, channelInfo] : ome.getChannelInfos()) {
      int32_t zStack = 1;
      if(!channelInfo.zStackForTimeFrame.empty()) {
        zStack = channelInfo.zStackForTimeFrame.begin()->second.size();
      }
      addTitle("Channel " + std::to_string(idx));
      addStringItem("ID", channelInfo.channelId);
      addStringItem("Name", channelInfo.name);
      addItem("Emission wave length", channelInfo.emissionWaveLength, channelInfo.emissionWaveLengthUnit);
      addStringItem("Contrast method", channelInfo.contrastMethos);
      addItem("Exposure time", channelInfo.exposuerTime, channelInfo.exposuerTimeUnit);
      addItem("Z-Stack", zStack, "");
    }
  } else {
    mImageMeta->setRowCount(0);
  }
  emit imageSelectionChanged(-1, 0);
}

}    // namespace joda::ui::qt