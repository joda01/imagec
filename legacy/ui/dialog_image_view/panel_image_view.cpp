///
/// \file      panel_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_image_view.hpp"
#include <qpixmap.h>
#include <qtmetamacros.h>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <string>
#include "backend/image_processing/image/image.hpp"
#include <opencv2/imgproc.hpp>

namespace joda::ui::qt {

////////////////////////////////////////////////////////////////
// Image view section
//
PanelImageView::PanelImageView(const joda::image::Image &imageReference,
                               const joda::image::Image &thumbnailImageReference, QWidget *parent) :
    QGraphicsView(parent),
    mActPixmapOriginal(imageReference), mThumbnailImageReference(thumbnailImageReference),
    scene(new QGraphicsScene(this))
{
  setScene(scene);
  setBackgroundBrush(QBrush(Qt::black));
  scene->setBackgroundBrush(QBrush(Qt::black));

  // Set up the view
  setRenderHint(QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMouseTracking(true);

  setFrameShape(Shape::NoFrame);

  connect(this, &PanelImageView::updateImage, this, &PanelImageView::onUpdateImage);
}

void PanelImageView::setState(State state)
{
  mState = state;
  switch(mState) {
    case MOVE:
      viewport()->setCursor(QCursor(Qt::OpenHandCursor));
      break;
    case PAINT:
      viewport()->setCursor(QCursor(Qt::CrossCursor));
      break;
  }

  emit updateImage();
}

void PanelImageView::imageUpdated()
{
  emit updateImage();
}

void PanelImageView::resetImage()
{
  mPlaceholderImageSet = true;
  delete mActPixmap;
  mActPixmap = nullptr;
  fitImageToScreenSize();
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::onUpdateImage()
{
  auto *img = mActPixmapOriginal.getImage();
  if(img != nullptr) {
    const auto pixmap = mActPixmapOriginal.getPixmap();
    scene->setSceneRect(pixmap.rect());
    if(nullptr == mActPixmap) {
      mActPixmap = scene->addPixmap(pixmap);
    } else {
      mActPixmap->setPixmap(pixmap);
    }
    auto size = img->size();
    if((size.width != mPixmapSize.width) || (size.height != mPixmapSize.height) || mPlaceholderImageSet) {
      mPixmapSize = size;
      fitImageToScreenSize();
      mPlaceholderImageSet = false;
    } else {
      emit onImageRepainted();
    }
  }
  scene->update();
  update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::mouseMoveEvent(QMouseEvent *event)
{
  if(isDragging) {
    // Calculate the difference in mouse position
    QPoint delta = event->pos() - lastPos;

    // Scroll the view
    verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());

    // Update the last position
    lastPos = event->pos();
    emit onImageRepainted();
  }
  if(mShowThumbnail) {
    getThumbnailAreaEntered(event);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // End dragging
    if(cursor() != Qt::OpenHandCursor) {
      setCursor(Qt::OpenHandCursor);
      viewport()->setCursor(Qt::OpenHandCursor);
    }

    isDragging = false;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // Start dragging
    if(cursor() != Qt::ClosedHandCursor) {
      setCursor(Qt::ClosedHandCursor);
      viewport()->setCursor(Qt::ClosedHandCursor);
    }

    isDragging = true;
    lastPos    = event->pos();
    if(mShowThumbnail) {
      getClickedTileInThumbnail(event);
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::wheelEvent(QWheelEvent *event)
{
  zoomImage(event->angleDelta().y() > 0);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::zoomImage(bool inOut)
{
  qreal zoomFactor = 1.05;
  if(inOut) {
    scale(zoomFactor, zoomFactor);
  } else {
    scale(1.0 / zoomFactor, 1.0 / zoomFactor);
  }
  emit onImageRepainted();

  /*
  QPointF center = mapToScene(viewport()->rect().center());
  centerOn(center);
  */
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::fitImageToScreenSize()
{
  resetTransform();
  float zoomFactor = static_cast<float>(width()) / static_cast<float>(mPixmapSize.width);
  scale(zoomFactor, zoomFactor);
  emit onImageRepainted();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::paintEvent(QPaintEvent *event)
{
  QGraphicsView::paintEvent(event);

  const float RECT_SIZE  = 80;
  const int RECT_START_X = 10;
  const int RECT_START_Y = 10;

  // Get the viewport rectangle
  QRect viewportRect = viewport()->rect();
  if(mActPixmap == nullptr) {
    QPainter painter(viewport());
    painter.setPen(QColor(0, 0, 0));      // Set the pen color to light blue
    painter.setBrush(QColor(0, 0, 0));    // Set the brush to no brush for transparent fill
    painter.drawRect(viewportRect);
  }

  QRect rectangle(RECT_START_X, RECT_START_Y, RECT_SIZE, RECT_SIZE);    // Adjust the size as needed

  float zoomFactor     = viewportTransform().m11();
  float actImageWith   = RECT_SIZE * width() / (zoomFactor * scene->width());
  float actImageHeight = RECT_SIZE * height() / (zoomFactor * scene->width());

  float posX =
      (RECT_SIZE - actImageWith) * (float) horizontalScrollBar()->value() / (float) horizontalScrollBar()->maximum();
  float posY =
      (RECT_SIZE - actImageHeight) * (float) verticalScrollBar()->value() / (float) verticalScrollBar()->maximum();

  QRect viewPort(RECT_START_X + posX, RECT_START_Y + posY, actImageWith, actImageHeight);

  // Draw the rectangle
  QPainter painter(viewport());
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill

  // Draw
  if(viewPort.width() < RECT_SIZE) {
    painter.drawRect(rectangle);
    painter.drawRect(viewPort);
  }

  // Draw histogram
  drawHistogram();
  if(mShowThumbnail) {
    drawThumbnail();
  }

  // Overlay
  if(mWaiting) {
    QPainter painter(viewport());
    QRect overlay(0, viewportRect.height() / 2 - 10, viewportRect.width(), 20);
    painter.setPen(QColor(0, 0, 0));      // Set the pen color to light blue
    painter.setBrush(QColor(0, 0, 0));    // Set the brush to no brush for transparent fill
    painter.drawRect(overlay);
    painter.setPen(QColor(255, 255, 255));      // Set the pen color to light blue
    painter.setBrush(QColor(255, 255, 255));    // Set the brush to no brush for transparent fill
    painter.drawText(overlay, Qt::AlignHCenter | Qt::AlignVCenter, "Generating preview ...");
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::drawThumbnail()
{
  if(mNrOfTilesX <= 1 && mNrOfTilesY <= 1) {
    return;
  }
  float rectHeight = THUMB_RECT_HEIGHT_NORMAL;
  float rectWidth  = THUMB_RECT_WIDTH_NORMAL;
  if(mThumbnailAreaEntered) {
    rectHeight = THUMB_RECT_HEIGHT_ZOOMED;
    rectWidth  = THUMB_RECT_WIDTH_ZOOMED;
  }

  QPainter painter(viewport());
  auto *img = mThumbnailImageReference.getImage();
  if(img == nullptr) {
    return;
  }
  auto thumbnailWidth  = img->cols;
  auto thumbnailHeight = img->rows;

  //
  // Scale thumbnail
  //
  int newWidth, newHeight;
  // Check if width or height is the limiting factor
  float aspectRatio = static_cast<float>(thumbnailWidth) / static_cast<float>(thumbnailHeight);
  if(rectWidth / aspectRatio <= rectHeight) {
    // Width is the limiting factor
    newWidth  = rectWidth;
    newHeight = static_cast<int>(rectWidth / aspectRatio);
  } else {
    // Height is the limiting factor
    newHeight = rectHeight;
    newWidth  = static_cast<int>(rectHeight * aspectRatio);
  }
  QRect thumbRect(QPoint(width() - THUMB_RECT_START_X - rectWidth, THUMB_RECT_START_Y),
                  QSize(newWidth,
                        newHeight));    // Adjust the size as needed
  painter.drawPixmap(thumbRect, mThumbnailImageReference.getPixmap());

  //
  // Draw bounding rext
  //
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill
  QRect rectangle(QPoint(width() - THUMB_RECT_START_X - rectWidth, THUMB_RECT_START_Y),
                  QSize(rectWidth,
                        rectHeight));    // Adjust the size as needed
  painter.drawRect(rectangle);

  //
  // Draw grid
  //
  float tileRectWidth  = rectWidth / mNrOfTilesX;
  float tileRectHeight = rectHeight / mNrOfTilesY;
  for(int y = 0; y < mNrOfTilesY; y++) {
    for(int x = 0; x < mNrOfTilesX; x++) {
      bool isSelected = false;
      if(x == mSelectedTileX && y == mSelectedTileY) {
        painter.setBrush(QColor(173, 216, 230));    // Set the brush to no brush for transparent fill
        isSelected = true;
      } else {
        if(!mThumbnailAreaEntered) {
          painter.setBrush(Qt::NoBrush);
        } else {
          painter.setBrush(Qt::NoBrush);
        }
      }
      if(mThumbnailAreaEntered || isSelected) {
        float xOffset = std::floor(static_cast<float>(x) * tileRectWidth);
        float yOffset = std::floor(static_cast<float>(y) * tileRectHeight);
        QRect rectangle(QPoint(width() - THUMB_RECT_START_X - rectWidth + xOffset, THUMB_RECT_START_Y + yOffset),
                        QSize(tileRectWidth, tileRectHeight));
        painter.drawRect(rectangle);
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::getClickedTileInThumbnail(QMouseEvent *event)
{
  float rectHeight = THUMB_RECT_HEIGHT_NORMAL;
  float rectWidth  = THUMB_RECT_WIDTH_NORMAL;
  if(mThumbnailAreaEntered) {
    rectHeight = THUMB_RECT_HEIGHT_ZOOMED;
    rectWidth  = THUMB_RECT_WIDTH_ZOOMED;
  }

  float tileRectWidth  = rectWidth / mNrOfTilesX;
  float tileRectHeight = rectHeight / mNrOfTilesY;
  for(int y = 0; y < mNrOfTilesY; y++) {
    for(int x = 0; x < mNrOfTilesX; x++) {
      int xOffset = x * tileRectWidth;
      int yOffset = y * tileRectHeight;

      QRect rectangle(QPoint(width() - THUMB_RECT_START_X - rectWidth + xOffset, THUMB_RECT_START_Y + yOffset),
                      QSize(tileRectWidth, tileRectHeight));
      if(rectangle.contains(event->pos())) {
        mSelectedTileX = x;
        mSelectedTileY = y;
        scene->update();
        update();
        emit tileClicked(x, y);
        return;
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::getThumbnailAreaEntered(QMouseEvent *event)
{
  float rectHeight = THUMB_RECT_HEIGHT_NORMAL;
  float rectWidth  = THUMB_RECT_WIDTH_NORMAL;
  if(mThumbnailAreaEntered) {
    rectHeight = THUMB_RECT_HEIGHT_ZOOMED;
    rectWidth  = THUMB_RECT_WIDTH_ZOOMED;
  }

  QRect rectangle(QPoint(width() - THUMB_RECT_START_X - rectWidth, THUMB_RECT_START_Y), QSize(rectWidth, rectHeight));
  if(rectangle.contains(event->pos())) {
    if(!mThumbnailAreaEntered) {
      mThumbnailAreaEntered = true;
      scene->update();
      update();
    }
    if(cursor() != Qt::CrossCursor) {
      setCursor(Qt::CrossCursor);
      viewport()->setCursor(Qt::CrossCursor);
    }
  } else {
    if(mThumbnailAreaEntered) {
      mThumbnailAreaEntered = false;
      scene->update();
      update();
    }
    if(cursor() == Qt::CrossCursor) {
      setCursor(Qt::OpenHandCursor);
      viewport()->setCursor(Qt::OpenHandCursor);
    }
  }
}

void PanelImageView::setThumbnailPosition(uint32_t nrOfTilesX, uint32_t nrOfTilesY, uint32_t x, uint32_t y)

{
  mNrOfTilesX    = nrOfTilesX;
  mNrOfTilesY    = nrOfTilesY;
  mSelectedTileX = x;
  mSelectedTileY = y;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::drawHistogram()
{
  const auto *image = mActPixmapOriginal.getImage();
  if(image == nullptr) {
    return;
  }

  const float RECT_START_X  = 10;
  const float RECT_START_Y  = 12;
  const float RECT_HEIGHT   = 80;
  const float NR_OF_MARKERS = 8;
  float RECT_WIDTH          = static_cast<float>(width()) - (RECT_START_X * 2);

  int type  = image->type();
  int depth = type & CV_MAT_DEPTH_MASK;
  if(depth == CV_16U) {
    if(!image->empty()) {
      // Compute the histogram
      int histSize           = UINT16_MAX + 1;
      float range[]          = {0, UINT16_MAX + 1};
      const float *histRange = {range};
      bool uniform           = true;
      bool accumulate        = false;
      cv::Mat hist;
      cv::calcHist(image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);    //, uniform, accumulate);

      // Normalize the histogram to [0, histImage.height()]
      hist.at<float>(0) = 0;    // We don't want to display black
      cv::normalize(hist, hist, 0, RECT_HEIGHT, cv::NORM_MINMAX);

      // Draw the histogram

      QPainter painter(viewport());

      // Place for the histogram
      {
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill
        QRect rectangle(width() - RECT_START_X - RECT_WIDTH, height() - RECT_START_Y - RECT_HEIGHT, RECT_WIDTH,
                        RECT_HEIGHT);    // Adjust the size as needed
        // painter.drawRect(rectangle);
      }

      float histOffset = mActPixmapOriginal.getHistogramOffset();
      float histZoom   = mActPixmapOriginal.getHitogramZoomFactor();

      int number = (float) UINT16_MAX / histZoom;

      float binWidth = (RECT_WIDTH / (float) number);
      QFont font;
      font.setPointSizeF(8);
      painter.setFont(font);

      int markerPos = number / NR_OF_MARKERS;

      for(int i = 1; i < number; i++) {
        float startX = ((float) width() - RECT_START_X - RECT_WIDTH) + (float) (i) *binWidth;
        float startY = (float) height() - RECT_START_Y;

        int idx = i + histOffset;
        if(idx > UINT16_MAX) {
          idx = UINT16_MAX;
        }
        uint16_t histValue = hist.at<float>(idx);
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        painter.drawLine(startX, startY, startX, startY - histValue);
        if(idx == mActPixmapOriginal.getUpperLevelContrast()) {
          painter.setPen(QColor(255, 0, 0));    // Set the pen color to light blue
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
          painter.drawLine(startX, startY, startX, startY - RECT_HEIGHT);
        }
        painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        if(i == 1 || i % markerPos == 0) {
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
        }

        /* painter.drawLine(binWidth * (i - 1), RECT_HEIGHT - qRound(hist.at<float>(i - 1)), binWidth * i,
                          RECT_HEIGHT - qRound(hist.at<float>(i)));*/
      }
    }
  }
}

void PanelImageView::enterEvent(QEnterEvent *)
{
}

void PanelImageView::leaveEvent(QEvent *)
{
}

void PanelImageView::setShowThumbnail(bool showThumbnail)
{
  mShowThumbnail = showThumbnail;
  viewport()->update();
  update();
}

}    // namespace joda::ui::qt
