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
  void setImage(const joda::image::Image &originalImage, const joda::image::Image &previewImage, int width, int height,
                const QString &info)
  {
    mPreviewLabel.setImage(previewImage);
    mPreviewInfo->setText(info);
    mImageViewer.setImage(originalImage, previewImage);
  }
  void resetImage(const QString &info)
  {
    mPreviewLabel.resetImage();
    mPreviewInfo->setText(info);
  }

private slots:
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onOpenFullScreenClickec();

private:
  /////////////////////////////////////////////////////
  QWidget *createToolBar();
  QLabel *mPreviewInfo;

  /////////////////////////////////////////////////////
  PanelImageView mPreviewLabel;
  DialogImageViewer mImageViewer;
};
}    // namespace joda::ui::qt
