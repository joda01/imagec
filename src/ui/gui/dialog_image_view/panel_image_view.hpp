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
#include <iostream>
#include <mutex>
#include <string>
#include "backend/enums/enums_classes.hpp"
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

  /////////////////////////////////////////////////////
  PanelImageView(const joda::image::Image *imageReference, const joda::image::Image *thumbnailImageReference, const joda::image::Image *overlay,
                 bool withThumbnail, QWidget *parent = nullptr);

  void imageUpdated(const ctrl::Preview::PreviewResults &info, const std::map<enums::ClassIdIn, QString> &classes);
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);
  const joda::image::Image &getImage()
  {
    return *mActPixmapOriginal;
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
  void setEnableThumbnail(bool);
  void setShowHistogram(bool);
  void setShowPixelInfo(bool);
  void setShowPipelineResults(bool);
  void setShowOverlay(bool);
  void setShowCrosshandCursor(bool);
  void setLockCrosshandCursor(bool);
  void setThumbnailPosition(const ThumbParameter &);
  void setCursorPosition(const QPoint &pos);
  void setCursorPositionFromOriginalImageCoordinatesAndCenter(const QRect &boundingRect);
  auto getCursorPosition() -> QPoint;
  void setImageReference(const joda::image::Image *imageReference);
  auto getSelectedClasses() const -> settings::ObjectInputClasses
  {
    return mSelectedClasses;
  }

signals:
  void classesToShowChanged(const settings::ObjectInputClasses &selectedClasses);
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
  void drawHistogram(QPainter &);
  void drawThumbnail(QPainter &);
  void drawPipelineResult(QPainter &);
  void drawCrossHairCursor(QPainter &);
  void drawPixelInfo(QPainter &, int32_t startX, int32_t startY, const PixelInfo &info);

  void getClickedTileInThumbnail(QMouseEvent *event);
  void getThumbnailAreaEntered(QMouseEvent *event);
  auto fetchPixelInfoFromMousePosition(const QPoint &pos) const -> PixelInfo;
  bool getPreviewResultsAreaEntered(QMouseEvent *event);
  bool getPreviewResultsAreaClicked(QMouseEvent *event);

  void updatePipelineResultsCoordinates();
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

  const float RESULTS_INFO_RECT_WIDTH = 150;
  // const float RESULTS_INFO_RECT_HEIGHT = 250;

  /////////////////////////////////////////////////////
  bool mPlaceholderImageSet                          = true;
  const joda::image::Image *mActPixmapOriginal       = nullptr;
  const joda::image::Image *mOverlayImage            = nullptr;
  const joda::image::Image *mThumbnailImageReference = nullptr;
  QGraphicsPixmapItem *mActPixmap                    = nullptr;
  QGraphicsScene *scene;
  bool isDragging = false;
  QPoint lastPos;
  State mState = State::MOVE;
  cv::Size mPixmapSize;
  ctrl::Preview::PreviewResults mPipelineResult;
  QString mPipelineResultsHtmlText;
  settings::ObjectInputClasses mSelectedClasses;
  std::vector<std::tuple<QRect, joda::enums::ClassIdIn>> mClassesCoordinates;
  std::map<enums::ClassIdIn, QString> mClasses;

  /////////////////////////////////////////////////////
  CrossCursorInfo mCrossCursorInfo;
  QRect mLastCrossHairCursorPos = {0, 0, 0, 0};

  /////////////////////////////////////////////////////
  ThumbParameter mThumbnailParameter;
  PixelInfo mPixelInfo;

  uint32_t mThumbRectWidth      = 0;
  uint32_t mThumbRectHeight     = 0;
  int32_t mTileRectWidthScaled  = 0;
  int32_t mTileRectHeightScaled = 0;

  /////////////////////////////////////////////////////
  bool mThumbnailAreaEntered = false;

  /////////////////////////////////////////////////////
  bool mWithThumbnail;
  bool mWaiting             = false;
  bool mShowThumbnail       = true;
  bool mShowPixelInfo       = true;
  bool mShowCrosshandCursor = false;
  bool mLockCrosshandCursor = false;
  bool mShowHistogram       = true;
  bool mShowOverlay         = true;
  bool mShowPipelineResults = false;

  mutable std::mutex mImageResetMutex;

private slots:
  void onUpdateImage();
};
}    // namespace joda::ui::gui
