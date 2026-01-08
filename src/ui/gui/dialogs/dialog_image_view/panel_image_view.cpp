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
#include <QtConcurrent/qtconcurrentrun.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstatictext.h>
#include <QtConcurrent/QtConcurrent>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <string>
#include <thread>
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
#include "ui/gui/dialogs/dialog_image_view/customer_painter/graphics_thumbnail.hpp"
#include "ui/gui/dialogs/widget_video_control_button_group/widget_video_control_button_group.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::ui::gui {

using namespace std::chrono_literals;

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
                               VideoControlButtonGroup *videoControl, QWidget *parent) :
    QGraphicsView(parent),
    scene(new QGraphicsScene(this)), mVideoControlButtons(videoControl), mClassSettings(classSettings), mObjectMap(objectMap)
{
  // setViewport(new QOpenGLWidget());
  setScene(scene);
  setBackgroundBrush(QBrush(Qt::black));
  scene->setBackgroundBrush(QBrush(Qt::black));
  scene->setItemIndexMethod(QGraphicsScene::NoIndex);
  // setCacheMode(QGraphicsView::CacheBackground);
  // setRenderHint(QPainter::Antialiasing, false);
  // setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  // Set up the view
  // setRenderHint(QPainter::SmoothPixmapTransform);
  //
  // setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  setDragMode(QGraphicsView::ScrollHandDrag);
  setInteractive(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setMouseTracking(true);
  setFrameShape(Shape::NoFrame);
  setCursor();

  mContourOverlay = new ContourOverlay();
  mContourOverlay->setZValue(200.0);

  mOverlayMasks = new RoiOverlay(objectMap, classSettings, mContourOverlay, parent);
  mOverlayMasks->setZValue(100.0);

  mOriginalImage = new GraphicsImagePainter();
  mOriginalImage->setZValue(50.0);

  mGraphicEditedImage = new GraphicsImagePainter();
  mGraphicEditedImage->setZValue(50.0);
  mGraphicEditedImage->setVisible(false);

  mThumbnail = new GraphicsThumbnail(&mTile);
  mThumbnail->setZValue(300.0);

  scene->addItem(mOverlayMasks);
  scene->addItem(mContourOverlay);
  scene->addItem(mOriginalImage);
  scene->addItem(mGraphicEditedImage);
  scene->addItem(mThumbnail);

  connect(mOverlayMasks, &RoiOverlay::paintedPolygonClicked, this, &PanelImageView::paintedPolygonClicked);
  connect(this, &PanelImageView::emitOpenImageFinished, this, &PanelImageView::openImageFinished);
  connect(mThumbnail, &GraphicsThumbnail::tileClicked, [this](int x, int y) {
    mTile.tileX = x;
    mTile.tileY = y;
    scheduleUpdate();
    emit tileClicked(x, y);
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelImageView::~PanelImageView()
{
  saveROI();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::shutdown()
{
  saveROI();
  storeChannelSettings();
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
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    if(mLoadingImage) {
      return;
    }
    setLoadingImage(true);
  }

  (void) QtConcurrent::run([this, imagePath, omeInfo]() { this->loadImageThread(imagePath, omeInfo); });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::loadImageThread(std::filesystem::path imagePath, const ome::OmeInfo *omeInfo)
{
  if(omeInfo != nullptr) {
    mOmeInfo = *omeInfo;
  }
  const bool loadOme = nullptr != omeInfo;

  auto *previewImage = new processor::DisplayImages();
  if(loadOme) {
    auto [tilesX, tilesY] = mOmeInfo.getImageInfo(mSeries).resolutions.at(0).getNrOfTiles(mTile.tileWidth, mTile.tileHeight);
    if(mTile.tileX > tilesX || mTile.tileY > tilesY) {
      mTile.tileX = 0;
      mTile.tileY = 0;
    }
    joda::ctrl::Controller::loadImage(imagePath, static_cast<uint16_t>(mSeries), mPlane, mTile, *previewImage, &mOmeInfo, mZprojection);
  } else {
    if(mPlane.tStack >= mOmeInfo.getNrOfTStack(mSeries)) {
      mPlane.tStack = mOmeInfo.getNrOfTStack(mSeries) - 1;
    }
    joda::ctrl::Controller::loadImage(imagePath, static_cast<uint16_t>(mSeries), mPlane, mTile, mDefaultPhysicalSize, *previewImage, mOmeInfo,
                                      mZprojection);
  }

  emit emitOpenImageFinished(imagePath, previewImage);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::openImageFinished(std::filesystem::path imagePath, processor::DisplayImages *newLoadedImages)
{
  const auto &imgInfo = mOmeInfo.getImageInfo(mSeries).resolutions;
  if(imgInfo.empty()) {
    return;
  }
  mOriginalImage->setImageToPaint(newLoadedImages->originalImage.mutableImage());
  mThumbnail->setImageToPaint(newLoadedImages->thumbnail.mutableImage(), {imgInfo.at(0).imageWidth, imgInfo.at(0).imageHeight});

  if(!mShowEditedImage) {
    mImageToShow = &newLoadedImages->originalImage;
  }

  if(mPreviewImages != nullptr) {
    newLoadedImages->originalImage.copyHistogramSettings(mPreviewImages->originalImage);
    newLoadedImages->thumbnail.copyHistogramSettings(mPreviewImages->thumbnail);
  }
  delete mPreviewImages;
  mPreviewImages = newLoadedImages;

  scene->setSceneRect(mOriginalImage->boundingRect());
  auto pixmapSize = mPreviewImages->originalImage.getPreviewImageSize();
  if(mPixmapSize != pixmapSize) {
    mPixmapSize = pixmapSize;
    fitImageToScreenSize();
  }
  if(mLastPath != imagePath) {
    if(!mLastPath.empty()) {
      storeChannelSettings();
    }
    mImageMeta.open(imagePath, mProjectPath);
  }

  if(mLastPath != imagePath) {
    saveROI();
  }

  if(mLastPath != imagePath) {
    mLastPath = imagePath;    // Don't change the order, loadROI() accesses the mLastPath
    loadROI();
    emit imageOpened();
  } else if(mLastPlane.tStack != mPlane.tStack || mLastTile != mTile) {
    emit imageOpened();
  }

  mLastPlane = mPlane;    // Don't change the order, restoreChannelSettings() accesses the mLastPlane
  mLastTile  = mTile;

  restoreChannelSettings();
  setRegionsOfInterestFromObjectList();
  repaintImage();

  setLoadingImage(false);
  emit channelOpened();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setEditedImage(joda::image::Image &&edited)
{
  {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    if(mPreviewImages == nullptr) {
      return;
    }
    auto *newEditedImage = new joda::image::Image(std::move(edited));
    mGraphicEditedImage->setImageToPaint(newEditedImage->mutableImage());
    if(nullptr != mEditedImage) {
      newEditedImage->copyHistogramSettings(*mEditedImage);
    }
    if(mShowEditedImage) {
      mImageToShow = newEditedImage;
    }
    delete mEditedImage;
    mEditedImage = newEditedImage;
  }
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
  storeChannelSettings();
  mShowEditedImage = showEdited;
  if(showEdited) {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    if(mPreviewImages == nullptr) {
      return;
    }
    mImageToShow = mEditedImage;
    mOriginalImage->setVisible(false);
    mGraphicEditedImage->setVisible(true);
  } else {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    if(mPreviewImages == nullptr) {
      return;
    }
    mImageToShow = &mPreviewImages->originalImage;
    mOriginalImage->setVisible(true);
    mGraphicEditedImage->setVisible(false);
  }
  restoreChannelSettings();
  repaintImage();
  emit channelOpened();
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
  mLastPath.clear();
  mLastPlane = {-1, -1, -1};
  mPlane     = {0, 0, 0};
  mSeries    = 0;
  {
    std::lock_guard<std::mutex> lock(mImageResetMutex);
    if(mPreviewImages != nullptr) {
      mPreviewImages->originalImage.clear();
      mPreviewImages->thumbnail.clear();
    }
  }
  mImageMeta = {};

  fitImageToScreenSize();
  scheduleUpdate();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setProjectPath(const std::filesystem::path &path)
{
  mProjectPath = path;
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
  if(mWaitBannerVisible) {
    scheduleUpdate();
  }
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  return mLastPath;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::storeChannelSettings()
{
  if(mImageToShow == nullptr) {
    return;
  }

  mImageMeta.setHistogramSetingsForChannel({.channel            = mLastPlane.cStack,
                                            .lowerLevelContrast = mImageToShow->getLowerLevelContrast(),
                                            .upperLevelContrast = mImageToShow->getUpperLevelContrast(),
                                            .lowerRange         = mImageToShow->getHistogramDisplayAreaLower(),
                                            .upperRange         = mImageToShow->getHistogramDisplayAreaUpper(),
                                            .usePseudocolors    = mImageToShow->getUsePseudoColors()},
                                           mShowEditedImage);
  if(!mProjectPath.empty() && !mLastPath.empty()) {
    mImageMeta.save(mLastPath, mProjectPath);
  }
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
  auto histoSettings = mImageMeta.getHistogramSettingsForImageChannel(mLastPlane.cStack, mShowEditedImage);

  if(histoSettings.lowerRange == 0 && histoSettings.upperRange == 0) {
    {
      std::lock_guard<std::mutex> locked(mImageResetMutex);
      if(mPreviewImages == nullptr || mImageToShow == nullptr) {
        return;
      }
      mImageToShow->autoAdjustBrightnessRange();
      mPreviewImages->thumbnail.autoAdjustBrightnessRange();
    }
    storeChannelSettings();
  } else {
    std::lock_guard<std::mutex> locked(mImageResetMutex);
    if(mImageToShow == nullptr) {
      return;
    }
    mImageToShow->setPseudoColorEnabled(histoSettings.usePseudocolors);
    mImageToShow->setBrightnessRange(histoSettings.lowerLevelContrast, histoSettings.upperLevelContrast, histoSettings.lowerRange,
                                     histoSettings.upperRange);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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

  openImage(mLastPath, &mOmeInfo);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::autoAdjustBrightnessRange()
{
  if(mImageToShow == nullptr || mPreviewImages == nullptr) {
    return;
  }
  mImageToShow->autoAdjustBrightnessRange();
  mPreviewImages->thumbnail.autoAdjustBrightnessRange();
  scheduleUpdate();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setBrightnessRange(int32_t lowerValue, int32_t upperValue, int32_t displayAreaLower, int32_t displayAreaUpper)
{
  if(mImageToShow == nullptr || mPreviewImages == nullptr) {
    return;
  }
  mImageToShow->setBrightnessRange(lowerValue, upperValue, displayAreaLower, displayAreaUpper);
  mPreviewImages->thumbnail.setBrightnessRange(lowerValue, upperValue, displayAreaLower, displayAreaUpper);
  scheduleUpdate();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setPseudoColorEnabled(bool pseudoColor)
{
  if(mImageToShow == nullptr || mPreviewImages == nullptr) {
    return;
  }
  mImageToShow->setPseudoColorEnabled(pseudoColor);
  mPreviewImages->thumbnail.setPseudoColorEnabled(pseudoColor);
  scheduleUpdate();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  const auto imagePlane = getImagePlane();
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  if(mImageToShow != nullptr && mPreviewImages != nullptr) {
    const auto &size = mImageToShow->getPreviewImageSize();
    if(mOverlayMasks != nullptr && !size.isNull() && !size.isEmpty() && size.width() > 0 && size.height() > 0) {
      mOverlayMasks->setOverlay({mPreviewImages->originalImage.getOriginalImage()->cols, mPreviewImages->originalImage.getOriginalImage()->rows},
                                {size.width(), size.height()}, getTileInfoInternal(), imagePlane);
    } else {
      mOverlayMasks->refresh(getTileInfoInternal(), imagePlane);
    }
  } else {
    mOverlayMasks->refresh(getTileInfoInternal(), imagePlane);
  }
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
void PanelImageView::setState(State state)
{
  mState = state;
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
  setCursor();
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
      QGraphicsView::setCursor(Qt::PointingHandCursor);
      viewport()->setCursor(Qt::PointingHandCursor);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  storeChannelSettings();
  mPlane = plane;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setImageChannel(int32_t ch)
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  storeChannelSettings();
  mPlane.cStack = ch;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelImageView::getImagePlane() const -> joda::enums::PlaneId
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
auto PanelImageView::getTileInfo() const -> enums::TileInfo
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  return getTileInfoInternal();
}
auto PanelImageView::getTileInfoInternal() const -> enums::TileInfo
{
  return {.tileSegment{mTile.tileX, mTile.tileY}, .tileSize{mTile.tileWidth, mTile.tileHeight}};
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
  setCursor();

  if(mState == State::MOVE) {
    if(mShowCrosshandCursor && event->button() == Qt::RightButton) {
      mCrossCursorInfo.mCursorPos = event->pos();
      mCrossCursorInfo.pixelInfo  = fetchPixelInfoFromMousePosition(event->pos());
      scheduleUpdate();
      return;
    }
    if(event->button() == Qt::LeftButton) {
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
        mRubberItem->setZValue(9999);
      } else if(mState == State::PAINT_OVAL) {
        mRubberItem = scene->addEllipse(QRectF(mPaintOrigin, mPaintOrigin), pen);
        mRubberItem->setZValue(9999);
      } else if(mState == State::PAINT_POLYGON) {
        if(!mDrawPolygon) {
          mDrawPolygon = true;
          mPolygonPoints.clear();
          mPolygonPoints.push_back(mPaintOrigin);

          mTempPolygonItem = new QGraphicsPolygonItem(QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end()));
          mTempPolygonItem->setPen(pen);
          mTempPolygonItem->setBrush(Qt::NoBrush);
          mTempPolygonItem->setZValue(9999);
          scene->addItem(mTempPolygonItem);
        } else {
          mPolygonPoints.push_back(mPaintOrigin);
          mTempPolygonItem->setPolygon(QPolygonF(mPolygonPoints.begin(), mPolygonPoints.end()));
        }
      }
      scheduleUpdate();
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
    mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());

    if(mShowCrosshandCursor) {
      mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
    }
    QGraphicsView::mouseMoveEvent(event);
  } else if(mState == State::SELECT) {
    mPixelInfo = fetchPixelInfoFromMousePosition(event->pos());

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
  setCursor();
  scheduleUpdate();
  updateCornerItemPosition();
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
  setCursor();
  updateCornerItemPosition();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  zoomImage(event->angleDelta().y() > 0);
  updateCornerItemPosition();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::resizeEvent(QResizeEvent *event)
{
  QGraphicsView::resizeEvent(event);
  updateCornerItemPosition();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  resetTransform();
  double zoomFactor = static_cast<double>(std::min(width(), height())) / static_cast<double>(mPixmapSize.width());
  scale(zoomFactor, zoomFactor);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::updateCornerItemPosition()
{
  if(mThumbnail == nullptr) {
    return;
  }

  // Fix the thumbnail to the top right
  QRectF visibleScene     = mapToScene(viewport()->rect()).boundingRect();
  const double zoomFactor = transform().m11();
  const qreal w           = mThumbnail->boundingRect().width() / zoomFactor;
  const auto marginX      = static_cast<double>(THUMB_RECT_START_X) / zoomFactor;
  const auto marginY      = (10.0 + static_cast<double>(TOP_TOOLBAR_HEIGHT)) / zoomFactor;
  mThumbnail->setPos(visibleScene.right() - w - marginX, visibleScene.top() + marginY);
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
  mPendingUpdate = false;

  // DurationCount counter("Pain event panel image view");
  QGraphicsView::paintEvent(event);
  updateCornerItemPosition();
  const float RECT_SIZE  = 80;
  const int RECT_START_X = 10;
  const int RECT_START_Y = 10 + static_cast<int>(TOP_TOOLBAR_HEIGHT);

  // Get the viewport rectangle
  QRect viewportRect = viewport()->rect();
  QPainter painter(viewport());
  painter.setRenderHint(QPainter::Antialiasing);

  QRect rectangle(RECT_START_X, RECT_START_Y, static_cast<int>(RECT_SIZE), static_cast<int>(RECT_SIZE));    // Adjust the size as needed

  float zoomFactor     = static_cast<float>(viewportTransform().m11());
  float actImageWith   = static_cast<float>(static_cast<double>(RECT_SIZE) * width() / (static_cast<double>(zoomFactor) * scene->width()));
  float actImageHeight = static_cast<float>(static_cast<double>(RECT_SIZE) * height() / (static_cast<double>(zoomFactor) * scene->width()));

  float posX = (RECT_SIZE - actImageWith) * static_cast<float>(horizontalScrollBar()->value()) / static_cast<float>(horizontalScrollBar()->maximum());
  float posY = (RECT_SIZE - actImageHeight) * static_cast<float>(verticalScrollBar()->value()) / static_cast<float>(verticalScrollBar()->maximum());

  QRect viewPort(static_cast<int>(RECT_START_X) + static_cast<int>(posX), static_cast<int>(RECT_START_Y) + static_cast<int>(posY),
                 static_cast<int>(actImageWith), static_cast<int>(actImageHeight));

  // Draw the rectangle
  painter.setPen(QColor(173, 216, 230));    // Set the pen color to light blue
  painter.setBrush(Qt::NoBrush);            // Set the brush to no brush for transparent fill

  // Draw
  if(viewPort.width() < static_cast<double>(RECT_SIZE)) {
    painter.drawRect(rectangle);
    painter.drawRect(viewPort);
  }
  // This is a transparent toolbar at the top of the image
  drawHeaderToolbar(painter);
  if(mShowCrosshandCursor) {
    drawImageInfo(painter, mPixelInfo, mCrossCursorInfo.pixelInfo);
  } else {
    drawImageInfo(painter, mPixelInfo, std::nullopt);
  }

  // Draw ruler
  if(mShowRuler) {
    drawRuler(painter);
  }

  // Waiting banner
  if((mWaiting || mLoadingImage) && mWaitBannerVisible) {
    QRect overlay(0, viewportRect.height() / 2 - 10, viewportRect.width(), 20);
    painter.setPen(QColor(0, 0, 0));      // Set the pen color to light blue
    painter.setBrush(QColor(0, 0, 0));    // Set the brush to no brush for transparent fill
    painter.drawRect(overlay);
    painter.setPen(QColor(255, 255, 255));      // Set the pen color to light blue
    painter.setBrush(QColor(255, 255, 255));    // Set the brush to no brush for transparent fill
    if(mLoadingImage) {
      painter.drawText(overlay, Qt::AlignHCenter | Qt::AlignVCenter, "Open image ...");
    } else if(mWaiting) {
      painter.drawText(overlay, Qt::AlignHCenter | Qt::AlignVCenter, "Analyzing ...");
    }
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
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::drawHeaderToolbar(QPainter &painter)
{
  QRect toolbarRect;
  if(!mShowCrosshandCursor) {
    toolbarRect = QRect(QPoint(0, 0), QSize(width(), static_cast<int32_t>(TOP_TOOLBAR_HEIGHT)));
  } else {
    toolbarRect = QRect(QPoint(0, 0), QSize(width(), static_cast<int32_t>(TOP_TOOLBAR_HEIGHT_EXTENDED)));
  }
  painter.setPen(Qt::NoPen);               // Set the pen color to light blue
  QColor transparentBlack(0, 0, 0, 80);    // 127 is approximately 50% of 255 for alpha
  painter.setBrush(transparentBlack);      // Set the brush to no brush for transparent fill
  QPainterPath path;
  path.addRect(toolbarRect);
  painter.fillPath(path, transparentBlack);
  painter.drawPath(path);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::drawImageInfo(QPainter &painter, const PixelInfo &info, const std::optional<PixelInfo> &infoCursor)
{
  mFontMedium = painter.font();
  mFontSmall  = painter.font();
  mFontSmall.setPointSize(8);

  QString leftText       = QString("Channel %1").arg(mPlane.cStack);
  auto generateRightText = [](const PixelInfo &pxInfo) -> QString {
    if(pxInfo.grayScale >= 0) {
      return QString("x=%1, y=%2, intensity=%3").arg(pxInfo.posX).arg(pxInfo.posY).arg(pxInfo.grayScale);
    } else if(pxInfo.redVal >= 0) {
      return QString("x=%1, y=%2, H=%3, S=%4, V=%5")
          .arg(pxInfo.posX)
          .arg(pxInfo.posY)
          .arg(pxInfo.grayScale)
          .arg(pxInfo.hue)
          .arg(pxInfo.saturation)
          .arg(pxInfo.value);
    }
    return "";
  };

  //  auto icon = generateSvgIcon<Style::REGULAR, Color::WHITE>("vector-three");
  painter.setPen(QColor(255, 255, 255));    // Set the pen color to light blue

  // Define drawing area
  QRect fullRect(static_cast<int32_t>(THUMB_RECT_START_X), 0, width() - static_cast<int32_t>(THUMB_RECT_START_X),
                 static_cast<int32_t>(TOP_TOOLBAR_HEIGHT));

  // Split into left and right zones
  QRect leftRect  = fullRect;
  QRect rightRect = fullRect;
  leftRect.setRight(fullRect.left() + (width() - static_cast<int32_t>(THUMB_RECT_START_X)) / 2);
  rightRect.setLeft(leftRect.right());

  // Draw left text
  painter.setFont(mFontSmall);
  painter.drawText(leftRect, Qt::AlignVCenter | Qt::AlignLeft, leftText);

  // Draw middle text
  {
    const QString middleText = mInfoText.data();
    painter.drawText(QRect(0, 0, width(), static_cast<int32_t>(TOP_TOOLBAR_HEIGHT)), Qt::AlignVCenter | Qt::AlignHCenter, middleText);
  }

  QFontMetrics fm(painter.font());
  // Draw right text
  if(!infoCursor.has_value()) {
    painter.setFont(mFontSmall);
    const auto rightText = generateRightText(info);
    QRect textRect       = fm.boundingRect(rightText);
    int textX            = rightRect.right() - textRect.width() - static_cast<int32_t>(THUMB_RECT_START_X);
    painter.drawText(QRect(textX, fullRect.top(), textRect.width(), fullRect.height()), Qt::AlignVCenter | Qt::AlignLeft, rightText);
  }
  if(infoCursor.has_value()) {
    {
      const auto rightText = generateRightText(info);
      QRect textRect       = fm.boundingRect(rightText);
      int textX            = rightRect.right() - textRect.width() - static_cast<int32_t>(THUMB_RECT_START_X);
      painter.drawText(QRect(textX, fullRect.top(), textRect.width(), fm.height()), Qt::AlignVCenter | Qt::AlignLeft, rightText);
    }
    {
      const auto rightText = generateRightText(infoCursor.value());
      QRect textRect       = fm.boundingRect(rightText);
      int textX            = rightRect.right() - textRect.width() - static_cast<int32_t>(THUMB_RECT_START_X);
      painter.drawText(QRect(textX, fullRect.top() + fm.height(), textRect.width(), fm.height()), Qt::AlignVCenter | Qt::AlignLeft, rightText);
    }
  }

  painter.setFont(mFontMedium);
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
auto PanelImageView::getSelectedTile() -> std::pair<int32_t, int32_t>
{
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  try {
    return mOmeInfo.getNrOfZStack(mSeries);
  } catch(...) {
    return 0;
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
  } else if(event->key() == Qt::Key_Escape || event->key() == Qt::Key_M) {
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
  } else if(event->key() == Qt::Key_0) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 0});
    reloadImage();
  } else if(event->key() == Qt::Key_1) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 1});
    reloadImage();
  } else if(event->key() == Qt::Key_2) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 2});
    reloadImage();
  } else if(event->key() == Qt::Key_3) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 3});
    reloadImage();
  } else if(event->key() == Qt::Key_4) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 4});
    reloadImage();
  } else if(event->key() == Qt::Key_5) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 5});
    reloadImage();
  } else if(event->key() == Qt::Key_6) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 6});
    reloadImage();
  } else if(event->key() == Qt::Key_7) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 7});
    reloadImage();
  } else if(event->key() == Qt::Key_8) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 8});
    reloadImage();
  } else if(event->key() == Qt::Key_9) {
    setImagePlane({.tStack = mPlane.tStack, .zStack = mPlane.tStack, .cStack = 9});
    reloadImage();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  mOverlayMasks->setSelectedRois(idxs);
  scheduleUpdate();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  mOverlayMasks->deleteRois(idxs);
  scheduleUpdate();
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
  QMessageBox messageBox(parentWidget());
  auto icon = joda::ui::gui::generateSvgIcon<joda::ui::gui::Style::REGULAR, joda::ui::gui::Color::YELLOW>("warning-circle");
  messageBox.setIconPixmap(icon.pixmap(42, 42));
  messageBox.setWindowTitle("Delete?");
  messageBox.setText("Delete selected annotations?");
  messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  auto *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  messageBox.setDefaultButton(noButton);
  messageBox.exec();
  if(messageBox.clickedButton() == noButton) {
    return false;
  }

  std::lock_guard<std::mutex> locked(mImageResetMutex);
  auto ret = mOverlayMasks->deleteSelectedRois();
  scheduleUpdate();
  return ret;
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  mObjectMap->erase(sourceToDelete);
  mOverlayMasks->refresh(getTileInfoInternal(), getImagePlane());
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  // Map the scene coordinates to image coordinates
  if(nullptr != mImageToShow && mImageToShow->getImage() != nullptr) {
    QPointF imagePos;
    {
      std::lock_guard<std::mutex> lock(mImageResetMutex);
      if(mPreviewImages == nullptr) {
        return {};
      }
      imagePos = mOriginalImage->mapFromScene(scenePos);
    }
    pixelInfo.posX = static_cast<int32_t>(imagePos.x());
    pixelInfo.posY = static_cast<int32_t>(imagePos.y());

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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  if(mThumbnail != nullptr) {
    mThumbnail->setVisible(showThumbnail);
  }
  scheduleUpdate();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
  mShowRois = show;
  CHECK_GUI_THREAD(mOverlayMasks)
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
void PanelImageView::setShowCrosshandCursor(bool show)
{
  mShowCrosshandCursor       = show;
  mCrossCursorInfo.pixelInfo = fetchPixelInfoFromMousePosition(mCrossCursorInfo.mCursorPos);
  scheduleUpdate();
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
  std::lock_guard<std::mutex> locked(mImageResetMutex);
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
  scheduleUpdate();
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
void PanelImageView::setInfoText(const std::string &text)
{
  mInfoText = text;
  scheduleUpdate();
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
  if(!mOmeInfo.containsSeries(mSeries)) {
    return;
  }

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

  std::lock_guard<std::mutex> locked(mImageResetMutex);
  if(mPreviewImages == nullptr || mImageToShow == nullptr) {
    return {};
  }
  QRectF sceneRect = mOriginalImage->sceneBoundingRect();
  QRect viewRect   = mapFromScene(sceneRect).boundingRect();

  auto originalImageSize = mImageToShow->getOriginalImageSize();
  // auto previewImageSize  = mImageToShow->getPreviewImageSize();
  auto viewPortImageSize = QSize{viewRect.width(), viewRect.height()};

  double factorX = static_cast<double>(viewPortImageSize.width()) / static_cast<double>(originalImageSize.width());
  double factorY = static_cast<double>(viewPortImageSize.height()) / static_cast<double>(originalImageSize.height());

  imgX *= factorX;
  imgY *= factorY;

  imgX += viewRect.x();
  imgY += viewRect.y();

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

  std::lock_guard<std::mutex> locked(mImageResetMutex);
  if(mPreviewImages == nullptr || mImageToShow == nullptr) {
    return {};
  }
  QRectF sceneRect       = mOriginalImage->sceneBoundingRect();
  QRect viewRect         = mapFromScene(sceneRect).boundingRect();
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

  return {static_cast<int32_t>(imgX), static_cast<int32_t>(imgY), static_cast<int32_t>(width), static_cast<int32_t>(height)};
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setLoadingImage(bool waiting)
{
  if(mLoadingImage == waiting) {
    return;
  }
  mLoadingImage = waiting;
  scheduleUpdate();
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
  if(mWaiting == waiting) {
    return;
  }
  mWaiting = waiting;
  scheduleUpdate();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::setWaitBannerVisible(bool waiting)
{
  mWaitBannerVisible = waiting;
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
  cv::Size imageSize{};
  {
    std::lock_guard<std::mutex> lock(mImageResetMutex);
    if(mPreviewImages == nullptr || mImageToShow == nullptr) {
      return;
    }
    imageSize = {mPreviewImages->originalImage.getOriginalImage()->cols, mPreviewImages->originalImage.getOriginalImage()->rows};
  }

  const auto &size = mImageToShow->getPreviewImageSize();
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
                             {{mTile.tileX, mTile.tileY}, tileSize});
  paintedRoi.setCategory(joda::atom::ROI::Category::MANUAL_SEGMENTATION);

  mObjectMap->triggerStartChangeCallback();
  mObjectMap->push_back(paintedRoi);
  mObjectMap->triggerChangeCallback();
  mObjectMap->triggerManuelAnnotationAdded();
  setRegionsOfInterestFromObjectList();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::scheduleUpdate()
{
  if(!mPendingUpdate) {
    mPendingUpdate = true;
    // Schedule update on the next event loop iteration
    QTimer::singleShot(0, this, [this]() { viewport()->update(); });
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::saveROI()
{
  try {
    if(mLastPath.empty() || mProjectPath.empty()) {
      return;
    }
    auto storagePathNew = joda::helper::generateImageMetaDataStoragePathFromImagePath(mLastPath, mProjectPath,
                                                                                      joda::fs::FILE_NAME_ANNOTATIONS + joda::fs::EXT_ANNOTATION);

    std::unique_ptr<atom::ObjectList> tmp = std::make_unique<atom::ObjectList>();
    if(std::filesystem::exists(storagePathNew)) {
      tmp->deserializeWithoutGivenTimeStack(storagePathNew, mLastPlane.tStack);
    }

    const auto &tmpList = mObjectMap->getObjectList();
    for(const auto &[_, roi] : *tmpList) {
      tmp->push_back(*roi);
    }

    tmp->serialize(storagePathNew);

  } catch(const std::exception &ex) {
    joda::log::logError("WindowMain::saveROI:" + std::string(ex.what()));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelImageView::loadROI()
{
  if(mLastPath.empty() || mProjectPath.empty()) {
    return;
  }
  auto storagePathNew = joda::helper::generateImageMetaDataStoragePathFromImagePath(mLastPath, mProjectPath,
                                                                                    joda::fs::FILE_NAME_ANNOTATIONS + joda::fs::EXT_ANNOTATION);
  mObjectMap->triggerStartChangeCallback();
  if(std::filesystem::exists(storagePathNew)) {
    mObjectMap->deserialize(storagePathNew, mPlane.tStack);
  } else {
    mObjectMap->clearAll();
  }

  mObjectMap->triggerChangeCallback();
}

}    // namespace joda::ui::gui
