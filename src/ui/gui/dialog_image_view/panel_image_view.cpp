///
/// \file      panel_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_image_view.hpp"
#include <qcolor.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstatictext.h>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <ranges>
#include <string>
#include <utility>
#include "backend/enums/types.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::ui::gui {

////////////////////////////////////////////////////////////////
// Image view section
//

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelImageView::PanelImageView(QWidget *parent) : QGraphicsView(parent), scene(new QGraphicsScene(this)), mImageToShow(&mPreviewImages.originalImage)
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::openImage(const std::filesystem::path &imagePath, const ome::OmeInfo *omeInfo)
{
  setWaiting(true);
  clearOverlay();
  if(omeInfo != nullptr) {
    joda::ctrl::Controller::loadImage(imagePath, mSeries, mPlane, mTile, mPreviewImages, omeInfo, mZprojection);
    mOmeInfo = *omeInfo;
  } else {
    joda::ctrl::Controller::loadImage(imagePath, mSeries, mPlane, mTile, mPreviewImages, mOmeInfo, mZprojection);
  }
  if(mLastPath != imagePath || mPlane.c != mLastPlane.c) {
    mImageToShow->autoAdjustBrightnessRange();
    mPreviewImages.thumbnail.autoAdjustBrightnessRange();
  }
  mLastPath  = imagePath;
  mLastPlane = mPlane;
  setWaiting(false);

  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::reloadImage()
{
  if(mLastPath.empty()) {
    return;
  }
  joda::ctrl::Controller::loadImage(mLastPath, mSeries, mPlane, mTile, mPreviewImages, &mOmeInfo, mZprojection);

  if(mPlane.c != mLastPlane.c) {
    mImageToShow->autoAdjustBrightnessRange();
    mPreviewImages.thumbnail.autoAdjustBrightnessRange();
  }
  mLastPlane = mPlane;
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::mutableImage() -> joda::image::Image *
{
  return mImageToShow;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setOverlay(const joda::image::Image &&overlay)
{
  mPreviewImages.overlay.setImage(std::move(*overlay.getImage()));
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setEditedImage(const joda::image::Image &&edited)
{
  mPreviewImages.editedImage.setImage(std::move(*edited.getImage()));
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setShowEditedImage(bool showEdited)
{
  if(showEdited) {
    mImageToShow = &mPreviewImages.editedImage;
  } else {
    mImageToShow = &mPreviewImages.originalImage;
  }
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setOverlayOpaque(float opaque)
{
  if(opaque > 1) {
    opaque = 1;
  }
  if(opaque < 0) {
    opaque = 0;
  }
  mOpaque = opaque;
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::clearOverlay()
{
  mPreviewImages.overlay.clear();
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::repaintImage()
{
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::repaintViewport()
{
  viewport()->update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setZprojection(enums::ZProjection projection)
{
  mZprojection = projection;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setSeries(int32_t series)
{
  mSeries = series;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setImagePlane(const joda::image::reader::ImageReader::Plane &plane)
{
  mPlane = plane;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setImageTile(int32_t tileWith, int32_t tileHeight)
{
  mTile.tileWidth  = tileWith;
  mTile.tileHeight = tileHeight;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setSelectedTile(int32_t tileX, int32_t tileY)
{
  mTile.tileX = tileX;
  mTile.tileY = tileY;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::resetImage()
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  mPlaceholderImageSet = true;
  for(QGraphicsItem *item : scene->items()) {
    if(auto *pixmapItem = dynamic_cast<QGraphicsPixmapItem *>(item)) {
      scene->removeItem(pixmapItem);
      delete pixmapItem;
    }
  }
  mActPixmap = nullptr;
  fitImageToScreenSize();
  emit updateImage();
}

void PanelImageView::onUpdateImage()
{
  auto *img = mImageToShow->getImage();
  if(img != nullptr) {
    auto pixmap = mImageToShow->getPixmap({nullptr});
    if(!mPreviewImages.overlay.empty() && mShowOverlay && mPreviewImages.overlay.getImage()->size == mImageToShow->getImage()->size) {
      pixmap = mImageToShow->getPixmap({.combineWith = &mPreviewImages.overlay, .opaque = mOpaque});
    }

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
  float zoomFactor = static_cast<float>(std::min(width(), height())) / static_cast<float>(mPixmapSize.width);
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
  QPainter painter(viewport());
  painter.setRenderHint(QPainter::Antialiasing);

  if(mActPixmap == nullptr) {
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
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill

  // Draw
  if(viewPort.width() < RECT_SIZE) {
    painter.drawRect(rectangle);
    painter.drawRect(viewPort);
  }

  // Draw thumbnail
  if(mShowThumbnail) {
    drawThumbnail(painter);
  }

  // Draw pixelInfo
  if(mShowPixelInfo) {
    // Takes 0.08ms
    drawPixelInfo(painter, width(), height() - 20, mPixelInfo);
  }

  // Waiting banner
  if(mWaiting) {
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
  // Takes 0.02 ms
  if(mShowCrosshandCursor) {
    drawCrossHairCursor(painter);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::drawCrossHairCursor(QPainter &painter)
{
  if(mLockCrosshandCursor) {
    QPoint pos{mLastCrossHairCursorPos.x(), mLastCrossHairCursorPos.y()};
    mCrossCursorInfo.mCursorPos = imageCoordinatesToPreviewCoordinates(pos);
    mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  }

  // Set the color and pen thickness for the cross lines
  QPen pen(QColor(0, 255, 0), 1);
  painter.setPen(pen);

  if(mCrossCursorInfo.mCursorPos.x() != -1 && mCrossCursorInfo.mCursorPos.y() != -1) {
    if(mLastCrossHairCursorPos.width() > 0 && mLastCrossHairCursorPos.height() > 0) {
      auto rect = imageCoordinatesToPreviewCoordinates(mLastCrossHairCursorPos);
      // Draw horizontal line at cursor's Y position
      auto stopX  = rect.x();
      auto startX = rect.x() + rect.width();
      auto yVal   = rect.y() + rect.width() / 2;
      painter.drawLine(0, yVal, stopX, yVal);
      painter.drawLine(startX, yVal, width(), yVal);

      // Draw vertical line at cursor's X position
      auto stopY  = rect.y();
      auto startY = rect.y() + rect.height();
      auto xVal   = rect.x() + rect.height() / 2;
      painter.drawLine(xVal, 0, xVal, stopY);
      painter.drawLine(xVal, startY, xVal, height());

    } else {
      // Draw horizontal line at cursor's Y position
      painter.drawLine(0, mCrossCursorInfo.mCursorPos.y(), width(), mCrossCursorInfo.mCursorPos.y());
      // Draw vertical line at cursor's X position
      painter.drawLine(mCrossCursorInfo.mCursorPos.x(), 0, mCrossCursorInfo.mCursorPos.x(), height());
    }

    if(mShowPixelInfo) {
      auto x = mCrossCursorInfo.mCursorPos.x();
      if(x < width() / 2) {
        // Switch the side
        x = x + PIXEL_INFO_RECT_WIDTH + THUMB_RECT_START_X * 2;
      }
      auto y = mCrossCursorInfo.mCursorPos.y();
      if(y < height() / 2) {
        y = y + PIXEL_INFO_RECT_HEIGHT + THUMB_RECT_START_Y * 2;
      }
      drawPixelInfo(painter, x, y, mCrossCursorInfo.pixelInfo);
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
void PanelImageView::drawPixelInfo(QPainter &painter, int32_t startX, int32_t startY, const PixelInfo &info)
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  const auto *image = mImageToShow->getImage();
  if(image == nullptr) {
    return;
  }

  QRect pixelInfoRect(QPoint(startX - THUMB_RECT_START_X - PIXEL_INFO_RECT_WIDTH, startY - THUMB_RECT_START_Y - PIXEL_INFO_RECT_HEIGHT),
                      QSize(PIXEL_INFO_RECT_WIDTH,
                            PIXEL_INFO_RECT_HEIGHT));    // Adjust the size as needed

  painter.setPen(Qt::NoPen);    // Set the pen color to light blue

  QColor transparentBlack(0, 0, 0, 127);    // 127 is approximately 50% of 255 for alpha
  painter.setBrush(transparentBlack);       // Set the brush to no brush for transparent fill
  QPainterPath path;
  path.addRoundedRect(pixelInfoRect, 8, 8);
  painter.fillPath(path, transparentBlack);
  painter.drawPath(path);

  painter.setPen(QColor(255, 255, 255));    // Set the pen color to light blue

  if(info.grayScale >= 0) {
    QString textToPrint = QString("%1, %2\nIntensity %3").arg(QString::number(info.posX)).arg(info.posY).arg(info.grayScale);
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
void PanelImageView::drawThumbnail(QPainter &painter)
{
  if(mPreviewImages.thumbnail.empty() || mOmeInfo.getNrOfSeries() < mSeries) {
    return;
  }

  auto [nrOfTilesX, nrOfTilesY] = mOmeInfo.getResolutionCount(mSeries).at(0).getNrOfTiles(mTile.tileWidth, mTile.tileHeight);

  if(nrOfTilesX <= 1 && nrOfTilesY <= 1) {
    return;
  }
  float rectHeight = THUMB_RECT_HEIGHT_NORMAL;
  float rectWidth  = THUMB_RECT_WIDTH_NORMAL;
  if(mThumbnailAreaEntered) {
    rectHeight = THUMB_RECT_HEIGHT_ZOOMED;
    rectWidth  = THUMB_RECT_WIDTH_ZOOMED;
  }

  auto *img = mPreviewImages.thumbnail.getImage();
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
  painter.drawPixmap(thumbRect, mPreviewImages.thumbnail.getPixmap({nullptr}));

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

  auto originalImageWidth  = mOmeInfo.getResolutionCount(mSeries).at(0).imageWidth;
  auto originalImageHeight = mOmeInfo.getResolutionCount(mSeries).at(0).imageHeight;

  mTileRectWidthScaled  = std::ceil(static_cast<float>(mTile.tileWidth) * (float) newWidth / (float) originalImageWidth);
  mTileRectHeightScaled = std::ceil(static_cast<float>(mTile.tileHeight) * (float) newHeight / (float) originalImageHeight);

  for(int y = 0; y < nrOfTilesY; y++) {
    for(int x = 0; x < nrOfTilesX; x++) {
      bool isSelected = false;
      if(x == mTile.tileX && y == mTile.tileY) {
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
  auto [nrOfTilesX, nrOfTilesY] = mOmeInfo.getResolutionCount(mSeries).at(0).getNrOfTiles(mTile.tileWidth, mTile.tileHeight);

  for(int y = 0; y < nrOfTilesY; y++) {
    for(int x = 0; x < nrOfTilesX; x++) {
      int xOffset = x * mTileRectWidthScaled;
      int yOffset = y * mTileRectHeightScaled;

      QRect rectangle(QPoint(width() - THUMB_RECT_START_X - mThumbRectWidth + xOffset, THUMB_RECT_START_Y + yOffset),
                      QSize(mTileRectWidthScaled, mTileRectHeightScaled));
      if(rectangle.contains(event->pos())) {
        mTile.tileX = x;
        mTile.tileY = y;
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
auto PanelImageView::getSelectedTile() -> std::pair<int32_t, int32_t>
{
  return {mTile.tileX, mTile.tileY};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfTstacks()
{
  return mOmeInfo.getNrOfTStack(mSeries);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfCstacks()
{
  return mOmeInfo.getNrOfChannels(mSeries);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfZstacks()
{
  return mOmeInfo.getNrOfZStack(mSeries);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  // Map the scene coordinates to image coordinates
  if(mActPixmap != nullptr && mImageToShow->getImage() != nullptr) {
    QPointF imagePos = mActPixmap->mapFromScene(scenePos);
    pixelInfo.posX   = imagePos.x();
    pixelInfo.posY   = imagePos.y();

    int type  = mImageToShow->getImage()->type();
    int depth = type & CV_MAT_DEPTH_MASK;
    cv::Mat image;
    if(pixelInfo.posX >= 0 && pixelInfo.posX < mImageToShow->getImage()->cols && pixelInfo.posY >= 0 &&
       pixelInfo.posY < mImageToShow->getImage()->rows) {
      if(depth == CV_16U) {
        pixelInfo.grayScale = mImageToShow->getImage()->at<uint16_t>(pixelInfo.posY, pixelInfo.posX);
        pixelInfo.redVal    = -1;
        pixelInfo.greenVal  = -1;
        pixelInfo.blueVal   = -1;
      } else {
        pixelInfo.grayScale = -1;
        pixelInfo.redVal    = mImageToShow->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[2];
        pixelInfo.greenVal  = mImageToShow->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[1];
        pixelInfo.blueVal   = mImageToShow->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[0];
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
void PanelImageView::leaveEvent(QEvent *)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setShowThumbnail(bool showThumbnail)
{
  mShowThumbnail = showThumbnail;
  viewport()->update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setShowOverlay(bool showOVerlay)
{
  mShowOverlay = showOVerlay;
  emit updateImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setShowPixelInfo(bool show)
{
  mShowPixelInfo = show;
  viewport()->update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setShowCrosshandCursor(bool show)
{
  mShowCrosshandCursor       = show;
  mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  viewport()->update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setLockCrosshandCursor(bool lock)
{
  mLockCrosshandCursor = lock;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setCursorPosition(const QPoint &pos)
{
  mCrossCursorInfo.mCursorPos = pos;
  mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  viewport()->update();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::getCursorPosition() -> QPoint
{
  return mCrossCursorInfo.mCursorPos;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setCursorPositionFromOriginalImageCoordinatesAndCenter(const QRect &boundingBox)
{
  ////////////////

  auto imgWidth    = mOmeInfo.getImageInfo(mSeries).resolutions.at(0).imageWidth;
  auto imageHeight = mOmeInfo.getImageInfo(mSeries).resolutions.at(0).imageHeight;
  auto tileWidth   = mTile.tileWidth;
  auto tileHeight  = mTile.tileHeight;
  if(imgWidth > mTile.tileWidth || imageHeight > mTile.tileHeight) {
    tileWidth  = mTile.tileWidth;
    tileHeight = mTile.tileHeight;
  } else {
    tileWidth  = imgWidth;
    tileHeight = imageHeight;
  }
  auto [tileNrX, tileNrY] = mOmeInfo.getImageInfo(mSeries).resolutions.at(0 /*resolution*/).getNrOfTiles(tileWidth, tileHeight);

  auto measBoxX = boundingBox.x() - mTile.tileX * tileWidth;
  auto measBoxY = boundingBox.y() - mTile.tileY * tileHeight;
  QRect cursorBox{(int32_t) measBoxX, (int32_t) measBoxY, (int32_t) boundingBox.width(), (int32_t) boundingBox.height()};

  //////////////////////
  if(mActPixmap != nullptr) {
    std::cout << "x: " << std::to_string(cursorBox.x()) << " y:" << std::to_string(cursorBox.y()) << std::endl;

    mLastCrossHairCursorPos = cursorBox;
    QPoint pos{cursorBox.x(), cursorBox.y()};
    auto originalPos = imageCoordinatesToPreviewCoordinates(pos);

    // Center viewport to the crosshair cursor center
    auto scrollX = originalPos.x() - viewport()->size().width() / 2;
    auto scrollY = originalPos.y() - viewport()->size().height() / 2;
    verticalScrollBar()->setValue(verticalScrollBar()->value() + scrollY);
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + scrollX);

    setCursorPosition(originalPos);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::imageCoordinatesToPreviewCoordinates(const QPoint &imageCoordinates) -> QPoint
{
  double imgX = imageCoordinates.x();
  double imgY = imageCoordinates.y();

  if(mActPixmap != nullptr) {
    QRectF sceneRect = mActPixmap->sceneBoundingRect();
    QRect viewRect   = mapFromScene(sceneRect).boundingRect();

    auto originalImageSize = mImageToShow->getOriginalImageSize();
    auto previewImageSize  = mImageToShow->getPreviewImageSize();
    auto viewPortImageSize = QSize{viewRect.width(), viewRect.height()};

    double factorX = static_cast<double>(viewPortImageSize.width()) / static_cast<double>(originalImageSize.width());
    double factorY = static_cast<double>(viewPortImageSize.height()) / static_cast<double>(originalImageSize.height());

    imgX *= factorX;
    imgY *= factorY;

    imgX += viewRect.x();
    imgY += viewRect.y();
  }
  return {(int32_t) imgX, (int32_t) imgY};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::imageCoordinatesToPreviewCoordinates(const QRect &imageCoordinates) -> QRect
{
  double imgX = imageCoordinates.x();
  double imgY = imageCoordinates.y();

  double width  = imageCoordinates.width();
  double height = imageCoordinates.height();

  if(mActPixmap != nullptr) {
    QRectF sceneRect = mActPixmap->sceneBoundingRect();
    QRect viewRect   = mapFromScene(sceneRect).boundingRect();

    auto originalImageSize = mImageToShow->getOriginalImageSize();
    auto previewImageSize  = mImageToShow->getPreviewImageSize();
    auto viewPortImageSize = QSize{viewRect.width(), viewRect.height()};

    double factorX = static_cast<double>(viewPortImageSize.width()) / static_cast<double>(originalImageSize.width());
    double factorY = static_cast<double>(viewPortImageSize.height()) / static_cast<double>(originalImageSize.height());

    imgX *= factorX;
    imgY *= factorY;

    imgX += viewRect.x();
    imgY += viewRect.y();

    width *= factorX;
    height *= factorY;
  }
  return {(int32_t) imgX, (int32_t) imgY, (int32_t) width, (int32_t) height};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setWaiting(bool waiting)
{
  mWaiting = waiting;
  update();
  viewport()->update();
}

}    // namespace joda::ui::gui
