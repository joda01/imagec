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
/// \brief     A short description what happens here.
///

#pragma once

#include <qdialog.h>
#include <qwindow.h>
#include "backend/image_processing/image/image.hpp"
#include "controller/controller.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::qt {

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
  void createHistogramDialog();
  joda::ctrl::Controller::Preview &getPreviewObject()
  {
    return mPreviewImages;
  }
  void setThumbnailPosition(uint32_t nrOfTilesX, uint32_t nrOfTilesY, uint32_t x, uint32_t y)
  {
    mImageViewLeft->setThumbnailPosition(nrOfTilesX, nrOfTilesY, x, y);
    mImageViewRight->setThumbnailPosition(nrOfTilesX, nrOfTilesY, x, y);
  }

private:
  /////////////////////////////////////////////////////
  static constexpr float HISTOGRAM_ZOOM_STEP = 25;

  /////////////////////////////////////////////////////
  QSlider *mSlider;
  QScrollBar *mSliderScaling;
  QScrollBar *mSliderHistogramOffset;

  joda::ctrl::Controller::Preview mPreviewImages;
  PanelImageView *mImageViewLeft;
  PanelImageView *mImageViewRight;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter = 0;

  /////////////////////////////////////////////////////
  QDialog *mHistogramDialog;

private slots:
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onLeftViewChanged();
  void onRightViewChanged();
  void onSliderMoved(int position);
  void onShowHistogramDialog();
  void onSetSateToMove();
  void onSetStateToPaintRect();
  void onFitHistogramToScreenSizeClicked();
  void onZoomHistogramOutClicked();
  void onZoomHistogramInClicked();
};

}    // namespace joda::ui::qt
