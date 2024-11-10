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
#include <qcolor.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <string>
#include "backend/helper/image/image.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::ui {

////////////////////////////////////////////////////////////////
// Image view section
//
PanelImageView::PanelImageView(const joda::image::Image &imageReference, const joda::image::Image &thumbnailImageReference, bool isEditedImage,
                               QWidget *parent) :
    QGraphicsView(parent),
    mActPixmapOriginal(imageReference), mThumbnailImageReference(thumbnailImageReference), scene(new QGraphicsScene(this)),
    mIsEditedImage(isEditedImage)
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
  setCursor(Qt::PointingHandCursor);
  viewport()->setCursor(Qt::PointingHandCursor);

  connect(this, &PanelImageView::updateImage, this, &PanelImageView::onUpdateImage);
}

void PanelImageView::setState(State state)
{
  mState = state;
  switch(mState) {
    case MOVE:
      viewport()->setCursor(QCursor(Qt::PointingHandCursor));
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
  if(mShowPixelInfo) {
    mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());
  }
  if(mShowCrosshandCursor) {
    mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
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
void PanelImageView::mouseReleaseEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    // End dragging
    if(cursor() != Qt::PointingHandCursor) {
      setCursor(Qt::PointingHandCursor);
      viewport()->setCursor(Qt::PointingHandCursor);
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
  if(mShowCrosshandCursor && event->button() == Qt::RightButton) {
    mCrossCursorInfo.mCursorPos = event->pos();
    mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(event->pos());
    viewport()->update();
    emit onImageRepainted();
    return;
  }

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

  float posX = (RECT_SIZE - actImageWith) * (float) horizontalScrollBar()->value() / (float) horizontalScrollBar()->maximum();
  float posY = (RECT_SIZE - actImageHeight) * (float) verticalScrollBar()->value() / (float) verticalScrollBar()->maximum();

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
  if(!mIsEditedImage) {
    drawHistogram();
  }

  // Draw thumbnail
  if(mShowThumbnail) {
    drawThumbnail();
  }

  // Draw pixelInfo
  if(mShowPixelInfo && !mIsEditedImage) {
    drawPixelInfo(width(), height(), mPixelInfo);
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

  //
  // Paint cross cursor
  //
  if(mShowCrosshandCursor) {
    QPainter painter(viewport());
    // Set the color and pen thickness for the cross lines
    QPen pen(Qt::blue, 2);
    painter.setPen(pen);

    if(mCrossCursorInfo.mCursorPos.x() != -1 && mCrossCursorInfo.mCursorPos.y() != -1) {
      // Draw horizontal line at cursor's Y position
      painter.drawLine(0, mCrossCursorInfo.mCursorPos.y(), width(), mCrossCursorInfo.mCursorPos.y());

      // Draw vertical line at cursor's X position
      painter.drawLine(mCrossCursorInfo.mCursorPos.x(), 0, mCrossCursorInfo.mCursorPos.x(), height());

      if(!mIsEditedImage) {
        drawPixelInfo(mCrossCursorInfo.mCursorPos.x(), mCrossCursorInfo.mCursorPos.y(), mCrossCursorInfo.pixelInfo);
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

void PanelImageView::drawPixelInfo(int32_t startX, int32_t startY, const PixelInfo &info)
{
  const auto *image = mActPixmapOriginal.getImage();
  if(image == nullptr) {
    return;
  }
  QPainter painter(viewport());

  QRect pixelInfoRect(QPoint(startX - THUMB_RECT_START_X - PIXEL_INFO_RECT_WIDTH, startY - THUMB_RECT_START_Y - PIXEL_INFO_RECT_HEIGHT),
                      QSize(PIXEL_INFO_RECT_WIDTH,
                            PIXEL_INFO_RECT_HEIGHT));    // Adjust the size as needed

  painter.setPen(Qt::NoPen);    // Set the pen color to light blue

  QColor transparentBlack(0, 0, 0, 127);    // 127 is approximately 50% of 255 for alpha
  painter.setBrush(transparentBlack);       // Set the brush to no brush for transparent fill
  painter.drawRect(pixelInfoRect);

  painter.setPen(QColor(255, 255, 255));    // Set the pen color to light blue

  if(info.grayScale >= 0) {
    QString textToPrint = QString("%1, %2\nI %3").arg(QString::number(info.posX)).arg(info.posY).arg(info.grayScale);
    painter.drawText(pixelInfoRect, Qt::AlignCenter, textToPrint);
  } else if(info.redVal >= 0) {
    QString textToPrint =
        QString("%1, %2\nH %3, S %4, V %5").arg(QString::number(info.posX)).arg(info.posY).arg(info.hue).arg(info.saturation).arg(info.value);
    painter.drawText(pixelInfoRect, Qt::AlignCenter, textToPrint);
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
  if(mThumbnailParameter.nrOfTilesX <= 1 && mThumbnailParameter.nrOfTilesY <= 1) {
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
  int newWidth;
  int newHeight;
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
  if(newWidth < 0 || newHeight < 0) {
    return;
  }

  QRect thumbRect(QPoint(width() - THUMB_RECT_START_X - rectWidth, THUMB_RECT_START_Y), QSize(newWidth,
                                                                                              newHeight));    // Adjust the size as needed
  painter.drawPixmap(thumbRect, mThumbnailImageReference.getPixmap());

  //
  // Draw bounding rect
  //
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill
  QRect rectangle(QPoint(width() - THUMB_RECT_START_X - rectWidth, THUMB_RECT_START_Y), QSize(newWidth,
                                                                                              newHeight));    // Adjust the size as needed
  painter.drawRect(rectangle);

  mThumbRectWidth  = newWidth;
  mThumbRectHeight = newHeight;

  //
  // Draw grid
  //
  // float tileRectWidth  = newWidth / mNrOfTilesX;
  // float tileRectHeight = newHeight / mNrOfTilesY;

  /// \todo Hier muss man aufgrunden
  mTileRectWidthScaled =
      std::ceil(static_cast<float>(mThumbnailParameter.tileWidth) * (float) newWidth / (float) mThumbnailParameter.originalImageWidth);
  mTileRectHeightScaled =
      std::ceil(static_cast<float>(mThumbnailParameter.tileHeight) * (float) newHeight / (float) mThumbnailParameter.originalImageHeight);

  for(int y = 0; y < mThumbnailParameter.nrOfTilesY; y++) {
    for(int x = 0; x < mThumbnailParameter.nrOfTilesX; x++) {
      bool isSelected = false;
      if(x == mThumbnailParameter.selectedTileX && y == mThumbnailParameter.selectedTileY) {
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
        float xOffset = std::floor(static_cast<float>(x) * static_cast<float>(mTileRectWidthScaled));
        float yOffset = std::floor(static_cast<float>(y) * static_cast<float>(mTileRectHeightScaled));
        QRect tileRect(QPoint(width() - THUMB_RECT_START_X - static_cast<float>(newWidth) + xOffset, THUMB_RECT_START_Y + yOffset),
                       QSize(mTileRectWidthScaled, mTileRectHeightScaled));

        if(tileRect.x() + tileRect.width() > rectangle.x() + rectangle.width()) {
          auto newWidth = (tileRect.x() + tileRect.width()) - (rectangle.x() + rectangle.width());
          tileRect.setWidth(tileRect.width() - newWidth);
        }
        if(tileRect.y() + tileRect.height() > rectangle.y() + rectangle.height()) {
          auto newHeight = (tileRect.y() + tileRect.height()) - (rectangle.y() + rectangle.height());
          tileRect.setHeight(tileRect.height() - newHeight);
        }

        painter.drawRect(tileRect);
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
  for(int y = 0; y < mThumbnailParameter.nrOfTilesY; y++) {
    for(int x = 0; x < mThumbnailParameter.nrOfTilesX; x++) {
      int xOffset = x * mTileRectWidthScaled;
      int yOffset = y * mTileRectHeightScaled;

      QRect rectangle(QPoint(width() - THUMB_RECT_START_X - mThumbRectWidth + xOffset, THUMB_RECT_START_Y + yOffset),
                      QSize(mTileRectWidthScaled, mTileRectHeightScaled));
      if(rectangle.contains(event->pos())) {
        mThumbnailParameter.selectedTileX = x;
        mThumbnailParameter.selectedTileY = y;
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
  QRect rectangle(QPoint(width() - THUMB_RECT_START_X - mThumbRectWidth, THUMB_RECT_START_Y), QSize(mThumbRectWidth, mThumbRectHeight));
  if(rectangle.contains(event->pos())) {
    if(!mThumbnailAreaEntered) {
      mThumbnailAreaEntered = true;
    }
    if(cursor() != Qt::CrossCursor) {
      setCursor(Qt::CrossCursor);
      viewport()->setCursor(Qt::CrossCursor);
    }
  } else {
    if(mThumbnailAreaEntered) {
      mThumbnailAreaEntered = false;
    }
    if(cursor() == Qt::CrossCursor) {
      setCursor(Qt::PointingHandCursor);
      viewport()->setCursor(Qt::PointingHandCursor);
    }
  }
}

void PanelImageView::setThumbnailPosition(const ThumbParameter &param)

{
  mThumbnailParameter = param;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::fetchPixelInfoFromMousePosition(const QPoint &viewPos) const -> PixelInfo
{
  // Map the view coordinates to scene coordinates
  QPointF scenePos = mapToScene(viewPos);
  PixelInfo pixelInfo;

  // Map the scene coordinates to image coordinates
  if(mActPixmap != nullptr) {
    QPointF imagePos = mActPixmap->mapFromScene(scenePos);
    pixelInfo.posX   = imagePos.x();
    pixelInfo.posY   = imagePos.y();

    int type  = mActPixmapOriginal.getImage()->type();
    int depth = type & CV_MAT_DEPTH_MASK;
    cv::Mat image;
    if(pixelInfo.posX >= 0 && pixelInfo.posX < mActPixmapOriginal.getImage()->cols && pixelInfo.posY >= 0 &&
       pixelInfo.posY < mActPixmapOriginal.getImage()->rows) {
      if(depth == CV_16U) {
        pixelInfo.grayScale = mActPixmapOriginal.getImage()->at<uint16_t>(pixelInfo.posY, pixelInfo.posX);
        pixelInfo.redVal    = -1;
        pixelInfo.greenVal  = -1;
        pixelInfo.blueVal   = -1;
      } else {
        pixelInfo.grayScale = -1;
        pixelInfo.redVal    = mActPixmapOriginal.getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[2];
        pixelInfo.greenVal  = mActPixmapOriginal.getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[1];
        pixelInfo.blueVal   = mActPixmapOriginal.getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[0];
        QColor color(pixelInfo.redVal, pixelInfo.greenVal, pixelInfo.blueVal);
        color.getHsv(&pixelInfo.hue, &pixelInfo.saturation, &pixelInfo.value);
      }
    }
  }
  return pixelInfo;
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

void PanelImageView::leaveEvent(QEvent *)
{
}

void PanelImageView::setShowThumbnail(bool showThumbnail)
{
  mShowThumbnail = showThumbnail;
  viewport()->update();
}

void PanelImageView::setShowPixelInfo(bool show)
{
  mShowPixelInfo = show;
  viewport()->update();
}

void PanelImageView::setShowCrosshandCursor(bool show)
{
  mShowCrosshandCursor       = show;
  mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  viewport()->update();
}

void PanelImageView::setCursorPosition(const QPoint &pos)
{
  mCrossCursorInfo.mCursorPos = pos;
  mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(pos);
  viewport()->update();
}
auto PanelImageView::getCursorPosition() -> QPoint
{
  return mCrossCursorInfo.mCursorPos;
}

}    // namespace joda::ui
