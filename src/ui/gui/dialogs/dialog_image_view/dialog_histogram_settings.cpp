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
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbar.h>
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
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
  mSliderHistogramMin->setMinimum(imagePanel->mutableImage()->getHistogramDisplayAreaLower());
  mSliderHistogramMin->setMaximum(imagePanel->mutableImage()->getHistogramDisplayAreaUpper());
  mSliderHistogramMin->setValue(imagePanel->mutableImage()->getLowerLevelContrast());
  layout->addRow("Min", mSliderHistogramMin);

  mSliderHistogramMax = new QSlider();
  mSliderHistogramMax->setOrientation(Qt::Orientation::Horizontal);
  mSliderHistogramMax->setMinimum(imagePanel->mutableImage()->getHistogramDisplayAreaLower());
  mSliderHistogramMax->setMaximum(imagePanel->mutableImage()->getHistogramDisplayAreaUpper());
  mSliderHistogramMax->setValue(imagePanel->mutableImage()->getUpperLevelContrast());
  layout->addRow("Max", mSliderHistogramMax);

  //
  // Axis range
  //
  mSliderDisplayLower = new QSpinBox();
  mSliderDisplayLower->setMinimum(0);
  mSliderDisplayLower->setMaximum(UINT16_MAX);
  mSliderDisplayLower->setValue(imagePanel->mutableImage()->getHistogramDisplayAreaLower());

  mSliderDisplayUpper = new QSpinBox();
  mSliderDisplayUpper->setMinimum(1);
  mSliderDisplayUpper->setMaximum(UINT16_MAX);
  mSliderDisplayUpper->setValue(imagePanel->mutableImage()->getHistogramDisplayAreaUpper());

  auto *rangeLayout = new QHBoxLayout();
  rangeLayout->addWidget(mSliderDisplayLower);
  rangeLayout->addWidget(mSliderDisplayUpper);
  layout->addRow("Axis range", rangeLayout);

  auto *autoAdjust = new QPushButton("Auto");
  connect(autoAdjust, &QPushButton::pressed, [this]() {
    mImagePanel->mutableImage()->autoAdjustBrightnessRange();
    mHistogramPanel->update();
    mImagePanel->repaintImage();
  });
  layout->addRow(autoAdjust);

  connect(mSliderDisplayLower, &QSpinBox::editingFinished, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderDisplayUpper, &QSpinBox::editingFinished, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramMin, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramMax, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });

  connect(imagePanel, &PanelImageView::updateImage, [this] {
    mSliderDisplayLower->blockSignals(true);
    mSliderDisplayUpper->blockSignals(true);
    mSliderHistogramMin->blockSignals(true);
    mSliderHistogramMax->blockSignals(true);

    mSliderHistogramMin->setMinimum(mImagePanel->mutableImage()->getHistogramDisplayAreaLower());
    mSliderHistogramMin->setMaximum(mImagePanel->mutableImage()->getHistogramDisplayAreaUpper());

    mSliderHistogramMax->setMinimum(mImagePanel->mutableImage()->getHistogramDisplayAreaLower());
    mSliderHistogramMax->setMaximum(mImagePanel->mutableImage()->getHistogramDisplayAreaUpper());

    mSliderDisplayLower->setValue(mImagePanel->mutableImage()->getHistogramDisplayAreaLower());
    mSliderDisplayUpper->setValue(mImagePanel->mutableImage()->getHistogramDisplayAreaUpper());
    mSliderHistogramMin->setValue(mImagePanel->mutableImage()->getLowerLevelContrast());
    mSliderHistogramMax->setValue(mImagePanel->mutableImage()->getUpperLevelContrast());

    mHistogramPanel->update();

    mSliderDisplayLower->blockSignals(false);
    mSliderDisplayUpper->blockSignals(false);
    mSliderHistogramMin->blockSignals(false);
    mSliderHistogramMax->blockSignals(false);
  });

  setLayout(layout);
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
  mImagePanel->mutableImage()->setBrightnessRange(mSliderHistogramMin->value(), mSliderHistogramMax->value(), mSliderDisplayLower->value(),
                                                  mSliderDisplayUpper->value());
  mHistogramPanel->update();
  mImagePanel->repaintImage();
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
