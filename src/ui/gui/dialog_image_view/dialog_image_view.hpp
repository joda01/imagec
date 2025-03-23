///
/// \file      dialog_image_view.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qdialog.h>
#include <qwindow.h>
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::gui {

class HistoToolbar;

///
/// \class      DialogImageViewer
/// \author     Joachim Danmayr
/// \brief
///
class DialogImageViewer : public QMainWindow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogImageViewer(QWidget *parent);
  ~DialogImageViewer();
  void imageUpdated();
  void fitImageToScreenSize();
  joda::ctrl::Preview &getPreviewObject()
  {
    return mPreviewImages;
  }
  void setThumbnailPosition(const PanelImageView::ThumbParameter &param)
  {
    mImageViewLeft.setThumbnailPosition(param);
    mImageViewRight.setThumbnailPosition(param);
  }
  void resetImage()
  {
    mPreviewImages.editedImage.clear();
    mPreviewImages.overlay.clear();
    mPreviewImages.originalImage.clear();
    mPreviewImages.thumbnail.clear();
    //    mPreviewImages.detectionResult.reset();
    mImageViewLeft.resetImage();
    mImageViewRight.resetImage();
  }
  void setWaiting(bool waiting)
  {
    mImageViewLeft.setWaiting(waiting);
    mImageViewRight.setWaiting(waiting);
  }

  void hideEvent(QHideEvent *event) override
  {
    QMainWindow::hideEvent(event);
    emit hidden();
  }
  enum class ImageView
  {
    LEFT  = 0,
    RIGHT = 1
  };
  void triggerPreviewUpdate(ImageView view, bool withUserHistoSettings);

signals:
  void tileClicked(int32_t tileX, int32_t tileY);
  void hidden();

private:
  /////////////////////////////////////////////////////

  joda::ctrl::Preview mPreviewImages;
  PanelImageView mImageViewLeft;
  PanelImageView mImageViewRight;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter = 0;

  HistoToolbar *mHistoToolbarLeft  = nullptr;
  HistoToolbar *mHistoToolbarRight = nullptr;

private slots:
  /////////////////////////////////////////////////////
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onLeftViewChanged();
  void onRightViewChanged();
  void onSetSateToMove();
  void onSetStateToPaintRect();
  void onShowPixelInfo(bool checked);
  void onShowThumbnailChanged(bool checked);
  void onShowCrossHandCursor(bool checked);
  void onTileClicked(int32_t tileX, int32_t tileY);
};

}    // namespace joda::ui::gui
