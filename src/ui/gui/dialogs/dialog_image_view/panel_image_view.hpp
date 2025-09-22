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

#include <qcolor.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_units.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/image/image.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "controller/controller.hpp"
#include "ui/gui/dialogs/dialog_image_view/graphics_polygon.hpp"
#include "ui/gui/dialogs/dialog_image_view/painted_roi_properties.hpp"
#include <opencv2/core/types.hpp>

namespace joda::ui::gui {

using PaintedRoi_t = std::vector<QPoint>;

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
    PAINT_RECTANGLE,
    PAINT_OVAL,
    PAINT_POLYGON,
    PAIN_BRUSH,
    PAINT_MAGIC_WAND
  };

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
  void setEditedImage(const joda::image::Image &&edited);
  void reloadImage();
  void repaintImage();
  void repaintViewport();
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);
  void setWaiting(bool waiting);
  void setShowThumbnail(bool);
  void setShowPixelInfo(bool);
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

  // SET STATE ///////////////////////////////////////////////////
  void setState(State);
  void setClassIdToUseForDrawing(enums::ClassId classId, const QColor &color);

  // INFORMATION NEEDED FROM EXTERNAL ///////////////////////////////////////////////////
  void setZprojection(enums::ZProjection);
  void setSeries(int32_t);
  void setImagePlane(const joda::image::reader::ImageReader::Plane &);
  void setSelectedTile(int32_t tileX, int32_t tileY);
  void setImageTile(int32_t tileWith, int32_t tileHeight);
  void setDefaultPhysicalSize(const joda::settings::ProjectImageSetup::PhysicalSizeSettings &);
  auto mutableImage() -> joda::image::Image *;

  // REGION OF INTERESTS //////////////////////////////////////////////
  auto getObjectMapFromAnnotatedRegions(const std::set<PaintedRoiProperties::SourceType> &filter, atom::ObjectListWithNone &,
                                        enums::ClassId classFilter = enums::ClassId::UNDEFINED) -> void;
  void setRegionsOfInterestFromObjectList(const atom::ObjectMap &, const joda::settings::Classification &);
  void clearRegionOfInterest(PaintedRoiProperties::SourceType sourceToDelete = PaintedRoiProperties::SourceType::FromPipeline);
  auto getPtrToPolygons() -> std::map<QGraphicsItem *, PaintedRoiProperties> *;
  void setSelectedRois(const std::set<QGraphicsItem *> &idxs);
  void deleteRois(const std::set<QGraphicsItem *> &idx);
  void deleteSelectedRois();
  void setFillRois(bool);
  void setShowRois(bool);
  void setRoisOpaque(float opaque);
  void setRoisToHide(const std::set<enums::ClassId> &);
  void setRoiColorsForClasses(const joda::settings::Classification &);

signals:
  /////////////////////////////////////////////////////
  void classesToShowChanged(const settings::ObjectInputClasses &selectedClasses);
  void updateImage();
  void onImageRepainted();
  void tileClicked(int32_t tileX, int32_t tileY);
  void paintedPolygonsChanged();
  void paintedPolygonClicked(QList<QGraphicsItem *>);

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
  void setCursor();

private:
  /////////////////////////////////////////////////////
  const int32_t MAX_POLYGONS_TO_DRAW = 30000;

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
  float mOpaque                    = 0.6F;
  joda::image::reader::ImageReader::Plane mPlane;
  joda::ome::TileToLoad mTile;
  enums::ZProjection mZprojection;
  int32_t mSeries = 0;

  // WIDGET ///////////////////////////////////////////////////
  QGraphicsPixmapItem *mActPixmap = nullptr;
  QGraphicsScene *scene           = nullptr;

  // STATE AND PAINTING ///////////////////////////////////////////////////
  State mState = State::MOVE;
  std::map<QGraphicsItem *, PaintedRoiProperties> mPolygonItems;
  PaintedRoi_t mActPaintingRoi;
  std::set<enums::ClassId> mRoiClassesToHide;

  QPointF mPaintOrigin;
  QAbstractGraphicsShapeItem *mRubberItem = nullptr;
  bool mDrawPolygon                       = false;
  std::vector<QPointF> mPolygonPoints;
  QGraphicsLineItem *mTempPolygonLine;
  MyPolygonItem *mTempPolygonItem;
  enums::ClassId mSelectedClassForDrawing = enums::ClassId::NONE;
  QColor mPixelClassColor                 = Qt::gray;

  // MOVE IMAGE ///////////////////////////////////////////////////
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
  bool mWaiting               = false;
  bool mShowThumbnail         = true;
  bool mShowPixelInfo         = true;
  bool mShowCrosshandCursor   = false;
  bool mLockCrosshandCursor   = false;
  bool mShowEditedImage       = false;
  bool mShowRuler             = true;
  bool mHideManualAnnotations = false;

  // ROI///////////////////////////////////////////////////
  bool mFillRoi  = false;
  bool mShowRois = true;

  mutable std::mutex mImageResetMutex;

private slots:
  void onUpdateImage();
};
}    // namespace joda::ui::gui
