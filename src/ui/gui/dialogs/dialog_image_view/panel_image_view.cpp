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
#include <qbrush.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstatictext.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <ranges>
#include <string>
#include <utility>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/classifier/classifier_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/fnv1a.hpp"
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include "ui/gui/dialogs/dialog_image_view/customer_painter/graphics_roi_overlay.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
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
PanelImageView::PanelImageView(const std::shared_ptr<atom::ObjectList> &objectMap, const joda::settings::Classification *classSettings,
                               QWidget *parent) :
    QGraphicsView(parent),
    mImageToShow(&mPreviewImages.originalImage), scene(new QGraphicsScene(this)), mClassSettings(classSettings), mObjectMap(objectMap)
{
  // setViewport(new QOpenGLWidget());
  setScene(scene);
  setBackgroundBrush(QBrush(Qt::black));
  scene->setBackgroundBrush(QBrush(Qt::black));

  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  setCacheMode(QGraphicsView::CacheBackground);
  setRenderHint(QPainter::Antialiasing, false);
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

  // Set up the view
  setRenderHint(QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setInteractive(true);
  setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMouseTracking(true);

  setFrameShape(Shape::NoFrame);
  setCursor();

  mContourOverlay = new ContourOverlay();
  mContourOverlay->setZValue(200.0);

  mOverlayMasks = new RoiOverlay(objectMap, classSettings, mContourOverlay, parent);
  mOverlayMasks->setZValue(100.0);

  scene->addItem(mOverlayMasks);
  scene->addItem(mContourOverlay);

  connect(this, &PanelImageView::updateImage, this, &PanelImageView::onUpdateImage);
  connect(mOverlayMasks, &RoiOverlay::paintedPolygonClicked, this, &PanelImageView::paintedPolygonClicked);
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

  if(omeInfo != nullptr) {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    joda::ctrl::Controller::loadImage(imagePath, static_cast<uint16_t>(mSeries), mPlane, mTile, mPreviewImages, omeInfo, mZprojection);
    mOmeInfo = *omeInfo;
  } else {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    joda::ctrl::Controller::loadImage(imagePath, static_cast<uint16_t>(mSeries), mPlane, mTile, mDefaultPhysicalSize, mPreviewImages, mOmeInfo,
                                      mZprojection);
  }
  restoreChannelSettings();

  mLastPath  = imagePath;
  mLastPlane = mPlane;
  setWaiting(false);
  repaintImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::getCurrentImagePath() const -> std::filesystem::path
{
  return mLastPath;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::restoreChannelSettings()
{
  auto key = SettingsIdx{.imageChannel = static_cast<uint16_t>(mPlane.cStack), .isEdited = mShowEditedImage};
  if(mChannelSettings.contains(key)) {
    const auto &tmp = mChannelSettings.at(key);
    if(tmp.mDisplayAreaLower <= 1 && tmp.mDisplayAreaUpper <= 1) {
      goto ADJUST;    // The histo of an empty image was calculates
    }
    {
      std::lock_guard<std::mutex> locked(mImageResetMutex);
      mImageToShow->setBrightnessRange(tmp.mLowerValue, tmp.mUpperValue, tmp.mDisplayAreaLower, tmp.mDisplayAreaUpper);
    }
    mPreviewImages.thumbnail.autoAdjustBrightnessRange();
  } else {
  ADJUST : {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    mImageToShow->autoAdjustBrightnessRange();
    mPreviewImages.thumbnail.autoAdjustBrightnessRange();
    mChannelSettings.emplace(key, ChannelSettings{
                                      .mLowerValue       = mImageToShow->getLowerLevelContrast(),
                                      .mUpperValue       = mImageToShow->getUpperLevelContrast(),
                                      .mDisplayAreaLower = mImageToShow->getHistogramDisplayAreaLower(),
                                      .mDisplayAreaUpper = mImageToShow->getHistogramDisplayAreaUpper(),
                                  });
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
void PanelImageView::setDefaultPhysicalSize(const joda::settings::ProjectImageSetup::PhysicalSizeSettings &set)
{
  if(mDefaultPhysicalSize.mode == enums::PhysicalSizeMode::Manual) {
    mDefaultPhysicalSize = set;
    mOmeInfo.setPhyiscalSize(joda::ome::PhyiscalSize{
        static_cast<double>(set.pixelWidth),
        static_cast<double>(set.pixelHeight),
        0,
        set.pixelSizeUnit,
    });
  } else {
    mDefaultPhysicalSize = {};
    mOmeInfo.setPhyiscalSize({});
  }
  mDefaultPhysicalSize.pixelSizeUnit = set.pixelSizeUnit;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::getPhysicalSizeSettings() const -> const joda::settings::ProjectImageSetup::PhysicalSizeSettings &
{
  return mDefaultPhysicalSize;
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
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    joda::ctrl::Controller::loadImage(mLastPath, static_cast<uint16_t>(mSeries), mPlane, mTile, mPreviewImages, &mOmeInfo, mZprojection);
  }
  restoreChannelSettings();
  mLastPlane = mPlane;
  repaintImage();
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
auto PanelImageView::getImage() const -> const joda::image::Image *
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
void PanelImageView::setRegionsOfInterestFromObjectList()
{
  const auto &size = mImageToShow->getPreviewImageSize();
  if(mOverlayMasks != nullptr) {
    mOverlayMasks->setOverlay({mPreviewImages.originalImage.getOriginalImage()->cols, mPreviewImages.originalImage.getOriginalImage()->rows},
                              {size.width(), size.height()});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::refreshRoiColors()
{
  mOverlayMasks->refresh();
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
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    mPreviewImages.editedImage.setImage(std::move(*edited.getImage()));
  }
  restoreChannelSettings();
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
  mShowEditedImage = showEdited;
  if(showEdited) {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    mImageToShow = &mPreviewImages.editedImage;
  } else {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    mImageToShow = &mPreviewImages.originalImage;
  }
  restoreChannelSettings();
  repaintImage();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setRoisOpaque(float opaque)
{
  if(opaque > 1) {
    opaque = 1;
  }
  if(opaque < 0) {
    opaque = 0;
  }
  mOpaque = opaque;
  mOverlayMasks->setAlpha(mOpaque);
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
  auto key = SettingsIdx{.imageChannel = static_cast<uint16_t>(mPlane.cStack), .isEdited = mShowEditedImage};
  if(mChannelSettings.contains(key)) {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    mChannelSettings[key] = ChannelSettings{
        .mLowerValue       = mImageToShow->getLowerLevelContrast(),
        .mUpperValue       = mImageToShow->getUpperLevelContrast(),
        .mDisplayAreaLower = mImageToShow->getHistogramDisplayAreaLower(),
        .mDisplayAreaUpper = mImageToShow->getHistogramDisplayAreaUpper(),
    };
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
void PanelImageView::setState(State state)
{
  mState = state;
  setCursor();
  if(state == State::MOVE) {
    setRoisSelectable(false);
    setDragMode(QGraphicsView::ScrollHandDrag);
  } else if(state == State::SELECT) {
    setRoisSelectable(true);
    setDragMode(QGraphicsView::NoDrag);
  } else {
    setRoisSelectable(false);
    setDragMode(QGraphicsView::NoDrag);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setClassIdToUseForDrawing(enums::ClassId classId, const QColor &color)

{
  mSelectedClassForDrawing = classId;
  mPixelClassColor         = color;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setCursor()
{
  switch(mState) {
    case MOVE:
      if(mThumbnailAreaEntered) {
        QGraphicsView::setCursor(Qt::CrossCursor);
        viewport()->setCursor(Qt::CrossCursor);
      } else {
        QGraphicsView::setCursor(Qt::PointingHandCursor);
        viewport()->setCursor(Qt::PointingHandCursor);
      }
      break;
    case SELECT:
      QGraphicsView::setCursor(Qt::ArrowCursor);
      viewport()->setCursor(Qt::ArrowCursor);
      break;
    case PAINT_RECTANGLE:
    case PAINT_OVAL:
    case PAINT_POLYGON:
    case PAIN_BRUSH:
      QGraphicsView::setCursor(Qt::CrossCursor);
      viewport()->setCursor(Qt::CrossCursor);
      break;
    case PAINT_MAGIC_WAND:
      break;
  }
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
auto PanelImageView::getZprojection() const -> enums::ZProjection
{
  return mZprojection;
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
int32_t PanelImageView::getSeries() const
{
  return mSeries;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setImagePlane(const joda::enums::PlaneId &plane)
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
auto PanelImageView::getImagePlane() const -> const joda::enums::PlaneId &
{
  return mPlane;
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
  if(scene == nullptr) {
    return;
  }
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);

    mPlaceholderImageSet = true;
    for(QGraphicsItem *item : scene->items()) {
      if(item == mOverlayMasks) {
        continue;
      }
      if(item == mContourOverlay) {
        continue;
      }
      if(auto *pixmapItem = dynamic_cast<QGraphicsPixmapItem *>(item)) {
        scene->removeItem(pixmapItem);
        delete pixmapItem;
      }
    }
    mActPixmap = nullptr;
  }
  fitImageToScreenSize();
  emit updateImage();
}

void PanelImageView::onUpdateImage()
{
  cv::Size size;
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    auto *img = mImageToShow->getImage();
    if(img == nullptr) {
      return;
    }
    size = img->size();
  }
  auto pixmap = mImageToShow->getPixmap();
  scene->setSceneRect(pixmap.rect());
  if(nullptr == mActPixmap) {
    mActPixmap = scene->addPixmap(pixmap);
  } else {
    mActPixmap->setPixmap(pixmap);
  }

  if((size.width != mPixmapSize.width) || (size.height != mPixmapSize.height) || mPlaceholderImageSet) {
    mPixmapSize = size;
    if(size.width > 0 && size.height > 0) {
      fitImageToScreenSize();
    }
    mPlaceholderImageSet = false;
  } else {
    emit onImageRepainted();
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
void PanelImageView::mousePressEvent(QMouseEvent *event)
{
  QGraphicsView::mousePressEvent(event);

  if(mState == State::MOVE) {
    if(mShowCrosshandCursor && event->button() == Qt::RightButton) {
      mCrossCursorInfo.mCursorPos = event->pos();
      mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(event->pos());
      viewport()->update();
      emit onImageRepainted();
      return;
    }
    if(event->button() == Qt::LeftButton) {
      if(mShowThumbnail) {
        getClickedTileInThumbnail(event);
      }
    }

  } else if(mState == State::SELECT) {
  } else {
    if(event->button() == Qt::LeftButton) {
      // Now start a new drawing
      auto pen = QPen(Qt::blue, 3, Qt::DashLine);
      pen.setCosmetic(true);
      // Start rectangle in scene coordinates
      mPaintOrigin = mapToScene(event->pos());
      if(mState == State::PAINT_RECTANGLE) {
        mRubberItem = scene->addRect(QRectF(mPaintOrigin, mPaintOrigin), pen);
      } else if(mState == State::PAINT_OVAL) {
        mRubberItem = scene->addEllipse(QRectF(mPaintOrigin, mPaintOrigin), pen);
      } else if(mState == State::PAINT_POLYGON) {
        if(!mDrawPolygon) {
          mDrawPolygon = true;
          mPolygonPoints.clear();
          mPolygonPoints.push_back(mPaintOrigin);

          mTempPolygonItem = new QGraphicsPolygonItem(QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end()));
          mTempPolygonItem->setPen(pen);
          mTempPolygonItem->setBrush(Qt::NoBrush);
          scene->addItem(mTempPolygonItem);
        } else {
          mPolygonPoints.push_back(mPaintOrigin);
          mTempPolygonItem->setPolygon(QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end()));
        }
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
void PanelImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton && mDrawPolygon && mPolygonPoints.size() >= 3) {
    if(mState == State::PAINT_POLYGON) {
      // Finish polygon
      mTempPolygonItem->setPolygon(QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end()));    // final update

      addPolygonToToObjectMap(mTempPolygonItem->polygon());

      // Remove the temporary rubber rectangle
      scene->removeItem(mTempPolygonItem);
      delete mTempPolygonItem;
      mTempPolygonItem = nullptr;
      mPolygonPoints.clear();
      mDrawPolygon = false;
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
void PanelImageView::mouseMoveEvent(QMouseEvent *event)
{
  if(mState == State::MOVE) {
    if(mShowThumbnail) {
      getThumbnailAreaEntered(event);
    }
    if(mShowPixelInfo) {
      mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());
    }
    if(mShowCrosshandCursor) {
      mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
    }
    QGraphicsView::mouseMoveEvent(event);
  } else if(mState == State::SELECT) {
    if(mShowPixelInfo) {
      mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());
    }
    if(mShowCrosshandCursor) {
      mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
    }
    QGraphicsView::mouseMoveEvent(event);
  } else {
    if(mRubberItem != nullptr) {
      QPointF current = mapToScene(event->pos());
      QRectF rect(mPaintOrigin, current);
      rect = rect.normalized();
      if(mState == State::PAINT_RECTANGLE) {
        dynamic_cast<QGraphicsRectItem *>(mRubberItem)->setRect(rect);
      } else if(mState == State::PAINT_OVAL) {
        dynamic_cast<QGraphicsEllipseItem *>(mRubberItem)->setRect(rect);
      }
    }
    if(mState == State::PAINT_POLYGON) {
      if(mDrawPolygon && !mPolygonPoints.empty()) {
        // Update temporary polygon with "rubber line" to cursor
        QPolygonF polyWithCursor = QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end());
        polyWithCursor.push_back(mapToScene(event->pos()));
        if(mTempPolygonItem != nullptr) {
          mTempPolygonItem->setPolygon(polyWithCursor);
        }
      }
    }
    QGraphicsView::mouseMoveEvent(event);
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
QPolygonF ellipseToPolygon(QGraphicsEllipseItem *ellipse)
{
  QRectF rect = ellipse->rect();

  qreal rx = rect.width() / 2.0;
  qreal ry = rect.height() / 2.0;

  // 1. Determine the maximum radius (R)
  qreal maxRadius = qMax(rx, ry);

  // 2. Calculate the minimum required segments for sub-pixel smoothness (f=0.5)
  //    segments = ceil(PI * sqrt(R))
  int segments = qCeil(M_PI * qSqrt(maxRadius));

  // 3. Ensure a minimum number for objects too small to calculate accurately
  //    (e.g., set a minimum of 8 for a visible shape)
  segments = qMax(9, segments);

  // 4. Paint polygon
  QPolygonF poly;
  qreal cx = rect.center().x();
  qreal cy = rect.center().y();

  for(int i = 0; i < segments; ++i) {
    qreal angle = (2 * M_PI * i) / segments;
    qreal x     = cx + rx * cos(angle);
    qreal y     = cy + ry * sin(angle);
    poly << QPointF(x, y);
  }

  return poly;
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
  if(mState == State::MOVE) {
    QGraphicsView::mouseReleaseEvent(event);
  } else if(mState == State::SELECT) {
    QGraphicsView::mouseReleaseEvent(event);
  } else {
    if(mRubberItem != nullptr) {
      QPolygonF poly;

      if(mState == State::PAINT_RECTANGLE) {
        QRectF rect = dynamic_cast<QGraphicsRectItem *>(mRubberItem)->rect();
        poly << rect.topLeft() << rect.topRight() << rect.bottomRight() << rect.bottomLeft();
      } else if(mState == State::PAINT_OVAL) {
        poly = ellipseToPolygon(dynamic_cast<QGraphicsEllipseItem *>(mRubberItem));
      }

      addPolygonToToObjectMap(poly);

      // Remove the temporary rubber rectangle
      scene->removeItem(mRubberItem);
      delete mRubberItem;
      mRubberItem = nullptr;
    }
    QGraphicsView::mouseReleaseEvent(event);
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
  double zoomFactor = static_cast<double>(std::min(width(), height())) / static_cast<double>(mPixmapSize.width);
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

  QRect rectangle(RECT_START_X, RECT_START_Y, static_cast<int>(RECT_SIZE), static_cast<int>(RECT_SIZE));    // Adjust the size as needed

  float zoomFactor     = static_cast<float>(viewportTransform().m11());
  float actImageWith   = static_cast<float>(static_cast<double>(RECT_SIZE) * width() / (static_cast<double>(zoomFactor) * scene->width()));
  float actImageHeight = static_cast<float>(static_cast<double>(RECT_SIZE) * height() / (static_cast<double>(zoomFactor) * scene->width()));

  float posX = (RECT_SIZE - actImageWith) * static_cast<float>(horizontalScrollBar()->value()) / static_cast<float>(horizontalScrollBar()->maximum());
  float posY = (RECT_SIZE - actImageHeight) * static_cast<float>(verticalScrollBar()->value()) / static_cast<float>(verticalScrollBar()->maximum());

  QRect viewPort(static_cast<int>(RECT_START_X + posX), static_cast<int>(RECT_START_Y + posY), static_cast<int>(actImageWith),
                 static_cast<int>(actImageHeight));

  // Draw the rectangle
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill

  // Draw
  if(viewPort.width() < static_cast<double>(RECT_SIZE)) {
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

  // Draw ruler
  if(mShowRuler) {
    drawRuler(painter);
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
  QPen pen(QColor(0, 255, 0), 3);
  pen.setCosmetic(true);
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
        x = x + static_cast<int32_t>(PIXEL_INFO_RECT_WIDTH) + static_cast<int32_t>(THUMB_RECT_START_X) * 2;
      }
      auto y = mCrossCursorInfo.mCursorPos.y();
      if(y < height() / 2) {
        y = y + static_cast<int32_t>(PIXEL_INFO_RECT_HEIGHT) + static_cast<int32_t>(THUMB_RECT_START_Y) * 2;
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
  QRect pixelInfoRect(QPoint(startX - static_cast<int32_t>(THUMB_RECT_START_X) - static_cast<int32_t>(PIXEL_INFO_RECT_WIDTH),
                             startY - static_cast<int32_t>(THUMB_RECT_START_Y) - static_cast<int32_t>(PIXEL_INFO_RECT_HEIGHT)),
                      QSize(static_cast<int32_t>(PIXEL_INFO_RECT_WIDTH),
                            static_cast<int32_t>(PIXEL_INFO_RECT_HEIGHT)));    // Adjust the size as needed

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
void PanelImageView::drawRuler(QPainter &painter)
{
  //
  // Calc
  //
  const auto &physk = mOmeInfo.getPhyiscalSize(mSeries);
  QTransform t      = transform();
  qreal scaleX      = t.m11();
  //  qreal scaleY      = t.m22();
  auto [sizeX, sizeY, sizeZ] = physk.getPixelSize(mDefaultPhysicalSize.pixelSizeUnit);
  double onePxSize           = sizeX / scaleX;

  double unitToShow = 500.0;
  switch(mDefaultPhysicalSize.pixelSizeUnit) {
    case enums::Units::um:
    case enums::Units::Undefined:
    case enums::Units::Pixels:
      unitToShow *= 1;
      break;
    case enums::Units::nm:
      unitToShow *= 10e3;
      break;
    case enums::Units::mm:
      unitToShow /= 10e3;
      break;
    case enums::Units::cm:
      unitToShow /= 10e4;
      break;
    case enums::Units::m:
      unitToShow /= 10e6;
      break;
    case enums::Units::km:
      unitToShow /= 10e9;
      break;
  }

  double rulerSize = unitToShow / onePxSize;
  while(rulerSize > 100) {
    if(unitToShow > 20) {
      unitToShow -= 20;
    } else if(unitToShow > 1) {
      unitToShow -= 1;
    } else {
      unitToShow -= 0.25;
    }
    rulerSize = unitToShow / onePxSize;
  }

  //
  // Draw
  //

  QColor transparentWhite(255, 255, 255, 127);    // 127 is approximately 50% of 255 for alpha
  painter.setBrush(transparentWhite);             // Set the brush to no brush for transparent fill
  QPen tmp = painter.pen();
  QPen pen(Qt::black);
  pen.setColor(transparentWhite);
  pen.setWidth(3);          // 2 px
  pen.setCosmetic(true);    // stays 2 px regardless of view transforms/DPI
  painter.setPen(pen);
  painter.drawLine(static_cast<int32_t>(THUMB_RECT_START_X), height() - 30,
                   static_cast<int32_t>(static_cast<int32_t>(THUMB_RECT_START_X) + rulerSize), height() - 30);

  painter.setPen(tmp);
  nlohmann::json j = mDefaultPhysicalSize.pixelSizeUnit;    // thanks to to_json generated by macro

  QString textToPrint = QString("%1 %2").arg(QString::number(static_cast<double>(unitToShow))).arg(j.get<std::string>().c_str());
  painter.drawText(QRect(static_cast<int32_t>(THUMB_RECT_START_X), height() - 20, static_cast<int32_t>(rulerSize), 10), Qt::AlignCenter, textToPrint);
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
  if(mPreviewImages.thumbnail.empty() || static_cast<int32_t>(mOmeInfo.getNrOfSeries()) < mSeries) {
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

  const auto *img = mPreviewImages.thumbnail.getImage();
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
    newWidth  = static_cast<int32_t>(rectWidth);
    newHeight = static_cast<int>(rectWidth / aspectRatio);
  } else {
    // Height is the limiting factor
    newHeight = static_cast<int32_t>(rectHeight);
    newWidth  = static_cast<int>(rectHeight * aspectRatio);
  }
  if(newWidth < 0 || newHeight < 0) {
    return;
  }

  QRect thumbRect(
      QPoint(static_cast<int32_t>(static_cast<float>(width()) - THUMB_RECT_START_X - rectWidth), static_cast<int32_t>(THUMB_RECT_START_Y)),
      QSize(newWidth,
            newHeight));    // Adjust the size as needed
  painter.drawPixmap(thumbRect, mPreviewImages.thumbnail.getPixmap());

  //
  // Draw bounding rect
  //
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill
  QRect rectangle(
      QPoint(static_cast<int32_t>(static_cast<float>(width()) - THUMB_RECT_START_X - rectWidth), static_cast<int32_t>(THUMB_RECT_START_Y)),
      QSize(newWidth,
            newHeight));    // Adjust the size as needed
  painter.drawRect(rectangle);

  mThumbRectWidth  = static_cast<uint32_t>(newWidth);
  mThumbRectHeight = static_cast<uint32_t>(newHeight);

  //
  // Draw grid
  //
  // float tileRectWidth  = newWidth / mNrOfTilesX;
  // float tileRectHeight = newHeight / mNrOfTilesY;

  auto originalImageWidth  = mOmeInfo.getResolutionCount(mSeries).at(0).imageWidth;
  auto originalImageHeight = mOmeInfo.getResolutionCount(mSeries).at(0).imageHeight;

  mTileRectWidthScaled =
      static_cast<int32_t>(std::ceil(static_cast<float>(mTile.tileWidth) * static_cast<float>(newWidth) / static_cast<float>(originalImageWidth)));
  mTileRectHeightScaled =
      static_cast<int32_t>(std::ceil(static_cast<float>(mTile.tileHeight) * static_cast<float>(newHeight) / static_cast<float>(originalImageHeight)));

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
        QRect tileRect(QPoint(static_cast<int32_t>(static_cast<float>(width()) - THUMB_RECT_START_X - static_cast<float>(newWidth) + xOffset),
                              static_cast<int32_t>(THUMB_RECT_START_Y + yOffset)),
                       QSize(mTileRectWidthScaled, mTileRectHeightScaled));

        if(tileRect.x() + tileRect.width() > rectangle.x() + rectangle.width()) {
          auto newWidthTmp = (tileRect.x() + tileRect.width()) - (rectangle.x() + rectangle.width());
          tileRect.setWidth(tileRect.width() - newWidthTmp);
        }
        if(tileRect.y() + tileRect.height() > rectangle.y() + rectangle.height()) {
          auto newHeightTmp = (tileRect.y() + tileRect.height()) - (rectangle.y() + rectangle.height());
          tileRect.setHeight(tileRect.height() - newHeightTmp);
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
  try {
    auto [nrOfTilesX, nrOfTilesY] = mOmeInfo.getResolutionCount(mSeries).at(0).getNrOfTiles(mTile.tileWidth, mTile.tileHeight);

    for(int y = 0; y < nrOfTilesY; y++) {
      for(int x = 0; x < nrOfTilesX; x++) {
        int xOffset = x * mTileRectWidthScaled;
        int yOffset = y * mTileRectHeightScaled;

        QRect rectangle(QPoint(static_cast<int32_t>(static_cast<float>(width()) - THUMB_RECT_START_X - static_cast<float>(mThumbRectWidth) +
                                                    static_cast<float>(xOffset)),
                               static_cast<int32_t>(THUMB_RECT_START_Y + static_cast<float>(yOffset))),
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
  } catch(...) {
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
auto PanelImageView::getOmeInfo() const -> const ome::OmeInfo &
{
  return mOmeInfo;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfTstacks() const
{
  try {
    return mOmeInfo.getNrOfTStack(mSeries);
  } catch(...) {
    return 0;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfCstacks() const
{
  try {
    return mOmeInfo.getNrOfChannels(mSeries);
  } catch(...) {
    return 0;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t PanelImageView::getNrOfZstacks() const
{
  try {
    return mOmeInfo.getNrOfZStack(mSeries);
  } catch(...) {
    return 0;
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
  QRect rectangle(
      QPoint(width() - static_cast<int32_t>(THUMB_RECT_START_X) - static_cast<int32_t>(mThumbRectWidth), static_cast<int32_t>(THUMB_RECT_START_Y)),
      QSize(static_cast<int32_t>(mThumbRectWidth), static_cast<int32_t>(mThumbRectHeight)));
  if(rectangle.contains(event->pos())) {
    if(!mThumbnailAreaEntered) {
      mThumbnailAreaEntered = true;
      setCursor();
    }
  } else {
    if(mThumbnailAreaEntered) {
      mThumbnailAreaEntered = false;
      setCursor();
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Delete) {
    deleteSelectedRois();
  } else if(event->key() == Qt::Key_Escape) {
    setState(State::MOVE);
    emit drawingToolChanged(State::MOVE);
  } else if(event->key() == Qt::Key_S) {
    setState(State::SELECT);
    emit drawingToolChanged(State::SELECT);
  } else if(event->key() == Qt::Key_R) {
    setState(State::PAINT_RECTANGLE);
    emit drawingToolChanged(State::PAINT_RECTANGLE);
  } else if(event->key() == Qt::Key_O) {
    setState(State::PAINT_OVAL);
    emit drawingToolChanged(State::PAINT_OVAL);
  } else if(event->key() == Qt::Key_P) {
    setState(State::PAINT_POLYGON);
    emit drawingToolChanged(State::PAINT_POLYGON);
  } else if(event->key() == Qt::Key_B) {
    setState(State::PAIN_BRUSH);
    emit drawingToolChanged(State::PAIN_BRUSH);
  } else if(event->key() == Qt::Key_W) {
    setState(State::PAIN_BRUSH);
    emit drawingToolChanged(State::PAIN_BRUSH);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setSelectedRois(const std::set<joda::atom::ROI *> &idxs)
{
  // ==============================
  // First rest old selections
  // ==============================
  mOverlayMasks->setSelectedRois(idxs);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::deleteRois(const std::set<joda::atom::ROI *> &idxs)
{
  mOverlayMasks->deleteRois(idxs);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
bool PanelImageView::deleteSelectedRois()
{
  return mOverlayMasks->deleteSelectedRois();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::clearRegionOfInterest(joda::atom::ROI::Category sourceToDelete)
{
  mObjectMap->erase(sourceToDelete);
  mOverlayMasks->refresh();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setFillRois(bool fill)
{
  mFillRoi = fill;
  if(mOverlayMasks != nullptr) {
    mOverlayMasks->setAlpha(mOpaque);
    mOverlayMasks->setFill(fill);
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
    pixelInfo.posX   = static_cast<int32_t>(imagePos.x());
    pixelInfo.posY   = static_cast<int32_t>(imagePos.y());

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
void PanelImageView::setShowRois(bool show)
{
  mShowRois = show;
  mOverlayMasks->setVisible(show);
  mContourOverlay->setVisible(show);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setRoisSelectable(bool selectable)
{
  mSelectable = selectable;
  mOverlayMasks->setSelectable(selectable);
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
  // auto [tileNrX, tileNrY] = mOmeInfo.getImageInfo(mSeries).resolutions.at(0 /*resolution*/).getNrOfTiles(tileWidth, tileHeight);

  auto measBoxX = boundingBox.x() - mTile.tileX * tileWidth;
  auto measBoxY = boundingBox.y() - mTile.tileY * tileHeight;
  QRect cursorBox{static_cast<int32_t>(measBoxX), static_cast<int32_t>(measBoxY), static_cast<int32_t>(boundingBox.width()),
                  static_cast<int32_t>(boundingBox.height())};

  //////////////////////
  if(mActPixmap != nullptr) {
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
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    auto originalImageSize = mImageToShow->getOriginalImageSize();
    // auto previewImageSize  = mImageToShow->getPreviewImageSize();
    auto viewPortImageSize = QSize{viewRect.width(), viewRect.height()};

    double factorX = static_cast<double>(viewPortImageSize.width()) / static_cast<double>(originalImageSize.width());
    double factorY = static_cast<double>(viewPortImageSize.height()) / static_cast<double>(originalImageSize.height());

    imgX *= factorX;
    imgY *= factorY;

    imgX += viewRect.x();
    imgY += viewRect.y();
  }
  return {static_cast<int32_t>(imgX), static_cast<int32_t>(imgY)};
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

    std::lock_guard<std::mutex> locked(mImageResetMutex);
    auto originalImageSize = mImageToShow->getOriginalImageSize();
    // auto previewImageSize  = mImageToShow->getPreviewImageSize();
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
  return {static_cast<int32_t>(imgX), static_cast<int32_t>(imgY), static_cast<int32_t>(width), static_cast<int32_t>(height)};
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

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::addPolygonToToObjectMap(const QPolygonF &poly)
{
  const auto &size   = mImageToShow->getPreviewImageSize();
  cv::Size imageSize = {mPreviewImages.originalImage.getOriginalImage()->cols, mPreviewImages.originalImage.getOriginalImage()->rows};
  cv::Size originalImageSize{mOmeInfo.getImageWidth(mSeries, 0), mOmeInfo.getImageHeight(mSeries, 0)};
  cv::Size previewSize = {size.width(), size.height()};
  cv::Size tileSize    = {mTile.tileWidth, mTile.tileHeight};

  double scaleX = static_cast<double>(imageSize.width) / static_cast<double>(previewSize.width);
  double scaleY = static_cast<double>(imageSize.height) / static_cast<double>(previewSize.height);

  // Convert to cv::Point
  std::vector<cv::Point> contour;
  contour.reserve(static_cast<size_t>(poly.size()));
  for(int i = 0; i < poly.size(); ++i) {
    contour.emplace_back(static_cast<int>(static_cast<double>(poly[i].x()) * scaleX), static_cast<int>(static_cast<double>(poly[i].y()) * scaleY));
  }

  // Make contour
  auto boundingBox = cv::boundingRect(contour);
  cv::Mat mask     = cv::Mat::zeros(boundingBox.size(), CV_8UC1);

  // Bring the contours box in the area of the bounding box
  for(auto &point : contour) {
    point.x = point.x - boundingBox.x;
    point.y = point.y - boundingBox.y;
  }

  std::vector<std::vector<cv::Point>> contours = {contour};
  cv::drawContours(mask, contours, -1, cv::Scalar(255), cv::FILLED);

  joda::atom::ROI paintedRoi(atom::ROI::RoiObjectId{.classId = mSelectedClassForDrawing, .imagePlane = mPlane}, 1.0, boundingBox, mask, contour,
                             imageSize, originalImageSize, {mTile.tileX, mTile.tileY}, tileSize);
  paintedRoi.setCategory(joda::atom::ROI::Category::MANUAL_SEGMENTATION);

  mObjectMap->push_back(paintedRoi);
  mObjectMap->triggerChangeCallback();
  setRegionsOfInterestFromObjectList();
}

}    // namespace joda::ui::gui
