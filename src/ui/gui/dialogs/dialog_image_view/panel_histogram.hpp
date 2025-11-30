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

class PanelImageView;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class PanelHistogram : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelHistogram(PanelImageView *image, QWidget *parent);

private:
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void drawHistogram(QPainter &);
  void mouseMoveEvent(QMouseEvent *event) override;

  /////////////////////////////////////////////////////
  PanelImageView *mImagePanel;
  QFont mFont;
};

}    // namespace joda::ui::gui
