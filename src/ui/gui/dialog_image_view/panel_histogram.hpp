///
/// \file      panel_histogram.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qpainter.h>
#include <qwidget.h>
#include "backend/helper/image/image.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class PanelHistogram : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelHistogram(joda::image::Image *image, QWidget *parent);

private:
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void drawHistogram(QPainter &);

  /////////////////////////////////////////////////////
  joda::image::Image *mImage;
};

}    // namespace joda::ui::gui
