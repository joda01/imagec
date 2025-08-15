///
/// \file      panel_image_view.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qlabel.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include <opencv2/core/types.hpp>

namespace joda::ui::gui {

///
/// \class      PanelImageView
/// \author     Joachim Danmayr
///
class PanelImageView : public QGraphicsView
{
  Q_OBJECT
public:
  struct PixelInfo
  {
    int32_t posX       = 0;
    int32_t posY       = 0;
    int32_t grayScale  = -1;
    int32_t redVal     = -1;
    int32_t greenVal   = -1;
    int32_t blueVal    = -1;
    int32_t hue        = -1;
    int32_t saturation = -1;
    int32_t value      = -1;
  };

  struct CrossCursorInfo
  {
    QPoint mCursorPos;
    PixelInfo pixelInfo;
  };

  struct SettingsIdx
  {
    uint16_t imageChannel = 0;
    bool isEdited         = false;

    bool operator<(const SettingsIdx &other) const
    {
      auto edit  = static_cast<uint32_t>(isEdited);
      auto editO = static_cast<uint32_t>(other.isEdited);
      return (static_cast<uint32_t>(imageChannel) << 16 | edit) < (static_cast<uint32_t>(other.imageChannel) << 16 | editO);
    }
  };

  struct ChannelSettings
  {
    uint16_t mLowerValue       = 0;
    uint16_t mUpperValue       = UINT16_MAX;
    uint16_t mDisplayAreaLower = 0;
    uint16_t mDisplayAreaUpper = 0;
  };

  /////////////////////////////////////////////////////
  PanelImageView(QWidget *parent = nullptr);
  void openImage(const std::filesystem::path &imagePath, const ome::OmeInfo *omeInfo = nullptr);
  void setOverlay(const joda::image::Image &&overlay);
  void setEditedImage(const joda::image::Image &&edited);
  void clearOverlay();
  void reloadImage();
  void repaintImage();
  void repaintViewport();
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);
  void setWaiting(bool waiting);
  void setShowThumbnail(bool);
  void setShowPixelInfo(bool);
  void setShowOverlay(bool);
  void setOverlayOpaque(float opaque);
  void setShowEditedImage(bool);
  void setShowCrosshandCursor(bool);
  void setLockCrosshandCursor(bool);
  void setCursorPosition(const QPoint &pos);
  void setCursorPositionFromOriginalImageCoordinatesAndCenter(const QRect &boundingRect);
  auto getCursorPosition() -> QPoint;
  auto getSelectedTile() -> std::pair<int32_t, int32_t>;
  auto getOmeInfo() const -> const ome::OmeInfo &;
  int32_t getNrOfTstacks() const;
  int32_t getNrOfCstacks() const;
  int32_t getNrOfZstacks() const;

  // INFORMATION NEEDED FROM EXTERNAL ///////////////////////////////////////////////////
  void setZprojection(enums::ZProjection);
  void setSeries(int32_t);
  void setImagePlane(const joda::image::reader::ImageReader::Plane &);
  void setSelectedTile(int32_t tileX, int32_t tileY);
  void setImageTile(int32_t tileWith, int32_t tileHeight);
  void setDefaultPhysicalSize(const joda::settings::ProjectImageSetup::PhysicalSizeSettings &);
  auto mutableImage() -> joda::image::Image *;

signals:
  /////////////////////////////////////////////////////
  void classesToShowChanged(const settings::ObjectInputClasses &selectedClasses);
  void updateImage();
  void onImageRepainted();
  void tileClicked(int32_t tileX, int32_t tileY);

private:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void drawThumbnail(QPainter &);
  void drawCrossHairCursor(QPainter &);
  void drawPixelInfo(QPainter &, int32_t startX, int32_t startY, const PixelInfo &info);
  void drawRuler(QPainter &);
  void getClickedTileInThumbnail(QMouseEvent *event);
  void getThumbnailAreaEntered(QMouseEvent *event);
  auto fetchPixelInfoFromMousePosition(const QPoint &pos) const -> PixelInfo;
  auto imageCoordinatesToPreviewCoordinates(const QPoint &imageCoordinates) -> QPoint;
  auto imageCoordinatesToPreviewCoordinates(const QRect &imageCoordinates) -> QRect;

private:
  /////////////////////////////////////////////////////
  const float THUMB_RECT_START_X       = 10;
  const float THUMB_RECT_START_Y       = 10;
  const float THUMB_RECT_HEIGHT_NORMAL = 128;
  const float THUMB_RECT_WIDTH_NORMAL  = 128;

  const float THUMB_RECT_HEIGHT_ZOOMED = 200;
  const float THUMB_RECT_WIDTH_ZOOMED  = 200;

  const float PIXEL_INFO_RECT_WIDTH  = 150;
  const float PIXEL_INFO_RECT_HEIGHT = 40;

  const float RULER_LENGTH = 100;

  // IMAGE ///////////////////////////////////////////////////
  bool mPlaceholderImageSet = true;
  std::filesystem::path mLastPath;
  joda::image::reader::ImageReader::Plane mLastPlane{-1, -1, -1};
  joda::ome::OmeInfo mOmeInfo;
  joda::ctrl::Preview mPreviewImages;
  joda::image::Image *mImageToShow = nullptr;
  float mOpaque                    = 0.9;
  joda::image::reader::ImageReader::Plane mPlane;
  joda::ome::TileToLoad mTile;
  enums::ZProjection mZprojection;
  int32_t mSeries = 0;

  // WIDGET ///////////////////////////////////////////////////
  QGraphicsPixmapItem *mActPixmap = nullptr;
  QGraphicsScene *scene           = nullptr;

  // MOVE IMAGE ///////////////////////////////////////////////////
  bool isDragging = false;
  QPoint lastPos;
  cv::Size mPixmapSize;

  // IMAGE INFO ///////////////////////////////////////////////////
  CrossCursorInfo mCrossCursorInfo;
  QRect mLastCrossHairCursorPos = {0, 0, 0, 0};
  joda::settings::ProjectImageSetup::PhysicalSizeSettings mDefaultPhysicalSize;

  /////////////////////////////////////////////////////
  // ThumbParameter mThumbnailParameter;
  PixelInfo mPixelInfo;
  uint32_t mThumbRectWidth      = 0;
  uint32_t mThumbRectHeight     = 0;
  int32_t mTileRectWidthScaled  = 0;
  int32_t mTileRectHeightScaled = 0;

  /////////////////////////////////////////////////////
  bool mThumbnailAreaEntered = false;

  // IMAGE CHANNEL SETTINGS ///////////////////////////////////////////////////
  void restoreChannelSettings();
  std::map<SettingsIdx, ChannelSettings> mChannelSettings;

  /////////////////////////////////////////////////////
  bool mWaiting             = false;
  bool mShowThumbnail       = true;
  bool mShowPixelInfo       = true;
  bool mShowCrosshandCursor = false;
  bool mLockCrosshandCursor = false;
  bool mShowOverlay         = true;
  bool mShowEditedImage     = false;
  bool mShowRuler           = true;

  mutable std::mutex mImageResetMutex;

private slots:
  void onUpdateImage();
};
}    // namespace joda::ui::gui
