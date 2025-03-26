///
/// \file      histo_toolbar.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "histo_toolbar.hpp"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include "ui/gui/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

HistoToolbar::HistoToolbar(int32_t leftOrRight, DialogImageViewer *parent, joda::image::Image *image) :
    mLeftOrRight(leftOrRight), mParent(parent), mImage(image)
{
  setMaximumHeight(32);

  mSlider = new QSlider();
  mSlider->setMinimum(1);
  mSlider->setMaximum(UINT16_MAX);
  mSlider->setValue(600);
  mSlider->setOrientation(Qt::Orientation::Horizontal);
  connect(mSlider, &QSlider::valueChanged, this, &HistoToolbar::onSliderMoved);
  addWidget(mSlider);

  QAction *fitToScreen = new QAction(generateIcon("automatic-contrast"), "");
  fitToScreen->setObjectName("ToolButton");
  fitToScreen->setToolTip("Auto adjust");
  connect(fitToScreen, &QAction::triggered, this, &HistoToolbar::autoAdjustHistogram);
  addAction(fitToScreen);

  QAction *action1 = new QAction(generateIcon("normal-distribution-histogram"), "");
  connect(action1, &QAction::triggered, this, &HistoToolbar::onShowHistogramDialog);
  addAction(action1);

  QAction *zoomIn = new QAction(generateIcon("zoom-in"), "");
  zoomIn->setObjectName("ToolButton");
  zoomIn->setToolTip("Zoom in");
  connect(zoomIn, &QAction::triggered, this, &HistoToolbar::onZoomHistogramInClicked);
  addAction(zoomIn);

  QAction *zoomOut = new QAction(generateIcon("zoom-out"), "");
  zoomOut->setObjectName("ToolButton");
  zoomOut->setToolTip("Zoom out");
  connect(zoomOut, &QAction::triggered, this, &HistoToolbar::onZoomHistogramOutClicked);
  addAction(zoomOut);

  createHistogramDialog();

  onSliderMoved(0);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto HistoToolbar::getHistoSettings() const -> std::tuple<float, float, float>
{
  return {mSlider->value(), mSliderScaling->value(), mSliderHistogramOffset->value()};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::createHistogramDialog()
{
  mHistogramDialog = new QDialog(this);
  mHistogramDialog->setWindowTitle("Histogram");
  // mHistogramDialog->setModal(true);

  QVBoxLayout *layoutMain = new QVBoxLayout();
  // Tools
  {
    QHBoxLayout *layout = new QHBoxLayout();
    mSliderScaling      = new QScrollBar(mHistogramDialog);
    mSliderScaling->setMinimum(1);
    mSliderScaling->setMaximum(UINT8_MAX);
    mSliderScaling->setValue(1);
    mSliderScaling->setOrientation(Qt::Orientation::Horizontal);
    mSliderScaling->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mSliderScaling, &QSlider::valueChanged, this, &HistoToolbar::onSliderMoved);

    layout->addWidget(mSliderScaling);
    layoutMain->addWidget(new QLabel("Histogram zoom"));
    layoutMain->addLayout(layout);
  }
  {
    QHBoxLayout *layout    = new QHBoxLayout();
    mSliderHistogramOffset = new QScrollBar(mHistogramDialog);
    mSliderHistogramOffset->setMinimum(0);
    mSliderHistogramOffset->setMaximum(0);
    mSliderHistogramOffset->setValue(0);
    mSliderHistogramOffset->setOrientation(Qt::Orientation::Horizontal);
    mSliderScaling->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mSliderHistogramOffset, &QScrollBar::valueChanged, this, &HistoToolbar::onSliderMoved);

    layout->addWidget(mSliderHistogramOffset);
    layoutMain->addWidget(new QLabel("Histogram position"));
    layoutMain->addLayout(layout);
  }

  mHistogramDialog->setLayout(layoutMain);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::onSliderMoved(int position)
{
  blockSignals(true);
  int number = (float) UINT16_MAX / mSliderScaling->value();
  int max    = UINT16_MAX - number;
  mSliderHistogramOffset->setMaximum(max);
  mSlider->setMinimum(mSliderHistogramOffset->value());
  mSlider->setMaximum(mSliderHistogramOffset->value() + number);
  blockSignals(false);

  mParent->triggerPreviewUpdate(static_cast<DialogImageViewer::ImageView>(mLeftOrRight), true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::onZoomHistogramOutClicked()
{
  auto value = mSliderScaling->value() - HISTOGRAM_ZOOM_STEP;
  mSliderScaling->setValue(value);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::onZoomHistogramInClicked()
{
  auto value = mSliderScaling->value() + HISTOGRAM_ZOOM_STEP;
  mSliderScaling->setValue(value);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::onShowHistogramDialog()
{
  if(!mHistogramDialog->isVisible()) {
    mHistogramDialog->show();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void HistoToolbar::autoAdjustHistogram()
{
  mImage->autoAdjustBrightnessRange();
  mParent->triggerPreviewUpdate(static_cast<DialogImageViewer::ImageView>(mLeftOrRight), false);
}

}    // namespace joda::ui::gui
