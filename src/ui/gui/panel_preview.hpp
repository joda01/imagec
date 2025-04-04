///
/// \file      panel_preview.hpp
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
#include <memory>
#include <string>
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/dialog_image_view/panel_image_view.hpp"

namespace joda::ui::gui {

class WindowMain;

class PanelPreview : public QWidget
{
  Q_OBJECT

public:
  PanelPreview(int width, int height, WindowMain *parent);
  void updateImage(const QString &info)
  {
    mImageViewer.imageUpdated();
    mPreviewLabel.imageUpdated();

    QMetaObject::invokeMethod(mPreviewInfo, "setText", Q_ARG(QString, info));
  }
  void hidePreviewImage()
  {
    mImageViewer.hide();
    mOpenFullScreenButton->setChecked(false);
  }
  void resetImage(const QString &info)
  {
    mImageViewer.resetImage();
    mPreviewLabel.resetImage();
    QMetaObject::invokeMethod(mPreviewInfo, "setText", Q_ARG(QString, info));
  }
  void setThumbnailPosition(const PanelImageView::ThumbParameter &param)
  {
    mPreviewLabel.setThumbnailPosition(param);
    mImageViewer.setThumbnailPosition(param);
  }
  joda::ctrl::Preview &getPreviewObject()
  {
    return mImageViewer.getPreviewObject();
  }

  void setWaiting(bool waiting)
  {
    mImageViewer.setWaiting(waiting);
    mPreviewLabel.setWaiting(waiting);
  }

  bool getFilledPreview() const
  {
    return mImageViewer.fillOverlay();
  }

  auto getSelectedClassesAndClasses() const -> settings::ObjectInputClasses
  {
    return mClassesClassesToShow->getValue();
  }

  int32_t getPreviewSize() const
  {
    return mPreviewSize->currentData().toInt();
  }

signals:
  void tileClicked(int32_t tileX, int32_t tileY);
  void onSettingChanged();

private slots:
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onOpenFullScreenClicked(bool checked);
  void onTileClicked(int32_t tileX, int32_t tileY);

private:
  /////////////////////////////////////////////////////
  WindowMain *mParent;
  QWidget *createToolBar();
  QLabel *mPreviewInfo;
  QComboBox *mPreviewSize;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> mClassesClassesToShow;

  // BUTTONS ///////////////////////////////////////////////////
  QPushButton *mOpenFullScreenButton;

  /////////////////////////////////////////////////////
  DialogImageViewer mImageViewer;
  PanelImageView mPreviewLabel;
};
}    // namespace joda::ui::gui
