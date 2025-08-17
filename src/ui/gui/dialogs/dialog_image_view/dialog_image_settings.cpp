///
/// \file      histo_toolbar.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "dialog_image_settings.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbar.h>
#include "backend/enums/enums_units.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"
#include "panel_histogram.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogImageSettings::DialogImageSettings(Settings *settings, QWidget *parent, const ome::OmeInfo &omeInfo) :
    QDialog(parent), mSettings(settings), mOmeInfo(omeInfo)
{
  setWindowTitle("Image settings");
  setMinimumWidth(400);
  auto *formLayout = new QFormLayout;

  auto addSeparator = [&formLayout]() {
    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    formLayout->addRow(separator);
  };

  //
  // Z-Projection
  //
  mZprojection = new QComboBox();
  mZprojection->addItem("None", (int32_t) enums::ZProjection::NONE);
  mZprojection->addItem("Max. intensity", (int32_t) enums::ZProjection::MAX_INTENSITY);
  mZprojection->addItem("Min. intensity", (int32_t) enums::ZProjection::MIN_INTENSITY);
  mZprojection->addItem("Avg. intensity", (int32_t) enums::ZProjection::AVG_INTENSITY);
  mZprojection->addItem("Take middle", (int32_t) enums::ZProjection::TAKE_MIDDLE);
  formLayout->addRow("z-projection", mZprojection);

  //
  // Image series
  //
  mSeries = new QComboBox();
  for(int n = 0; n < 10; n++) {
    mSeries->addItem("Series " + QString::number(n), n);
  }
  formLayout->addRow("Series", mSeries);

  //
  // Tile-size
  //
  mTileSize = new QComboBox();
  mTileSize->addItem("8192x8192", 8192);
  mTileSize->addItem("4096x4096", 4096);
  mTileSize->addItem("2048x2048", 2048);
  mTileSize->addItem("1024x1024", 1024);
  mTileSize->addItem("512x512", 512);
  formLayout->addRow("Tile size", mTileSize);
  mTileSize->setCurrentIndex(1);

  addSeparator();

  //
  // Pixel sizes
  //
  mPixelSizeMode = new QComboBox();
  mPixelSizeMode->addItem("Automatic", 0);
  mPixelSizeMode->addItem("Manual", 1);
  formLayout->addRow("Pixel size mode", mPixelSizeMode);

  auto *physicalSizeX = new QHBoxLayout();
  mPixelWidth         = new QLineEdit("0");
  mPixelWidth->setEnabled(false);
  mUnitWidth = new QComboBox();
  mUnitWidth->addItem("Px", (int32_t) enums::Units::Pixels);
  mUnitWidth->addItem("nm", (int32_t) enums::Units::nm);
  mUnitWidth->addItem("µm", (int32_t) enums::Units::um);
  mUnitWidth->addItem("mm", (int32_t) enums::Units::mm);
  mUnitWidth->addItem("cm", (int32_t) enums::Units::cm);
  mUnitWidth->addItem("m", (int32_t) enums::Units::m);
  mUnitWidth->addItem("km", (int32_t) enums::Units::km);
  mUnitWidth->setEnabled(false);
  physicalSizeX->addWidget(mPixelWidth);
  physicalSizeX->addWidget(mUnitWidth);
  formLayout->addRow("Pixel width", physicalSizeX);

  auto *physicalSizeY = new QHBoxLayout();
  mPixelHeight        = new QLineEdit("0");
  mPixelHeight->setEnabled(false);
  mUnitHeight = new QComboBox();
  mUnitHeight->addItem("Px", (int32_t) enums::Units::Pixels);
  mUnitHeight->addItem("nm", (int32_t) enums::Units::nm);
  mUnitHeight->addItem("µm", (int32_t) enums::Units::um);
  mUnitHeight->addItem("mm", (int32_t) enums::Units::mm);
  mUnitHeight->addItem("cm", (int32_t) enums::Units::cm);
  mUnitHeight->addItem("m", (int32_t) enums::Units::m);
  mUnitHeight->addItem("km", (int32_t) enums::Units::km);
  mUnitHeight->setEnabled(false);
  physicalSizeY->addWidget(mPixelHeight);
  physicalSizeY->addWidget(mUnitHeight);
  formLayout->addRow("Pixel height", physicalSizeY);

  connect(mUnitWidth, &QComboBox::currentIndexChanged, [&](int index) { mUnitHeight->setCurrentIndex(index); });
  connect(mPixelSizeMode, &QComboBox::currentIndexChanged, [&](int index) {
    if(mPixelSizeMode->currentData().toInt() == 0) {
      // Automatic
      mUnitWidth->setEnabled(false);
      mPixelWidth->setEnabled(false);
      mPixelHeight->setEnabled(false);
      setFromOme(mOmeInfo);
    } else {
      // Manual
      mUnitWidth->setEnabled(true);
      mPixelWidth->setEnabled(true);
      mPixelHeight->setEnabled(true);
    }
  });

  //
  // Assign data
  //
  fromSettings(omeInfo);

  // Okay and cancel
  auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  formLayout->addWidget(buttonBox);

  setLayout(formLayout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageSettings::setFromOme(const ome::OmeInfo &omeInfo)
{
  const auto &physk = omeInfo.getPhyiscalSize(mSettings->imageSeries, true);
  mPixelWidth->setText(QString::number(physk.sizeX));
  mPixelHeight->setText(QString::number(physk.sizeY));

  auto idx = mUnitWidth->findData((int32_t) physk.unitX);
  if(idx != -1) {
    mUnitWidth->setCurrentIndex(idx);
  }
  idx = mUnitWidth->findData((int32_t) physk.unitY);
  if(idx != -1) {
    mUnitHeight->setCurrentIndex(idx);
  } else {
    mUnitHeight->setCurrentIndex(mUnitWidth->currentIndex());
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageSettings::fromSettings(const ome::OmeInfo &omeInfo)
{
  int idx = mZprojection->findData((int32_t) mSettings->zProjection);
  if(idx != -1) {
    mZprojection->setCurrentIndex(idx);
  }

  idx = mSeries->findData(mSettings->imageSeries);
  if(idx != -1) {
    mSeries->setCurrentIndex(idx);
  }

  idx = mTileSize->findData(mSettings->tileWidth);
  if(idx != -1) {
    mTileSize->setCurrentIndex(idx);
  }

  if(mSettings->unit == enums::Units::Automatic) {
    mPixelSizeMode->setCurrentIndex(0);
    setFromOme(omeInfo);
  } else {
    mPixelSizeMode->setCurrentIndex(1);
    idx = mUnitWidth->findData((int32_t) mSettings->unit);
    if(idx != -1) {
      mUnitWidth->setCurrentIndex(idx);
      mUnitHeight->setCurrentIndex(idx);
    }
    mPixelWidth->setText(QString::number(mSettings->pixelWidth));
    mPixelHeight->setText(QString::number(mSettings->pixelHeight));
  }
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageSettings::accept()
{
  mSettings->zProjection = static_cast<enums::ZProjection>(mZprojection->currentData().toInt());
  mSettings->imageSeries = mSeries->currentData().toInt();
  mSettings->tileWidth   = mTileSize->currentData().toInt();

  if(0 == mPixelSizeMode->currentData().toInt()) {
    // Automatic mode
    mSettings->pixelWidth  = mPixelWidth->text().toDouble();
    mSettings->pixelHeight = mPixelHeight->text().toDouble();
    mSettings->unit        = enums::Units::Automatic;
  } else {
    // Automatic mode
    mSettings->pixelWidth  = mPixelWidth->text().toDouble();
    mSettings->pixelHeight = mPixelHeight->text().toDouble();
    mSettings->unit        = static_cast<enums::Units>(mUnitWidth->currentData().toInt());
  }

  QDialog::accept();
}

}    // namespace joda::ui::gui
