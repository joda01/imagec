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

#include "dialog_channel_settings.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
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
DialogChannelSettings::DialogChannelSettings(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("Channel settings");
  setMinimumSize(300, 400);

  auto *layout = new QFormLayout();

  mHistogramPanel = new PanelHistogram(imagePanel->mutableImage(), this);
  mHistogramPanel->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
  layout->addRow(mHistogramPanel);

  //
  // Add sliders
  //
  mSliderOffset = new QSlider();
  mSliderOffset->setOrientation(Qt::Orientation::Horizontal);
  mSliderOffset->setMinimum(0);
  mSliderOffset->setMaximum(UINT16_MAX);
  mSliderOffset->setValue(imagePanel->mutableImage()->getHistogramOffset());
  layout->addRow("Offset", mSliderOffset);

  mSliderHistogramZoom = new QSlider();
  mSliderHistogramZoom->setOrientation(Qt::Orientation::Horizontal);
  mSliderHistogramZoom->setMinimum(1);
  mSliderHistogramZoom->setMaximum(UINT8_MAX);
  mSliderHistogramZoom->setValue(imagePanel->mutableImage()->getHitogramZoomFactor());
  layout->addRow("Zoom", mSliderHistogramZoom);

  mSliderHistogramMin = new QSlider();
  mSliderHistogramMin->setOrientation(Qt::Orientation::Horizontal);
  mSliderHistogramMin->setMinimum(1);
  mSliderHistogramMin->setMaximum(UINT16_MAX);
  mSliderHistogramMin->setValue(imagePanel->mutableImage()->getLowerLevelContrast());
  layout->addRow("Min", mSliderHistogramMin);

  mSliderHistogramMax = new QSlider();
  mSliderHistogramMax->setOrientation(Qt::Orientation::Horizontal);
  mSliderHistogramMax->setMinimum(1);
  mSliderHistogramMax->setMaximum(UINT16_MAX);
  mSliderHistogramMax->setValue(imagePanel->mutableImage()->getUpperLevelContrast());
  layout->addRow("Max", mSliderHistogramMax);

  connect(mSliderOffset, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramZoom, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramMin, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });
  connect(mSliderHistogramMax, &QScrollBar::valueChanged, [this] { applyHistogramSettingsToImage(); });

  connect(imagePanel, &PanelImageView::updateImage, [this] { mHistogramPanel->update(); });

  setLayout(layout);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogChannelSettings::applyHistogramSettingsToImage()
{
  mImagePanel->mutableImage()->setBrightnessRange(mSliderHistogramMin->value(), mSliderHistogramMax->value(), mSliderHistogramZoom->value(),
                                                  mSliderOffset->value());
  mHistogramPanel->update();
  mImagePanel->repaintImage();
}

}    // namespace joda::ui::gui
