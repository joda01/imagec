///
/// \file      panel_image_view.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>
#include "backend/image_processing/image/image.hpp"

namespace joda::ui::qt {

///
/// \class      PanelImageView
/// \author     Joachim Danmayr
///
class PanelImageView : public QGraphicsView
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  PanelImageView(QWidget *parent = nullptr);
  void setImage(const joda::image::Image &image);
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);
  joda::image::Image &getImage()
  {
    return mActPixmapOriginal;
  }
  void emitUpdateImage()
  {
    emit updateImage();
  }

signals:
  void updateImage();
  void onImageRepainted();

protected:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *) override;
  void leaveEvent(QEvent *) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void drawHistogram(const cv::Mat &image);

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PLACEHOLDER_BASE_SIZE = 450;
  static inline const QString PLACEHOLDER{":/icons/outlined/icons8-picture-1000-lightgray.png"};

  /////////////////////////////////////////////////////
  bool mPlaceholderImageSet = true;
  joda::image::Image mActPixmapOriginal;
  QGraphicsPixmapItem *mActPixmap = nullptr;
  QGraphicsScene *scene;
  bool isDragging = false;
  QPoint lastPos;

private slots:
  void onUpdateImage();
};
}    // namespace joda::ui::qt
