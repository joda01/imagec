///
/// \file      dialog_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_image_view.hpp"
#include <qaction.h>
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qdockwidget.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <thread>
#include "backend/helper/image/image.hpp"
#include "ui/gui/dialog_image_view/dialog_channel_settings.hpp"
#include "ui/gui/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::DialogImageViewer(QWidget *parent, QToolBar *toolbarParent) : QWidget(parent), mImageViewRight(parent)
{
  setWindowTitle("Preview");
  setVisible(false);
  setContentsMargins(0, 0, 0, 0);

  mMainLayout = new QVBoxLayout();
  mMainLayout->setContentsMargins(0, 0, 0, 0);
  {
    QToolBar *toolbarTop;
    if(toolbarParent == nullptr) {
      toolbarTop = new QToolBar();
    } else {
      toolbarTop = toolbarParent;
    }

    auto *pinToTop = new QAction(generateSvgIcon("window-pin"), "");
    pinToTop->setToolTip("Pin to stay on top");
    pinToTop->setCheckable(true);
    pinToTop->setChecked(false);
    connect(pinToTop, &QAction::triggered, [this](bool checked) {
      if(checked) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
      } else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
      }
    });

    auto *buttonGroup = new QActionGroup(toolbarTop);

    auto *action2 = new QAction({}, "");
    action2->setCheckable(true);
    action2->setChecked(true);
    buttonGroup->addAction(action2);

    auto *paintRectangle = new QAction(generateSvgIcon("draw-rectangle"), "");
    paintRectangle->setCheckable(true);
    buttonGroup->addAction(paintRectangle);

    auto *showThumbnail = new QAction(generateSvgIcon("virtual-desktops"), "");
    showThumbnail->setStatusTip("Show/Hide image thumbnail");
    showThumbnail->setCheckable(true);
    showThumbnail->setChecked(true);
    connect(showThumbnail, &QAction::triggered, this, &DialogImageViewer::onShowThumbnailChanged);
    toolbarTop->addAction(showThumbnail);

    showPixelInfo = new QAction(generateSvgIcon("coordinate"), "");
    showPixelInfo->setStatusTip("Show/Hide pixel information");
    showPixelInfo->setCheckable(true);
    showPixelInfo->setChecked(true);
    connect(showPixelInfo, &QAction::triggered, this, &DialogImageViewer::onShowPixelInfo);
    toolbarTop->addAction(showPixelInfo);

    showCrossHairCursor = new QAction(generateSvgIcon("crosshairs"), "");
    showCrossHairCursor->setStatusTip("Show/Hide cross hair cursor (right click to place)");
    showCrossHairCursor->setCheckable(true);
    showCrossHairCursor->setChecked(false);
    connect(showCrossHairCursor, &QAction::triggered, this, &DialogImageViewer::onShowCrossHandCursor);
    toolbarTop->addAction(showCrossHairCursor);

    toolbarTop->addSeparator();

    showOverlay = new QAction(generateSvgIcon("redeyes"), "");
    showOverlay->setStatusTip("Show/Hide results as overlay");
    showOverlay->setCheckable(true);
    showOverlay->setChecked(true);
    connect(showOverlay, &QAction::triggered, [this](bool selected) { mImageViewRight.setShowOverlay(selected); });
    toolbarTop->addAction(showOverlay);

    mFillOVerlay = new QAction(generateSvgIcon("fill-color"), "");
    mFillOVerlay->setStatusTip("Fill/Outline results overlay");
    mFillOVerlay->setCheckable(true);
    connect(mFillOVerlay, &QAction::triggered, this, &DialogImageViewer::onSettingsChanged);
    toolbarTop->addAction(mFillOVerlay);

    mOverlayOpaque = new QSlider();
    mOverlayOpaque->setOrientation(Qt::Orientation::Horizontal);
    mOverlayOpaque->setMinimum(0);
    mOverlayOpaque->setMaximum(100);
    mOverlayOpaque->setValue(50);
    mOverlayOpaque->setMaximumWidth(100);
    connect(mOverlayOpaque, &QSlider::valueChanged,
            [this] { mImageViewRight.setOverlayOpaque(static_cast<float>(mOverlayOpaque->value()) / 100.0F); });
    toolbarTop->addWidget(mOverlayOpaque);

    toolbarTop->addSeparator();

    auto *fitToScreen = new QAction(generateSvgIcon("zoom-fit-best"), "");
    fitToScreen->setStatusTip("Fit image to screen");
    fitToScreen->setObjectName("ToolButton");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    auto *zoomIn = new QAction(generateSvgIcon("zoom-in"), "");
    zoomIn->setStatusTip("Zoom image in");
    zoomIn->setObjectName("ToolButton");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomInClicked);
    toolbarTop->addAction(zoomIn);

    auto *zoomOut = new QAction(generateSvgIcon("zoom-out"), "");
    zoomOut->setObjectName("ToolButton");
    zoomIn->setStatusTip("Zoom image out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomOutClicked);
    toolbarTop->addAction(zoomOut);

    auto *histogram = new QAction(generateSvgIcon("view-object-histogram-linear"), "");
    histogram->setObjectName("ToolButton");
    histogram->setStatusTip("Histogram");
    connect(histogram, &QAction::triggered, [this] {
      auto *dialog = new DialogChannelSettings(&mImageViewRight, this);
      dialog->show();
    });
    toolbarTop->addAction(histogram);

    toolbarTop->addSeparator();

    //
    // Preview size
    //
    {
      auto *tileSizeMenu = new QMenu();
      mTileSizeGroup     = new QActionGroup(toolbarTop);
      auto addTileSize   = [this, &tileSizeMenu](int32_t tileWidth) {
        auto *action = tileSizeMenu->addAction(QString::number(tileWidth) + "x" + QString::number(tileWidth));
        action->setCheckable(true);
        mTileSizeGroup->addAction(action);
        mTileSizes.emplace(tileWidth, action);
        return action;
      };
      addTileSize(8192);
      addTileSize(4096);
      auto *action = addTileSize(2048);
      action->setChecked(true);
      addTileSize(1024);
      addTileSize(512);

      mTileSize = new QAction(generateSvgIcon("computer"), "");
      mTileSize->setStatusTip("Tile size");
      mTileSize->setMenu(tileSizeMenu);
      toolbarTop->addAction(mTileSize);
      auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(mTileSize));
      btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
      connect(mTileSizeGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingsChanged);
    }

    //
    // Image channel
    //
    {
      auto *channelMenu      = new QMenu();
      mImageChannelMenuGroup = new QActionGroup(toolbarTop);

      auto addChannel = [this, &channelMenu](int32_t chNr) {
        auto *action = channelMenu->addAction("CH" + QString::number(chNr));
        action->setCheckable(true);
        if(chNr == 0) {
          action->setChecked(true);
        }
        mImageChannelMenuGroup->addAction(action);
        mChannelSelections.emplace(chNr, action);
      };
      for(int n = 0; n < 9; n++) {
        addChannel(n);
      }

      auto *imageChannel = new QAction(generateSvgIcon("irc-operator"), "");
      imageChannel->setStatusTip("Image channel to show");
      imageChannel->setMenu(channelMenu);
      toolbarTop->addAction(imageChannel);
      auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(imageChannel));
      btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
      connect(mImageChannelMenuGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingsChanged);
    }

    //
    // z-Projection
    //
    {
      auto *zProjectionMenu    = new QMenu();
      mZProjectionGroup        = new QActionGroup(toolbarTop);
      mSingleChannelProjection = zProjectionMenu->addAction("Single channel");
      mZProjectionGroup->addAction(mSingleChannelProjection);
      mSingleChannelProjection->setCheckable(true);
      mMaxIntensityProjection = zProjectionMenu->addAction("Max. intensity");
      mZProjectionGroup->addAction(mMaxIntensityProjection);
      mMaxIntensityProjection->setCheckable(true);
      mMaxIntensityProjection->setChecked(true);
      mMinIntensityProjection = zProjectionMenu->addAction("Min. intensity");
      mZProjectionGroup->addAction(mMinIntensityProjection);
      mMinIntensityProjection->setCheckable(true);
      mAvgIntensity = zProjectionMenu->addAction("Avg. intensity");
      mZProjectionGroup->addAction(mAvgIntensity);
      mAvgIntensity->setCheckable(true);
      mTakeTheMiddleProjection = zProjectionMenu->addAction("Take the middle");
      mZProjectionGroup->addAction(mTakeTheMiddleProjection);
      mTakeTheMiddleProjection->setCheckable(true);
      mZProjectionAction = new QAction(generateSvgIcon("layer-visible-on"), "");
      mZProjectionAction->setStatusTip("z-projection options");
      mZProjectionAction->setMenu(zProjectionMenu);
      toolbarTop->addAction(mZProjectionAction);
      auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(mZProjectionAction));
      btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
      connect(mZProjectionGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingsChanged);
    }

    if(toolbarParent == nullptr) {
      mMainLayout->addWidget(toolbarTop);
    }
  }

  // Central images
  {
    mCentralLayout      = new QBoxLayout(QBoxLayout::TopToBottom);
    auto *centralWidget = new QWidget();
    mCentralLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    auto *rightVerticalLayout = new QVBoxLayout();
    mImageViewRight.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightVerticalLayout->addWidget(&mImageViewRight);
    mCentralLayout->addLayout(rightVerticalLayout);
    mImageViewRight.resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(mCentralLayout);
    mMainLayout->addWidget(centralWidget);

    connect(&mImageViewRight, &PanelImageView::tileClicked, this, &DialogImageViewer::onSettingsChanged);
    connect(&mImageViewRight, &PanelImageView::classesToShowChanged, this, &DialogImageViewer::onSettingsChanged);
  }

  // Bottom toolbar
  {
    mSpinnerActTimeStack = new QSpinBox();
    mSpinnerActTimeStack->setValue(0);
    connect(mSpinnerActTimeStack, &QSpinBox::valueChanged, [this] { onSettingsChanged(); });

    // Create spacer widgets
    auto *leftSpacer = new QWidget;
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *rightSpacer = new QWidget;
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    mPlaybackToolbar = new QToolBar();
    mPlaybackToolbar->setVisible(false);
    mPlaybackToolbar->addWidget(leftSpacer);
    auto *skipBackward = new QAction(generateSvgIcon("media-skip-backward"), "");
    connect(skipBackward, &QAction::triggered, [this] {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(0);
      mSpinnerActTimeStack->blockSignals(false);
      emit settingChanged();
    });
    mPlaybackToolbar->addAction(skipBackward);

    auto *seekBackward = new QAction(generateSvgIcon("media-seek-backward"), "");
    connect(seekBackward, &QAction::triggered, [this] {
      if(mSpinnerActTimeStack->value() > 0) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() - 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      emit settingChanged();
    });
    mPlaybackToolbar->addAction(seekBackward);

    // ==========================================================

    mPlaybackSpeedSelector     = new QMenu();
    mPlaybackspeedGroup        = new QActionGroup(mPlaybackSpeedSelector);
    auto addPlaybackSpeedLamda = [this](QAction *action, int32_t timeMs, bool checked = false) {
      mPlaybackspeedGroup->addAction(action);
      action->setCheckable(true);
      action->setChecked(checked);
      connect(action, &QAction::triggered, [timeMs, this]() {
        mPlaybackSpeed = timeMs;
        if(mActionPlay->isChecked()) {
          mPlayTimer->stop();
          mPlayTimer->start(mPlaybackSpeed);
        }
      });
    };

    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("25 Hz"), 40);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("20 Hz"), 50);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("10 Hz"), 100);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("5 Hz"), 200);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("4 Hz"), 250);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("3 Hz"), 333);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("2 Hz"), 500);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("1 Hz"), 1000, true);
    addPlaybackSpeedLamda(mPlaybackSpeedSelector->addAction("0.5 Hz"), 2000);

    mActionPlay = new QAction(generateSvgIcon("media-playback-start"), "");
    mActionPlay->setMenu(mPlaybackSpeedSelector);
    mActionPlay->setCheckable(true);
    connect(mActionPlay, &QAction::triggered, [this](bool selected) {
      if(selected) {
        mPlayTimer->start(mPlaybackSpeed);
      } else {
        mPlayTimer->stop();
      }
    });

    mPlaybackToolbar->addAction(mActionPlay);
    mPlaybackToolbar->addWidget(mSpinnerActTimeStack);

    mActionStop = new QAction(generateSvgIcon("media-playback-stop"), "");
    connect(mActionStop, &QAction::triggered, [this] {
      mActionPlay->setChecked(false);
      mPlayTimer->stop();
    });
    mPlaybackToolbar->addAction(mActionStop);

    auto *seekForward = new QAction(generateSvgIcon("media-seek-forward"), "");
    connect(seekForward, &QAction::triggered, [this] {
      if(mSpinnerActTimeStack->value() < mImageViewRight.getNrOfTstacks()) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      emit settingChanged();
    });
    mPlaybackToolbar->addAction(seekForward);
    mPlaybackToolbar->addWidget(rightSpacer);
    mMainLayout->addWidget(mPlaybackToolbar);
  }

  // setLayout(layout);
  setLayout(mMainLayout);

  // Video timer
  mPlayTimer = new QTimer();
  QObject::connect(mPlayTimer, &QTimer::timeout, [&] {
    if(mSpinnerActTimeStack->value() < mImageViewRight.getNrOfTstacks()) {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
      mSpinnerActTimeStack->blockSignals(false);
    } else {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(0);
      mSpinnerActTimeStack->blockSignals(false);
    }
    emit settingChanged();
  });

  onSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::~DialogImageViewer()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setImagePlane(const ImagePlaneSettings &settings)
{
  mSpinnerActTimeStack->blockSignals(true);
  mImageChannelMenuGroup->blockSignals(true);

  mSelectedImageSeries = settings.series;
  mSelectedZStack      = settings.plane.z;
  mSpinnerActTimeStack->setValue(settings.plane.t);
  for(const auto &[chNr, action] : mChannelSelections) {
    if(chNr == settings.plane.c) {
      action->setChecked(true);
    } else {
      action->setChecked(false);
    }
  }

  mImageViewRight.setSelectedTile(settings.tileX, settings.tileY);
  for(const auto &[size, action] : mTileSizes) {
    if(size == settings.tileWidth) {
      action->setChecked(true);
    } else {
      action->setChecked(false);
    }
  }

  applySettingsToImagePanel();
  mSpinnerActTimeStack->blockSignals(false);
  mImageChannelMenuGroup->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setShowCrossHairCursor(bool show)
{
  showCrossHairCursor->setChecked(true);
  mImageViewRight.setShowCrosshandCursor(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setSettingsPointer(joda::settings::AnalyzeSettings *settings)
{
  mSettings = settings;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogImageViewer::getImagePanel() -> PanelImageView *
{
  return &mImageViewRight;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::applySettingsToImagePanel()
{
  auto tileSize = getTileSize();
  mImageViewRight.setZprojection(getSelectedZProjection());
  mImageViewRight.setImagePlane({.z = mSelectedZStack, .c = getSelectedImageChannel(), .t = mSpinnerActTimeStack->value()});
  mImageViewRight.setImageTile(tileSize, tileSize);
  mImageViewRight.setSeries(mSelectedImageSeries);

  // Sync to settings
  if(mSettings != nullptr) {
    auto tileSize                                      = getTileSize();
    mSettings->imageSetup.imageTileSettings.tileHeight = tileSize;
    mSettings->imageSetup.imageTileSettings.tileWidth  = tileSize;
    mSettings->imageSetup.series                       = mSelectedImageSeries;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onSettingsChanged()
{
  applySettingsToImagePanel();
  mImageViewRight.reloadImage();
  emit settingChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onZoomInClicked()
{
  mImageViewRight.zoomImage(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onZoomOutClicked()
{
  mImageViewRight.zoomImage(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onFitImageToScreenSizeClicked()
{
  mImageViewRight.fitImageToScreenSize();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowThumbnailChanged(bool checked)
{
  mImageViewRight.setShowThumbnail(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowPixelInfo(bool checked)
{
  mImageViewRight.setShowPixelInfo(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowCrossHandCursor(bool checked)
{
  mImageViewRight.setShowCrosshandCursor(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogImageViewer::getSelectedZProjection() const -> enums::ZProjection
{
  if(mZProjectionGroup != nullptr) {
    auto *checked = mZProjectionGroup->checkedAction();
    if(checked == mSingleChannelProjection) {
      return enums::ZProjection::NONE;
    }
    if(checked == mMaxIntensityProjection) {
      return enums::ZProjection::MAX_INTENSITY;
    }
    if(checked == mMinIntensityProjection) {
      return enums::ZProjection::MIN_INTENSITY;
    }
    if(checked == mAvgIntensity) {
      return enums::ZProjection::AVG_INTENSITY;
    }
    if(checked == mTakeTheMiddleProjection) {
      return enums::ZProjection::TAKE_MIDDLE;
    }
  }
  return enums::ZProjection::MAX_INTENSITY;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getTileSize() const
{
  if(mTileSizeGroup != nullptr) {
    for(const auto &[size, action] : mTileSizes) {
      if(action->isChecked()) {
        return size;
      }
    }
  }
  return 2048;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSeries() const
{
  return mSelectedImageSeries;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSelectedImageChannel() const
{
  if(mImageChannelMenuGroup != nullptr) {
    for(const auto &[chNr, action] : mChannelSelections) {
      if(action->isChecked()) {
        return chNr;
      }
    }
  }
  return 0;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSelectedTimeStack() const
{
  return mSpinnerActTimeStack->value();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setWaiting(bool waiting)
{
  // Don't show waiting dialog if video is running for a better view.
  if(mActionPlay->isChecked()) {
    return;
  }
  mImageViewRight.setWaiting(waiting);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool DialogImageViewer::getFillOverlay() const
{
  return mFillOVerlay->isChecked();
}

}    // namespace joda::ui::gui
