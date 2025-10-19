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
#include "ui/gui/dialogs/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/html_delegate.hpp"

namespace joda::ui::gui {

PanelImages::PanelImages(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  {
    mImages = new PlaceholderTableWidget(0, 2);
    mImages->setFrameStyle(QFrame::NoFrame);
    mImages->setShowGrid(false);
    mImages->setPlaceholderText("Select a working directory");
    mImages->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mImages->verticalHeader()->setVisible(false);
    mImages->horizontalHeader()->setVisible(true);
    mImages->setHorizontalHeaderLabels({"Idx", "Images"});
    mImages->setAlternatingRowColors(true);
    mImages->setSelectionBehavior(QAbstractItemView::SelectRows);
    mImages->setColumnHidden(0, true);
    connect(mImages, &QTableWidget::itemSelectionChanged, [&]() { updateImageMeta(); });
    layout->addWidget(mImages, 1);
  }

  {
    mImageMeta = new PlaceholderTableWidget(0, 2);
    mImageMeta->setFrameStyle(QFrame::NoFrame);
    mImageMeta->setPlaceholderText("Select an image");
    mImageMeta->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mImageMeta->verticalHeader()->setVisible(false);
    mImageMeta->horizontalHeader()->setVisible(true);
    mImageMeta->setHorizontalHeaderLabels({"Name", "Value"});
    mImageMeta->setAlternatingRowColors(true);
    mImageMeta->setSelectionBehavior(QAbstractItemView::SelectRows);
    mImageMeta->setItemDelegate(new HtmlDelegate(mImageMeta));
    layout->addWidget(mImageMeta, 4);
  }

  {
    mSearchField = new QLineEdit();
    mSearchField->setPlaceholderText("Search ...");
    layout->addWidget(mSearchField);
    connect(mSearchField, &QLineEdit::editingFinished, this, &PanelImages::filterImages);
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
  int32_t series  = mWindowMain->getPreviewDock()->getSeries();

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
  int32_t series            = mWindowMain->getPreviewDock()->getSeries();

  if(path.empty()) {
    if(mImages->rowCount() > 0) {
      QTableWidgetItem *item = mImages->item(0, 0);
      if(item != nullptr) {
        auto pathIn                             = std::filesystem::path(item->text().toStdString());
        const auto &defaultPhysicalSizeSettings = mWindowMain->getSettings().imageSetup.imagePixelSizeSettings;
        auto omeInfoIn                          = mWindowMain->getController()->getImageProperties(pathIn, series, defaultPhysicalSizeSettings);
        return {pathIn, series, omeInfoIn};
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
  mImages->setRowCount(static_cast<int>(foundImages.size()));
  for(const auto &filename : foundImages) {
    if(contains(filename.string().data())) {
      auto *index = new QTableWidgetItem(filename.string().data());
      index->setFlags(index->flags() & ~Qt::ItemIsEditable);
      mImages->setItem(row, 0, index);

      auto *item = new QTableWidgetItem(filename.filename().string().data());
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      mImages->setItem(row, 1, item);
      row++;
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
    auto [imagePath, series, _]             = getSelectedImage();
    const auto &defaultPhysicalSizeSettings = mWindowMain->getSettings().imageSetup.imagePixelSizeSettings;
    mOmeFromActSelectedImage                = mWindowMain->getController()->getImageProperties(imagePath, series, defaultPhysicalSizeSettings);
    // Open image
    mWindowMain->openImage(imagePath, &mOmeFromActSelectedImage);
    auto tileSize = mWindowMain->getSettings().imageSetup.imageTileSettings;

    mImageMeta->clearContents();
    mImageMeta->setRowCount(0);

    auto addItem = [this](const std::string &name, int64_t value, const std::string &prefix) {
      auto row = mImageMeta->rowCount();
      mImageMeta->setRowCount(row + 1);
      auto *title = new QTableWidgetItem(name.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);

      auto *valueItem = new QTableWidgetItem(QString::number(value) + " " + QString(prefix.data()));
      valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 1, valueItem);
    };

    auto addItemFloat = [this](const std::string &name, float value, const std::string &prefix) {
      auto row = mImageMeta->rowCount();
      mImageMeta->setRowCount(row + 1);

      auto *title = new QTableWidgetItem(name.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);

      auto *valueItem = new QTableWidgetItem(QString::number(static_cast<double>(value)) + " " + QString(prefix.data()));
      valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 1, valueItem);
    };

    auto addStringItem = [this](const std::string &name, const std::string &value) {
      auto row = mImageMeta->rowCount();
      mImageMeta->setRowCount(row + 1);
      auto *title = new QTableWidgetItem(name.data());
      title->setFlags(title->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 0, title);

      auto *valueItem = new QTableWidgetItem(value.data());
      valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
      mImageMeta->setItem(row, 1, valueItem);
    };

    auto addTitle = [this](const std::string &name) {
      auto row = mImageMeta->rowCount();
      mImageMeta->setRowCount(row + 1);
      auto *item = new QTableWidgetItem("<b>" + QString(name.data()) + "</b>");
      QFont font;
      font.setBold(true);
      item->setFont(font);
      mImageMeta->setItem(row, 0, item);
      mImageMeta->setItem(row, 1, new QTableWidgetItem(""));
    };

    auto addSubTitle = [this](const std::string &name) {
      auto row = mImageMeta->rowCount();
      mImageMeta->setRowCount(row + 1);
      auto *item = new QTableWidgetItem("<i>" + QString(name.data()) + "</i>");
      QFont font;
      font.setItalic(true);
      item->setFont(font);
      mImageMeta->setItem(row, 0, item);
      mImageMeta->setItem(row, 1, new QTableWidgetItem(""));
    };

    addItem("Series", static_cast<int64_t>(mOmeFromActSelectedImage.getNrOfSeries()), "");

    for(int32_t seriesIn = 0; seriesIn < static_cast<int32_t>(mOmeFromActSelectedImage.getNrOfSeries()); seriesIn++) {
      const auto &imgInfo = mOmeFromActSelectedImage.getImageInfo(seriesIn);

      addTitle("Image (Series - " + std::to_string(seriesIn) + ")");
      addStringItem("Name", imagePath.filename().string());
      addItem("Width", imgInfo.resolutions.at(0).imageWidth, "px");
      addItem("Height", imgInfo.resolutions.at(0).imageHeight, "px");
      addItem("Bits", imgInfo.resolutions.at(0).bits, "");

      addItem("Tile width", imgInfo.resolutions.at(0).optimalTileWidth, "px");
      addItem("Tile height", imgInfo.resolutions.at(0).optimalTileHeight, "px");
      addItem("Tile count", imgInfo.resolutions.at(0).getTileCount(), "");

      // addItem("Composite tile width", tileSize.tileWidth, "px");
      // addItem("Composite tile height", tileSize.tileHeight, "px");
      // addItem("Composite tile count", imgInfo.resolutions.at(0).getTileCount(tileSize.tileWidth, tileSize.tileHeight), "");
      addItem("Pyramids", static_cast<int64_t>(mOmeFromActSelectedImage.getResolutionCount(seriesIn).size()), "");

      QString channelInfoStr;
      for(const auto &[idx, channelInfo] : mOmeFromActSelectedImage.getChannelInfos(seriesIn)) {
        addSubTitle("Channel " + std::to_string(idx));
        addStringItem("ID", channelInfo.channelId);
        addStringItem("Name", channelInfo.name);
        addItemFloat("Emission wave length", channelInfo.emissionWaveLength, channelInfo.emissionWaveLengthUnit);
        addStringItem("Contrast method", channelInfo.contrastMethod);

        for(const auto &[tStack, tdata] : channelInfo.planes) {
          for(const auto &[zStack, zData] : tdata) {
            addItemFloat("Exposure time", zData.exposureTime, zData.exposureTimeUnit);
            addItem("Z-Stack", zStack, "");
            addItem("T-Stack", tStack, "");
          }
        }
      }
    }

    const auto &objectiveInfo = mOmeFromActSelectedImage.getObjectiveInfo();
    addStringItem("Objective manufacturer", objectiveInfo.manufacturer);
    addStringItem("Objective model", objectiveInfo.model);
    addStringItem("Objective medium", objectiveInfo.medium);
    addStringItem("Objective magnification", "x" + std::to_string(objectiveInfo.magnification));

  } else {
    mImageMeta->setRowCount(0);
  }
}

}    // namespace joda::ui::gui
