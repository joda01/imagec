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
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/image/image.hpp"
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::ui::gui {

////////////////////////////////////////////////////////////////
// Image view section
//
PanelImageView::PanelImageView(const joda::image::Image *imageReference, const joda::image::Image *thumbnailImageReference,
                               const joda::image::Image *overlay, bool withThumbnail, QWidget *parent) :
    QGraphicsView(parent),
    mActPixmapOriginal(imageReference), mThumbnailImageReference(thumbnailImageReference), mOverlayImage(overlay), scene(new QGraphicsScene(this)),
    mWithThumbnail(withThumbnail)
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

void PanelImageView::setImageReference(const joda::image::Image *imageReference)
{
  mActPixmapOriginal = imageReference;
  emit updateImage();
}

void PanelImageView::imageUpdated(const ctrl::Preview::PreviewResults &previewResult, const std::map<enums::ClassIdIn, QString> &classes)
{
  if(mThumbnailImageReference == nullptr) {
    return;
  }
  mClasses        = classes;
  mPipelineResult = previewResult;

  updatePipelineResultsCoordinates();
  /////////////////////////////////////////////////////
  const_cast<joda::image::Image *>(mThumbnailImageReference)->autoAdjustBrightnessRange();
  emit updateImage();
}

void PanelImageView::updatePipelineResultsCoordinates()
{
  auto xOffset = width() - THUMB_RECT_START_X - RESULTS_INFO_RECT_WIDTH;
  auto yOffset = THUMB_RECT_START_Y;
  QRect pixelInfoRect(QPoint(xOffset, THUMB_RECT_START_Y), QSize(RESULTS_INFO_RECT_WIDTH,
                                                                 RESULTS_INFO_RECT_HEIGHT));    // Adjust the size as needed

  yOffset += THUMB_RECT_START_Y;
  auto addToTextCoordinated = [this, &xOffset, &yOffset](const QString &tmp, enums::ClassId classId) -> int32_t {
    QTextDocument doc;
    doc.setHtml(tmp);
    QSizeF size  = doc.size();
    qreal width  = size.width();
    qreal height = size.height();
    height       = 16;

    mClassesCoordinates.emplace_back(QRect(xOffset, yOffset, RESULTS_INFO_RECT_WIDTH, height), static_cast<enums::ClassIdIn>(classId));
    return height;
  };

  mClassesCoordinates.clear();
  /////////////////////////////////////////////////////
  QString info = "<html>";
  for(const auto &[classId, count] : mPipelineResult.foundObjects) {
    QString prefix = "□ ";
    if(mSelectedClasses.contains(static_cast<enums::ClassIdIn>(classId))) {
      prefix = "▣ ";
    }
    QString tmp = prefix + "<span style=\"color: " + QString(count.color.data()) + ";\">" +
                  (mClasses.at(static_cast<enums::ClassIdIn>(classId)) + "</span>: " + QString::number(count.count) + "<br>");

    yOffset += addToTextCoordinated(tmp, classId);
    info += tmp;
  }
  if(mPipelineResult.isOverExposed) {
    QString tmp = "<span style=\"color: #750000;\">Image may be overexposed</span><br>";
    yOffset += addToTextCoordinated(tmp, enums::ClassId::NONE);
    info += tmp;
  }
  if(mPipelineResult.noiseDetected) {
    QString tmp = "<span style=\"color: #750000;\">Image may be noisy</span><br>";
    yOffset += addToTextCoordinated(tmp, enums::ClassId::NONE);
    info += tmp;
  }
  info += "</html>";
  mPipelineResultsHtmlText = info;
}

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
  auto *img = mActPixmapOriginal->getImage();
  if(img != nullptr) {
    auto pixmap = mActPixmapOriginal->getPixmap(nullptr);
    if(mOverlayImage != nullptr && mShowOverlay) {
      pixmap = mActPixmapOriginal->getPixmap(mOverlayImage);
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
  if(mShowThumbnail && mWithThumbnail) {
    getThumbnailAreaEntered(event);
  }
  if(mShowPixelInfo) {
    mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());
  }
  if(mShowPipelineResults && !mWithThumbnail) {
    getPreviewResultsAreaEntered(event);
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

  if(mShowPipelineResults && !mWithThumbnail) {
    if(getPreviewResultsAreaClicked(event)) {
      return;
    }
  }

  if(event->button() == Qt::LeftButton) {
    // Start dragging
    if(cursor() != Qt::ClosedHandCursor) {
      setCursor(Qt::ClosedHandCursor);
      viewport()->setCursor(Qt::ClosedHandCursor);
    }

    isDragging = true;
    lastPos    = event->pos();
    if(mShowThumbnail && mWithThumbnail) {
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

  // Draw histogram
  if(mShowHistogram) {
    // Takes 5ms
    drawHistogram(painter);
  }

  // Draw thumbnail
  if(mShowThumbnail && mWithThumbnail) {
    drawThumbnail(painter);
  }

  // Draw pixelInfo
  if(mShowPixelInfo && mShowHistogram) {
    // Takes 0.08ms
    drawPixelInfo(painter, width(), height() - 20, mPixelInfo);
  }

  // Draw pipeline result
  if(mShowPipelineResults && !mWithThumbnail) {
    drawPipelineResult(painter);
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
    // Set the color and pen thickness for the cross lines
    QPen pen(Qt::blue, 2);
    painter.setPen(pen);

    if(mCrossCursorInfo.mCursorPos.x() != -1 && mCrossCursorInfo.mCursorPos.y() != -1) {
      // Draw horizontal line at cursor's Y position
      painter.drawLine(0, mCrossCursorInfo.mCursorPos.y(), width(), mCrossCursorInfo.mCursorPos.y());

      // Draw vertical line at cursor's X position
      painter.drawLine(mCrossCursorInfo.mCursorPos.x(), 0, mCrossCursorInfo.mCursorPos.x(), height());

      if(mShowHistogram && mShowPixelInfo) {
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
  const auto *image = mActPixmapOriginal->getImage();
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
  path.addRoundedRect(pixelInfoRect, 10, 10);
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
void PanelImageView::drawPipelineResult(QPainter &painter)
{
  /// \todo this needs not beeing updated all the time
  updatePipelineResultsCoordinates();
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  const auto *image = mActPixmapOriginal->getImage();
  if(image == nullptr) {
    return;
  }
  int32_t startY = 0;
  QRect pixelInfoRect(QPoint(width() - THUMB_RECT_START_X - RESULTS_INFO_RECT_WIDTH, THUMB_RECT_START_Y),
                      QSize(RESULTS_INFO_RECT_WIDTH,
                            RESULTS_INFO_RECT_HEIGHT));    // Adjust the size as needed

  painter.setPen(Qt::NoPen);    // Set the pen color to light blue

  QColor transparentBlack(0, 0, 0, 127);    // 127 is approximately 50% of 255 for alpha
  painter.setBrush(transparentBlack);       // Set the brush to no brush for transparent fill
  QPainterPath path;
  path.addRoundedRect(pixelInfoRect, 10, 10);
  painter.fillPath(path, transparentBlack);
  painter.drawPath(path);

  painter.setPen(QColor(255, 255, 255));    // Set the pen color to light blue

  //
  // Paint the results
  //

  QStaticText text(mPipelineResultsHtmlText);
  QPoint pText = pixelInfoRect.topLeft();
  pText.setX(pText.x() + THUMB_RECT_START_X);
  pText.setY(pText.y() + THUMB_RECT_START_Y);
  painter.drawStaticText(pText, text);
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
  if(mThumbnailImageReference == nullptr) {
    return;
  }
  if(mThumbnailParameter.nrOfTilesX <= 1 && mThumbnailParameter.nrOfTilesY <= 1) {
    return;
  }
  float rectHeight = THUMB_RECT_HEIGHT_NORMAL;
  float rectWidth  = THUMB_RECT_WIDTH_NORMAL;
  if(mThumbnailAreaEntered) {
    rectHeight = THUMB_RECT_HEIGHT_ZOOMED;
    rectWidth  = THUMB_RECT_WIDTH_ZOOMED;
  }

  auto *img = mThumbnailImageReference->getImage();
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
  painter.drawPixmap(thumbRect, mThumbnailImageReference->getPixmap(nullptr));

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
bool PanelImageView::getPreviewResultsAreaEntered(QMouseEvent *event)
{
  auto pos = event->pos();

  QRect pixelInfoRect(QPoint(width() - THUMB_RECT_START_X - RESULTS_INFO_RECT_WIDTH, THUMB_RECT_START_Y),
                      QSize(RESULTS_INFO_RECT_WIDTH,
                            RESULTS_INFO_RECT_HEIGHT));    // Adjust the size as needed

  if(pixelInfoRect.contains(pos)) {
    return true;
  }
  return false;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool PanelImageView::getPreviewResultsAreaClicked(QMouseEvent *event)
{
  auto pos = event->pos();

  QRect pixelInfoRect(QPoint(width() - THUMB_RECT_START_X - RESULTS_INFO_RECT_WIDTH, THUMB_RECT_START_Y),
                      QSize(RESULTS_INFO_RECT_WIDTH,
                            RESULTS_INFO_RECT_HEIGHT));    // Adjust the size as needed

  if(pixelInfoRect.contains(pos)) {
    for(const auto &[classPos, classId] : mClassesCoordinates) {
      if(classPos.contains(pos)) {
        // QMessageBox::information(this, "Information", "Clicked: " + QString::number((int32_t) classId));
        if(mSelectedClasses.contains(classId)) {
          mSelectedClasses.erase(classId);
        } else {
          mSelectedClasses.emplace(classId);
        }
        viewport()->update();
        emit classesToShowChanged(mSelectedClasses);
        break;
      }
    }
    return true;
  }
  return false;
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
  if(mActPixmap != nullptr && mActPixmapOriginal->getImage() != nullptr) {
    QPointF imagePos = mActPixmap->mapFromScene(scenePos);
    pixelInfo.posX   = imagePos.x();
    pixelInfo.posY   = imagePos.y();

    int type  = mActPixmapOriginal->getImage()->type();
    int depth = type & CV_MAT_DEPTH_MASK;
    cv::Mat image;
    if(pixelInfo.posX >= 0 && pixelInfo.posX < mActPixmapOriginal->getImage()->cols && pixelInfo.posY >= 0 &&
       pixelInfo.posY < mActPixmapOriginal->getImage()->rows) {
      if(depth == CV_16U) {
        pixelInfo.grayScale = mActPixmapOriginal->getImage()->at<uint16_t>(pixelInfo.posY, pixelInfo.posX);
        pixelInfo.redVal    = -1;
        pixelInfo.greenVal  = -1;
        pixelInfo.blueVal   = -1;
      } else {
        pixelInfo.grayScale = -1;
        pixelInfo.redVal    = mActPixmapOriginal->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[2];
        pixelInfo.greenVal  = mActPixmapOriginal->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[1];
        pixelInfo.blueVal   = mActPixmapOriginal->getImage()->at<cv::Vec3b>(pixelInfo.posY, pixelInfo.posX)[0];
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
void PanelImageView::drawHistogram(QPainter &painter)
{
  const auto *image = mActPixmapOriginal->getImage();
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
      // Place for the histogram
      QFont font;
      font.setPointSizeF(8);
      painter.setFont(font);
      painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
      painter.setBrush(Qt::NoBrush);         // Set the brush to no brush for transparent fill

      // Precalculation
      float histOffset    = mActPixmapOriginal->getHistogramOffset();
      float histZoom      = mActPixmapOriginal->getHitogramZoomFactor();
      int number          = (float) UINT16_MAX / histZoom;
      float binWidth      = (RECT_WIDTH / static_cast<float>(number));
      int markerPos       = number / NR_OF_MARKERS;
      const auto &hist    = mActPixmapOriginal->getHistogram();
      int32_t compression = 1;

      if(number > UINT16_MAX / 2) {
        compression = 2;
      }

      for(int i = 1; i < number; i += compression) {
        int idx = i + histOffset;
        if(idx > UINT16_MAX) {
          idx = UINT16_MAX;
        }
        float startX    = (static_cast<float>(width()) - RECT_START_X - RECT_WIDTH) + static_cast<float>(i) * binWidth;
        float startY    = static_cast<float>(height()) - RECT_START_Y;
        float histValue = hist.at<float>(idx) * RECT_HEIGHT;
        painter.drawLine(startX, startY, startX, startY - histValue);
        if(idx == mActPixmapOriginal->getUpperLevelContrast() || (compression != 1 && idx + 1 == mActPixmapOriginal->getUpperLevelContrast())) {
          painter.setPen(QColor(255, 0, 0));    // Set the pen color to red
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
          painter.drawLine(startX, startY, startX, startY - RECT_HEIGHT);
          painter.setPen(QColor(0, 89, 179));    // Set the pen color to light blue
        }
        if(i == 1 || i % markerPos == 0) {
          painter.drawText(QRect(startX - 50, startY, 100, 12), Qt::AlignHCenter, std::to_string(idx).data());
        }
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

void PanelImageView::setShowHistogram(bool showHistorgram)
{
  mShowHistogram = showHistorgram;
  viewport()->update();
}

void PanelImageView::setShowOverlay(bool showOVerlay)
{
  mShowOverlay = showOVerlay;
  emit updateImage();
}

void PanelImageView::setShowPixelInfo(bool show)
{
  mShowPixelInfo = show;
  viewport()->update();
}

void PanelImageView::setShowPipelineResults(bool show)
{
  mShowPipelineResults = show;
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
  mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  viewport()->update();
}
auto PanelImageView::getCursorPosition() -> QPoint
{
  return mCrossCursorInfo.mCursorPos;
}

void PanelImageView::setCursorPositionFromOriginalImageCoordinates(const QPoint &pos)
{
  setCursorPosition(imageCoordinatesToPreviewCoordinates(pos));
}

auto PanelImageView::imageCoordinatesToPreviewCoordinates(const QPoint &imageCoordinates) -> QPoint
{
  double imgX = imageCoordinates.x();
  double imgY = imageCoordinates.y();

  if(mActPixmap != nullptr) {
    QRectF sceneRect = mActPixmap->sceneBoundingRect();
    QRect viewRect   = mapFromScene(sceneRect).boundingRect();
    std::cout << "--------------------" << std::endl;

    auto originalImageSize = mActPixmapOriginal->getOriginalImageSize();
    auto previewImageSize  = mActPixmapOriginal->getPreviewImageSize();
    auto viewPortImageSize = QSize{viewRect.width(), viewRect.height()};

    qDebug() << "Displayed image size in view:" << viewRect.size();
    qDebug() << "Displayed image size in view X:" << viewRect.x();
    qDebug() << "Displayed image size in view Y:" << viewRect.y();
    qDebug() << "Viewport image size in view:" << viewPortImageSize;
    qDebug() << "Original image size in view:" << originalImageSize;

    double factorX = static_cast<double>(viewPortImageSize.width()) / static_cast<double>(originalImageSize.width());
    double factorY = static_cast<double>(viewPortImageSize.height()) / static_cast<double>(originalImageSize.height());

    imgX *= factorX;
    imgY *= factorY;

    imgX += viewRect.x();
    imgY += viewRect.y();

    qDebug() << "Coordinates " << std::to_string(imgX) << " | " << std::to_string(imgY);
    std::cout << "--------------------" << std::endl;
  }
  return {(int32_t) imgX, (int32_t) imgY};
}

}    // namespace joda::ui::gui
