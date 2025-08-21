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
#include "backend/settings/analze_settings.hpp"
#include "controller/controller.hpp"
#include "dialog_image_settings.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::gui {

class HistoToolbar;
class VideoControlButtonGroup;

///
/// \class      DialogImageViewer
/// \author     Joachim Danmayr
/// \brief
///
class DialogImageViewer : public QWidget
{
  Q_OBJECT

public:
  struct ImagePlaneSettings
  {
    joda::image::reader::ImageReader::Plane plane;
    int32_t series;
    int32_t tileWidth;
    int32_t tileHeight;
    int32_t tileX;
    int32_t tileY;
  };

  /////////////////////////////////////////////////////
  DialogImageViewer(QWidget *parent, joda::settings::AnalyzeSettings *settings, QToolBar *toolbarParent = nullptr);
  ~DialogImageViewer();

  // IMAGE CONTROL ///////////////////////////////
  void setWaiting(bool waiting);

  int32_t getTileSize() const;
  int32_t getSeries() const;
  auto getSelectedZProjection() const -> enums::ZProjection;
  int32_t getSelectedImageChannel() const;
  int32_t getSelectedTimeStack() const;
  bool getFillOverlay() const;
  auto getImagePanel() -> PanelImageView *;

  // SETTER ///////////////////////////////////////////
  void setImagePlane(const ImagePlaneSettings &);
  void setImageChannel(int32_t channel);
  void setShowCrossHairCursor(bool show);
  void fromSettings(const joda::settings::AnalyzeSettings &settings);

  // Visibility ///////////////////////////////////////////
  void setOverlayButtonsVisible(bool);
  void removeVideoControl();

signals:
  /////////////////////////////////////////////////////
  void settingChanged();

private:
  /////////////////////////////////////////////////////
  void applySettingsToImagePanel();

  // LAYOUT //////////////////////////////////////////
  QBoxLayout *mCentralLayout;
  QVBoxLayout *mMainLayout;

  // IMAGE PANELS ///////////////////////////////////////////////////
  PanelImageView mImageViewRight;

  // ACTIONS //////////////////////////////////////////////////
  QAction *mFillOVerlay         = nullptr;
  QAction *showCrossHairCursor  = nullptr;
  QAction *showPixelInfo        = nullptr;
  QAction *showOverlay          = nullptr;
  QSlider *mOverlayOpaque       = nullptr;
  QAction *mOverlayOpaqueAction = nullptr;

  QActionGroup *mImageChannelMenuGroup = nullptr;
  QAction *mImageChannel               = nullptr;
  std::map<int32_t, QAction *> mChannelSelections;

  // T-STACK //////////////////////////////////////////////////
  VideoControlButtonGroup *mVideoButtonGroup;

  // Z-STACK //////////////////////////////////////////////////
  int32_t mSelectedZStack = 0;
  int32_t mSelectedTStack = 0;

  // IMAGE SETTINGS //////////////////////////////////////////////////
  DialogImageSettings::Settings mImageSettings;

  // ANALYZE SETTINGS ///////////////////////////////////
  joda::settings::AnalyzeSettings *mSettings = nullptr;

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
