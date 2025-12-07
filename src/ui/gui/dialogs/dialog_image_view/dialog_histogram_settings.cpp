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

#include "dialog_histogram_settings.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbar.h>
#include <cstdint>
#include <string>
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/helper/debugging.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "panel_histogram.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogHistogramSettings::DialogHistogramSettings(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("Histogram");
  setMinimumSize(300, 400);

  auto *layout = new QFormLayout();

  mHistogramPanel = new PanelHistogram(imagePanel, this);
  mHistogramPanel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  layout->addRow(mHistogramPanel);

  //
  // Add sliders
  //
  mSliderHistogramMin = new QSlider();
  mSliderHistogramMin->setOrientation(Qt::Orientation::Horizontal);
  layout->addRow("Min", mSliderHistogramMin);

  mSliderHistogramMax = new QSlider();
  mSliderHistogramMax->setOrientation(Qt::Orientation::Horizontal);
  layout->addRow("Max", mSliderHistogramMax);

  //
  // Axis range
  //
  mSliderDisplayLower = new QSpinBox();
  mSliderDisplayLower->setSingleStep(1);
  mSliderDisplayLower->setMinimum(0);
  mSliderDisplayLower->setMaximum(UINT16_MAX);

  mSliderDisplayUpper = new QSpinBox();
  mSliderDisplayUpper->setSingleStep(1);
  mSliderDisplayUpper->setMinimum(1);
  mSliderDisplayUpper->setMaximum(UINT16_MAX);

  auto *rangeLayout = new QHBoxLayout();
  rangeLayout->addWidget(mSliderDisplayLower);
  rangeLayout->addWidget(mSliderDisplayUpper);
  layout->addRow("Axis range", rangeLayout);

  auto *autoAdjust = new QPushButton("Auto");
  autoAdjust->setAutoDefault(false);
  autoAdjust->setDefault(false);
  connect(autoAdjust, &QPushButton::pressed, [this]() {
    mImagePanel->autoAdjustBrightnessRange();
    getHistogramSettingsFromImage();
    mHistogramPanel->update();
    mImagePanel->repaintImage();
  });
  layout->addRow(autoAdjust);

  // Color mode
  mColorMode = new QComboBox();
  mColorMode->addItem("Grayscale", 0);
  mColorMode->addItem("Pseudo color", 1);

  layout->addRow(mColorMode);

  //
  // Image channel
  //
  mImageChannel = new QComboBox();
  for(int n = 0; n < 9; n++) {
    mImageChannel->addItem("Channel " + QString::number(n), n);
  }
  layout->addRow(mImageChannel);

  // Connect
  connect(mSliderDisplayLower, &QSpinBox::editingFinished, [this] {
    mSliderHistogramMin->blockSignals(true);
    mSliderHistogramMax->blockSignals(true);

    CHECK_GUI_THREAD(mSliderHistogramMin);
    mSliderHistogramMin->setMinimum(mSliderDisplayLower->value());
    mSliderHistogramMax->setMinimum(mSliderDisplayLower->value());

    mImagePanel->setBrightnessRange(mSliderHistogramMin->value(), mSliderHistogramMax->value(), static_cast<int32_t>(mSliderDisplayLower->value()),
                                    static_cast<int32_t>(mSliderDisplayUpper->value()));
    mHistogramPanel->update();
    mImagePanel->repaintImage();

    mSliderHistogramMin->blockSignals(false);
    mSliderHistogramMax->blockSignals(false);
  });

  connect(mSliderDisplayUpper, &QSpinBox::editingFinished, [this] {
    mSliderHistogramMin->blockSignals(true);
    mSliderHistogramMax->blockSignals(true);

    CHECK_GUI_THREAD(mSliderHistogramMin);
    mSliderHistogramMin->setMaximum(mSliderDisplayUpper->value());
    mSliderHistogramMax->setMaximum(mSliderDisplayUpper->value());
    mImagePanel->setBrightnessRange(mSliderHistogramMin->value(), mSliderHistogramMax->value(), static_cast<int32_t>(mSliderDisplayLower->value()),
                                    static_cast<int32_t>(mSliderDisplayUpper->value()));
    mHistogramPanel->update();
    mImagePanel->repaintImage();
    mSliderHistogramMin->blockSignals(false);
    mSliderHistogramMax->blockSignals(false);
  });

  connect(mSliderHistogramMin, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramMax, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mColorMode, &QComboBox::currentIndexChanged, [this] { applyHistogramSettingsToImage(); });

  connect(mImageChannel, &QComboBox::currentIndexChanged, [this] {
    mImagePanel->setImageChannel(mImageChannel->currentData().toInt());
    mImagePanel->reloadImage();
  });

  // This is the problemantic connect
  connect(mImagePanel, &PanelImageView::channelOpened, [this] { getHistogramSettingsFromImage(); });

  setLayout(layout);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistogramSettings::getHistogramSettingsFromImage()
{
  QMetaObject::invokeMethod(
      mSliderDisplayLower,
      [this]() {
        mSliderHistogramMin->blockSignals(true);
        mSliderHistogramMax->blockSignals(true);
        mImageChannel->blockSignals(true);
        mColorMode->blockSignals(true);

        auto *mutableImg = mImagePanel->mutableImage();
        if(mutableImg == nullptr) {
          return;
        }

        const auto lowerArea = mutableImg->getHistogramDisplayAreaLower();
        const auto upperArea = mutableImg->getHistogramDisplayAreaUpper();

        mSliderHistogramMin->setMinimum(lowerArea);
        mSliderHistogramMin->setMaximum(upperArea);

        mSliderHistogramMax->setMinimum(lowerArea);
        mSliderHistogramMax->setMaximum(upperArea);

        mSliderHistogramMin->setValue(mutableImg->getLowerLevelContrast());
        mSliderHistogramMax->setValue(mutableImg->getUpperLevelContrast());

        mImageChannel->setCurrentIndex(mImagePanel->getImagePlane().cStack);

        const bool usePseudoColors = mutableImg->getUsePseudoColors();
        if(usePseudoColors) {
          mColorMode->setCurrentIndex(1);
        } else {
          mColorMode->setCurrentIndex(0);
        }

        mSliderDisplayLower->blockSignals(true);
        mSliderDisplayUpper->blockSignals(true);

        mSliderDisplayLower->setValue(lowerArea);
        mSliderDisplayUpper->setValue(upperArea);

        mSliderDisplayLower->blockSignals(false);
        mSliderDisplayUpper->blockSignals(false);
        mHistogramPanel->update();

        mColorMode->blockSignals(false);
        mImageChannel->blockSignals(false);
        mSliderHistogramMin->blockSignals(false);
        mSliderHistogramMax->blockSignals(false);
      },
      Qt::QueuedConnection);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistogramSettings::applyHistogramSettingsToImage()
{
  blockSignals(true);

  if(mColorMode->currentData() == 0) {
    mImagePanel->setPseudoColorEnabled(false);
  } else {
    mImagePanel->setPseudoColorEnabled(true);
  }

  mImagePanel->setBrightnessRange(mSliderHistogramMin->value(), mSliderHistogramMax->value(), static_cast<int32_t>(mSliderDisplayLower->value()),
                                  static_cast<int32_t>(mSliderDisplayUpper->value()));

  mHistogramPanel->update();
  blockSignals(false);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogHistogramSettings::clearSettings()
{
}

}    // namespace joda::ui::gui
