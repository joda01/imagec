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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbar.h>
#include "ui/gui/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/dialog_image_view/panel_histogram.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogImageSettings::DialogImageSettings(Settings *settings, QWidget *parent) : QDialog(parent), mSettings(settings)
{
  setWindowTitle("Image settings");
  setMinimumWidth(500);
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

  //
  // Assign data
  //
  fromSettings();

  // Okay and canlce
  auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
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
void DialogImageSettings::fromSettings()
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
  QDialog::accept();
}

}    // namespace joda::ui::gui
