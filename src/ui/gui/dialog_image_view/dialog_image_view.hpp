///
/// \file      dialog_image_view.hpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qaction.h>
#include <qdialog.h>
#include <qwindow.h>
#include "backend/helper/image/image.hpp"
#include "controller/controller.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::gui {

class HistoToolbar;

///
/// \class      DialogImageViewer
/// \author     Joachim Danmayr
/// \brief
///
class DialogImageViewer : public QDockWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogImageViewer(QWidget *parent, bool showOriginalImage = true);
  ~DialogImageViewer();
  void imageUpdated(const ctrl::Preview::PreviewResults &info, const std::map<enums::ClassIdIn, QString> &classes);
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
    mPreviewImages.thumbnail.clear();
    mPreviewImages.originalImage.clear();
    mPreviewImages.editedImage.clear();
    mPreviewImages.overlay.clear();
    mPreviewImages.results.foundObjects.clear();
    mImageViewLeft.resetImage();
    mImageViewRight.resetImage();
  }
  void setWaiting(bool waiting)
  {
    mImageViewLeft.setWaiting(waiting);
    mImageViewRight.setWaiting(waiting);
  }
  int32_t getPreviewSize() const
  {
    if(mPreviewSizeGroup != nullptr) {
      auto *checked     = mPreviewSizeGroup->checkedAction();
      QStringList parts = checked->text().split('x');
      int width         = parts.value(0).toInt();
      return width;
    }
    return 2048;
  }

  auto getSelectedClassesAndClasses() const -> settings::ObjectInputClasses
  {
    return mImageViewRight.getSelectedClasses();
  }

  enum class ImageView
  {
    LEFT  = 0,
    RIGHT = 1,
    BOTH  = 2
  };
  void triggerPreviewUpdate(ImageView view, bool withUserHistoSettings);

  bool fillOverlay() const
  {
    return mFillOVerlay->isChecked();
  }
  void closeEvent(QCloseEvent *event) override;

  void setPipelineResultsButtonVisible(bool show)
  {
    showPipelineResults->setChecked(show);
    showPipelineResults->setVisible(show);
    mImageViewRight.setShowPipelineResults(show);
  }

  void setPreviewImageSizeVisble(bool show)
  {
    previewSize->setVisible(show);
  }

  void setCrossHairCursorPositionAndCenter(int32_t x, int32_t y);

signals:
  void tileClicked(int32_t tileX, int32_t tileY);
  void onSettingChanged();

private:
  /////////////////////////////////////////////////////
  void leaveEvent(QEvent *event) override;

  /////////////////////////////////////////////////////
  joda::ctrl::Preview mPreviewImages;
  PanelImageView mImageViewLeft;
  PanelImageView mImageViewRight;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter = 0;
  QBoxLayout *mCentralLayout;

  HistoToolbar *mHistoToolbarLeft  = nullptr;
  HistoToolbar *mHistoToolbarRight = nullptr;

  // ACTIONS //////////////////////////////////////////////////
  QAction *mFillOVerlay           = nullptr;
  QActionGroup *mPreviewSizeGroup = nullptr;
  QAction *showPipelineResults    = nullptr;
  QAction *previewSize            = nullptr;
  QAction *showCrossHairCursor    = nullptr;

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
  void onShowPipelineResults(bool checked);
  void onShowThumbnailChanged(bool checked);
  void onShowCrossHandCursor(bool checked);
  void onTileClicked(int32_t tileX, int32_t tileY);
};

}    // namespace joda::ui::gui
