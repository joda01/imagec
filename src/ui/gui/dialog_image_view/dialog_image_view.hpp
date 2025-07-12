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
#include <optional>
#include "backend/enums/types.hpp"
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
class DialogImageViewer : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogImageViewer(QWidget *parent, QToolBar *toolbarParent = nullptr);
  ~DialogImageViewer();

  // IMAGE CONTROL ///////////////////////////////
  void setWaiting(bool waiting);
  int32_t getPreviewSize() const;
  auto getSelectedZProjection() const -> enums::ZProjection;
  int32_t getSelectedImageChannel() const;
  int32_t getSelectedTimeStack() const;
  bool getFillOverlay() const;
  auto getImagePanel() -> PanelImageView *;

signals:
  /////////////////////////////////////////////////////
  void settingChanged();

private:
  // LAYOUT //////////////////////////////////////////
  QBoxLayout *mCentralLayout;
  QVBoxLayout *mMainLayout;

  // IMAGE PANELS ///////////////////////////////////////////////////
  PanelImageView mImageViewRight;

  // ACTIONS //////////////////////////////////////////////////
  QAction *mFillOVerlay                = nullptr;
  QActionGroup *mPreviewSizeGroup      = nullptr;
  QAction *previewSize                 = nullptr;
  QAction *showCrossHairCursor         = nullptr;
  QAction *showPixelInfo               = nullptr;
  QAction *showOverlay                 = nullptr;
  QAction *mZProjectionAction          = nullptr;
  QActionGroup *mZProjectionGroup      = nullptr;
  QActionGroup *mImageChannelMenuGroup = nullptr;
  QAction *mSingleChannelProjection    = nullptr;
  QAction *mMaxIntensityProjection     = nullptr;
  QAction *mMinIntensityProjection     = nullptr;
  QAction *mAvgIntensity               = nullptr;
  QAction *mTakeTheMiddleProjection    = nullptr;

  // T-STACK //////////////////////////////////////////////////
  std::optional<int32_t> mMaxTimeStacks = std::nullopt;
  QToolBar *mPlaybackToolbar;
  int32_t mPlaybackSpeed = 1000;
  QActionGroup *mPlaybackspeedGroup;
  QMenu *mPlaybackSpeedSelector;
  QTimer *mPlayTimer;
  QAction *mActionPlay;
  QAction *mActionStop;
  QSpinBox *mSpinnerActTimeStack;
  bool mPlaybackToolbarVisible = false;

private slots:
  /////////////////////////////////////////////////////
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();
  void onShowPixelInfo(bool checked);
  void onShowThumbnailChanged(bool checked);
  void onShowCrossHandCursor(bool checked);
  void onSettingsChanged();
};

}    // namespace joda::ui::gui
