///
/// \file      histo_toolbar.hpp
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

#pragma once

#include <qdialog.h>
#include <qscrollbar.h>
#include <qslider.h>
#include <qtmetamacros.h>
#include <qtoolbar.h>
#include <qwindow.h>
#include <tuple>
#include "backend/helper/image/image.hpp"

namespace joda::ui::gui {

class DialogImageViewer;

class HistoToolbar : public QToolBar
{
  Q_OBJECT

public:
  HistoToolbar(int32_t leftOrRight, DialogImageViewer *parent, joda::image::Image *image);
  auto getHistoSettings() const -> std::tuple<float, float, float>;
public slots:
  void autoAdjustHistogram();

private:
  /////////////////////////////////////////////////////
  static constexpr float HISTOGRAM_ZOOM_STEP = 1;

  /////////////////////////////////////////////////////
  DialogImageViewer *mParent;
  joda::image::Image *mImage = nullptr;
  QSlider *mSlider;
  QScrollBar *mSliderScaling;
  QScrollBar *mSliderHistogramOffset;
  void createHistogramDialog();
  QDialog *mHistogramDialog;
  int32_t mLeftOrRight = 0;

private slots:
  /////////////////////////////////////////////////////
  void onZoomHistogramOutClicked();
  void onZoomHistogramInClicked();
  void onSliderMoved(int position);
  void onShowHistogramDialog();
};

}    // namespace joda::ui::gui
