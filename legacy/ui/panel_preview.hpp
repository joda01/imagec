///
/// \file      panel_preview.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <memory>
#include <string>
#include "backend/image_processing/image/image.hpp"
#include "controller/controller.hpp"
#include "ui/dialog_image_view/dialog_image_view.hpp"
#include "ui/dialog_image_view/panel_image_view.hpp"

namespace joda::ui::qt {

class PanelPreview : public QWidget
{
  Q_OBJECT

public:
  PanelPreview(int width, int height, QWidget *parent);
  void updateImage(const QString &info)
  {
    mImageViewer.imageUpdated();
    mPreviewLabel.imageUpdated();
    mPreviewInfo->setText(info);
  }
  void resetImage(const QString &info)
  {
    mImageViewer.resetImage();
    mPreviewLabel.resetImage();
    mPreviewInfo->setText(info);
  }
  void setThumbnailPosition(uint32_t nrOfTilesX, uint32_t nrOfTilesY, uint32_t x, uint32_t y)
  {
    mPreviewLabel.setThumbnailPosition(nrOfTilesX, nrOfTilesY, x, y);
    mImageViewer.setThumbnailPosition(nrOfTilesX, nrOfTilesY, x, y);
  }
  joda::ctrl::Controller::Preview &getPreviewObject()
  {
    return mImageViewer.getPreviewObject();
  }

  void setWaiting(bool waiting)
  {
    mImageViewer.setWaiting(waiting);
    mPreviewLabel.setWaiting(waiting);
  }

signals:
  void tileClicked(int32_t tileX, int32_t tileY);

private slots:
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onOpenFullScreenClickec();
  void onTileClicked(int32_t tileX, int32_t tileY);

private:
  /////////////////////////////////////////////////////
  QWidget *createToolBar();
  QLabel *mPreviewInfo;

  /////////////////////////////////////////////////////
  DialogImageViewer mImageViewer;
  PanelImageView mPreviewLabel;
};
}    // namespace joda::ui::qt