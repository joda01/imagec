///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_image.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <filesystem>
#include <regex>
#include <string>
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

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
    mImages->horizontalHeader()->setVisible(false);
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
    mImageMeta->horizontalHeader()->setVisible(false);
    mImageMeta->setHorizontalHeaderLabels({"Name"});
    mImageMeta->setAlternatingRowColors(false);
    mImageMeta->setSelectionBehavior(QAbstractItemView::SelectRows);
    mImageMeta->setItemDelegate(new HtmlDelegate(mImageMeta));
    mImageMeta->setColumnHidden(1, true);
    layout->addWidget(mImageMeta);
  }

  setLayout(layout);

  windowMain->getController()->registerImageLookupCallback([this](joda::filesystem ::State state) {
    if(state == joda::filesystem::State::FINISHED) {
      mImages->setPlaceholderText("No images found");
      updateImagesList();
    } else if(state == joda::filesystem::State::RUNNING) {
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
auto PanelImages::getSelectedImage() const -> std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo>
{
  int selectedRow = mImages->currentRow();
  int32_t series  = mWindowMain->getPanelProjectSettings()->getImageSeries();

  if(selectedRow >= 0) {
    QTableWidgetItem *item = mImages->item(selectedRow, 0);
    if(item != nullptr) {
      return {std::filesystem::path(item->text().toStdString()), series, mOmeFromActSelectedImage};
    }
  }

  return {std::filesystem::path{}, 0, {}};
}

///
/// \brief  Get first image from list
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelImages::getSelectedImageOrFirst() const -> std::tuple<std::filesystem::path, int32_t, joda::ome::OmeInfo>
{
  auto [path, idx, omeInfo] = getSelectedImage();
  int32_t series            = mWindowMain->getPanelProjectSettings()->getImageSeries();

  if(path.empty()) {
    if(mImages->rowCount() > 0) {
      QTableWidgetItem *item = mImages->item(0, 0);
      if(item != nullptr) {
        auto path    = std::filesystem::path(item->text().toStdString());
        auto omeInfo = mWindowMain->getController()->getImageProperties(path, series);
        return {path, series, omeInfo};
      }
    }
  }

  return {path, series, omeInfo};
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
  mImages->clearContents();
  int row = 0;
  int idx = 0;
  for(const auto &[plateId, images] : foundImages) {
    auto rowCount = row + images.size();
    mImages->setRowCount(rowCount);
    for(const auto &filename : images) {
      if(contains(filename.string().data())) {
        auto *index = new QTableWidgetItem(filename.string().data());
        index->setFlags(index->flags() & ~Qt::ItemIsEditable);
        mImages->setItem(row, 0, index);

        auto *item = new QTableWidgetItem(filename.filename().string().data());
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        mImages->setItem(row, 1, item);
        row++;
      }
      idx++;
    }
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
    auto [imagePath, series, _] = getSelectedImage();

    mOmeFromActSelectedImage = mWindowMain->getController()->getImageProperties(imagePath, series);
    auto tileSize            = mWindowMain->getSettings().imageSetup.imageTileSettings;

    mImageMeta->clearContents();
    mImageMeta->setRowCount(20 + mOmeFromActSelectedImage.getNrOfSeries() * 20);

    int32_t row = 0;

    auto addItem = [this, &row](const std::string name, int64_t value, const std::string &prefix) {
      auto *title = new QTableWidgetItem("<b><i>" + QString(name.data()) + ":</i></b> " + QString::number(value) + " " + QString(prefix.data()));
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);
      row++;
    };

    auto addItemFloat = [this, &row](const std::string name, float value, const std::string &prefix) {
      auto *title = new QTableWidgetItem("<b><i>" + QString(name.data()) + ":</i></b> " + QString::number(value) + " " + QString(prefix.data()));
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);
      row++;
    };

    auto addStringItem = [this, &row](const std::string name, const std::string &value) {
      auto *title = new QTableWidgetItem("<b><i>" + QString(name.data()) + ":</i></b> " + value.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);
      row++;
    };

    auto addTitle = [this, &row](const std::string name) {
      auto *item = new QTableWidgetItem("<b><span style=\"color:#000000;\">" + QString(name.data()) + "</span></b>");
      item->setBackground(QColor("#f7f7f7"));
      mImageMeta->setItem(row, 0, item);
      row++;
    };

    auto addSubTitle = [this, &row](const std::string name) {
      auto *item = new QTableWidgetItem("<b><span style=\"color:#3399ff;\">" + QString(name.data()) + "</span></b>");
      mImageMeta->setItem(row, 0, item);
      row++;
    };

    auto addSubSubTitle = [this, &row](const std::string name) {
      auto *item = new QTableWidgetItem("<b><span style=\"color:#000000;\">" + QString(name.data()) + "</span></b>");
      mImageMeta->setItem(row, 0, item);
      row++;
    };

    addStringItem("Filename", imagePath.filename().string());
    addItem("Nr. series", mOmeFromActSelectedImage.getNrOfSeries(), "");

    for(int32_t series = 0; series < mOmeFromActSelectedImage.getNrOfSeries(); series++) {
      const auto &imgInfo = mOmeFromActSelectedImage.getImageInfo(series);

      addTitle("Image (Series - " + std::to_string(series) + ")");
      addItem("Width", imgInfo.resolutions.at(0).imageWidth, "px");
      addItem("Height", imgInfo.resolutions.at(0).imageHeight, "px");
      addItem("Bits", imgInfo.resolutions.at(0).bits, "");

      addItem("Tile width", imgInfo.resolutions.at(0).optimalTileWidth, "px");
      addItem("Tile height", imgInfo.resolutions.at(0).optimalTileHeight, "px");
      addItem("Tile count", imgInfo.resolutions.at(0).getTileCount(), "");

      // addItem("Composite tile width", tileSize.tileWidth, "px");
      // addItem("Composite tile height", tileSize.tileHeight, "px");
      // addItem("Composite tile count", imgInfo.resolutions.at(0).getTileCount(tileSize.tileWidth, tileSize.tileHeight), "");
      addItem("Pyramids", mOmeFromActSelectedImage.getResolutionCount(series).size(), "");

      QString channelInfoStr;
      for(const auto &[idx, channelInfo] : mOmeFromActSelectedImage.getChannelInfos(series)) {
        mImageMeta->setRowCount(mImageMeta->rowCount() + 5);
        addSubTitle("Channel " + std::to_string(idx));
        addStringItem("Name", channelInfo.name);
        addStringItem("ID", channelInfo.channelId);
        addItemFloat("Emission wave length", channelInfo.emissionWaveLength, channelInfo.emissionWaveLengthUnit);
        addStringItem("Contrast method", channelInfo.contrastMethod);
        addItem("Nr. T-Stacks", channelInfo.planes.size(), "");
        if(!channelInfo.planes.empty()) {
          addItem("Nr. Z-Stacks", channelInfo.planes.at(0).size(), "");
        } else {
          addItem("Nr. Z-Stacks", 0, "");
        }

        for(const auto &[tStack, tdata] : channelInfo.planes) {
          for(const auto &[zStack, zData] : tdata) {
            mImageMeta->setRowCount(mImageMeta->rowCount() + 3);
            addSubSubTitle("Stack Z" + std::to_string(zStack) + "/T" + std::to_string(tStack));
            addItemFloat("Exposure time", zData.exposureTime, zData.exposureTimeUnit);
            // addItem("Z-Stack", zStack, "");
            // addItem("T-Stack", tStack, "");
          }
        }
      }
    }

    //
    // Objective information
    //
    const auto &objectiveInfo = mOmeFromActSelectedImage.getObjectiveInfo();
    addTitle("Objective");
    addStringItem("Manufacturer", objectiveInfo.manufacturer);
    addStringItem("Model", objectiveInfo.model);
    addStringItem("Medium", objectiveInfo.medium);
    addStringItem("Magnification", "x" + std::to_string(objectiveInfo.magnification));

    mImageMeta->setRowCount(row);

  } else {
    mImageMeta->setRowCount(0);
  }
  emit imageSelectionChanged(-1, 0);
}

}    // namespace joda::ui::gui
