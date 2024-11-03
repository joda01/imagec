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

///

#pragma once

#include <qlabel.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>
#include "backend/helper/image/image.hpp"
#include <opencv2/core/types.hpp>

namespace joda::ui {

///
/// \class      PanelImageView
/// \author     Joachim Danmayr
///
class PanelImageView : public QGraphicsView
{
  Q_OBJECT
public:
  enum State
  {
    MOVE,
    PAINT
  };

  struct ThumbParameter
  {
    int32_t nrOfTilesX          = 0;
    int32_t nrOfTilesY          = 0;
    int32_t tileWidth           = 0;
    int32_t tileHeight          = 0;
    int32_t originalImageWidth  = 0;
    int32_t originalImageHeight = 0;
    int32_t selectedTileX       = 0;
    int32_t selectedTileY       = 0;
  };

  /////////////////////////////////////////////////////
  PanelImageView(const joda::image::Image &imageReference, const joda::image::Image &thumbnailImageReference, QWidget *parent = nullptr);
  void imageUpdated();
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);
  const joda::image::Image &getImage()
  {
    return mActPixmapOriginal;
  }
  void emitUpdateImage()
  {
    emit updateImage();
  }
  void setWaiting(bool waiting)
  {
    mWaiting = waiting;
    update();
    viewport()->update();
  }

  void setState(State);
  void setShowThumbnail(bool);
  void setShowCrosshandCursor(bool);
  void setThumbnailPosition(const ThumbParameter &);
  void setCursorPosition(const QPoint &pos);
  auto getCursorPosition() -> QPoint;

signals:
  void updateImage();
  void onImageRepainted();
  void tileClicked(int32_t tileX, int32_t tileY);

protected:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void drawHistogram();
  void drawThumbnail();

  void getClickedTileInThumbnail(QMouseEvent *event);
  void getThumbnailAreaEntered(QMouseEvent *event);

private:
  /////////////////////////////////////////////////////
  const float THUMB_RECT_START_X       = 10;
  const float THUMB_RECT_START_Y       = 12;
  const float THUMB_RECT_HEIGHT_NORMAL = 128;
  const float THUMB_RECT_WIDTH_NORMAL  = 128;

  const float THUMB_RECT_HEIGHT_ZOOMED = 200;
  const float THUMB_RECT_WIDTH_ZOOMED  = 200;

  /////////////////////////////////////////////////////
  bool mPlaceholderImageSet = true;
  const joda::image::Image &mActPixmapOriginal;
  const joda::image::Image &mThumbnailImageReference;
  QGraphicsPixmapItem *mActPixmap = nullptr;
  QGraphicsScene *scene;
  bool isDragging = false;
  QPoint lastPos;
  QPoint mCursorPos;
  State mState = State::MOVE;
  cv::Size mPixmapSize;

  /////////////////////////////////////////////////////
  ThumbParameter mThumbnailParameter;

  uint32_t mThumbRectWidth      = 0;
  uint32_t mThumbRectHeight     = 0;
  int32_t mTileRectWidthScaled  = 0;
  int32_t mTileRectHeightScaled = 0;

  /////////////////////////////////////////////////////
  bool mThumbnailAreaEntered = false;

  /////////////////////////////////////////////////////
  bool mWaiting             = false;
  bool mShowThumbnail       = true;
  bool mShowCrosshandCursor = false;

private slots:
  void onUpdateImage();
};
}    // namespace joda::ui
