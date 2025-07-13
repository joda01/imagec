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
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qtoolbar.h>
#include <qwindow.h>
#include <tuple>
#include "backend/helper/image/image.hpp"

namespace joda::ui::gui {

class DialogImageViewer;
class PanelHistogram;
class PanelImageView;

class DialogChannelSettings : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogChannelSettings(PanelImageView *image, QWidget *parent);

private:
  /////////////////////////////////////////////////////
  void applyHistogramSettingsToImage();
  /////////////////////////////////////////////////////
  PanelImageView *mImagePanel = nullptr;
  PanelHistogram *mHistogramPanel;

  QSpinBox *mSliderDisplayLower;
  QSpinBox *mSliderDisplayUpper;
  QSlider *mSliderHistogramMin;
  QSlider *mSliderHistogramMax;
};

}    // namespace joda::ui::gui
