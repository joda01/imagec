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
#include <qwidget.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <thread>
#include "backend/helper/image/image.hpp"
#include "ui/gui/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "histo_toolbar.hpp"

namespace joda::ui::gui {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::DialogImageViewer(QWidget *parent, bool showOriginalImage, QMainWindow *toolbarParent) :
    QDockWidget(parent), mImageViewLeft(&mPreviewImages.originalImage, &mPreviewImages.thumbnail, nullptr, true),
    mImageViewRight(&mPreviewImages.editedImage, &mPreviewImages.thumbnail, &mPreviewImages.overlay, false), mWindowMain(toolbarParent)
{
  setWindowTitle("Preview");
  setVisible(false);
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  // Set initial size constraints
  setMaximumWidth(500);    // Max width when docked
  setMinimumWidth(500);    // Min width even when docked

  mImageViewRight.setShowPipelineResults(true);

  auto *mainContainer = new QWidget();
  mMainLayout         = new QVBoxLayout();

  {
    auto *toolbarTop = new QToolBar();

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
    // toolbarTop->addAction(pinToTop);

    // toolbarTop->addSeparator();

    QActionGroup *buttonGroup = new QActionGroup(toolbarTop);

    QAction *action2 = new QAction({}, "");
    action2->setCheckable(true);
    action2->setChecked(true);
    connect(action2, &QAction::triggered, this, &DialogImageViewer::onSetSateToMove);
    buttonGroup->addAction(action2);
    // toolbarTop->addAction(action2);

    QAction *paintRectangle = new QAction(generateSvgIcon("draw-rectangle"), "");
    paintRectangle->setCheckable(true);
    connect(paintRectangle, &QAction::triggered, this, &DialogImageViewer::onSetStateToPaintRect);
    buttonGroup->addAction(paintRectangle);
    // toolbarTop->addAction(paintRectangle);

    // toolbarTop->addSeparator();

    QAction *showThumbnail = new QAction(generateSvgIcon("virtual-desktops"), "");
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

    showPipelineResults = new QAction(generateSvgIcon("sort-presence"), "");
    showPipelineResults->setStatusTip("Show/Hide pipeline results");
    showPipelineResults->setCheckable(true);
    showPipelineResults->setChecked(true);
    connect(showPipelineResults, &QAction::triggered, this, &DialogImageViewer::onShowPipelineResults);
    toolbarTop->addAction(showPipelineResults);

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
    connect(mFillOVerlay, &QAction::triggered, this, &DialogImageViewer::onSettingChanged);
    toolbarTop->addAction(mFillOVerlay);

    toolbarTop->addSeparator();

    QAction *fitToScreen = new QAction(generateSvgIcon("zoom-fit-best"), "");
    fitToScreen->setStatusTip("Fit image to screen");
    fitToScreen->setObjectName("ToolButton");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    QAction *zoomIn = new QAction(generateSvgIcon("zoom-in"), "");
    zoomIn->setStatusTip("Zoom image in");
    zoomIn->setObjectName("ToolButton");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomInClicked);
    toolbarTop->addAction(zoomIn);

    QAction *zoomOut = new QAction(generateSvgIcon("zoom-out"), "");
    zoomOut->setObjectName("ToolButton");
    zoomIn->setStatusTip("Zoom image out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomOutClicked);
    toolbarTop->addAction(zoomOut);

    toolbarTop->addSeparator();

    //
    // Preview size
    //
    {
      auto *resolutionMenu = new QMenu();
      mPreviewSizeGroup    = new QActionGroup(toolbarTop);
      auto *r8192          = resolutionMenu->addAction("8192x8192");
      mPreviewSizeGroup->addAction(r8192);
      r8192->setCheckable(true);
      auto *r4096 = resolutionMenu->addAction("4096x4096");
      mPreviewSizeGroup->addAction(r4096);
      r4096->setCheckable(true);
      auto *r2048 = resolutionMenu->addAction("2048x2048");
      mPreviewSizeGroup->addAction(r2048);
      r2048->setCheckable(true);
      r2048->setChecked(true);
      auto *r1024 = resolutionMenu->addAction("1024x1024");
      mPreviewSizeGroup->addAction(r1024);
      r1024->setCheckable(true);
      auto *r512 = resolutionMenu->addAction("512x512");
      mPreviewSizeGroup->addAction(r512);
      r512->setCheckable(true);
      auto *r256 = resolutionMenu->addAction("256x256");
      mPreviewSizeGroup->addAction(r256);
      r256->setCheckable(true);
      auto *r128 = resolutionMenu->addAction("128x128");
      mPreviewSizeGroup->addAction(r128);
      r128->setCheckable(true);
      previewSize = new QAction(generateSvgIcon("computer"), "");
      previewSize->setStatusTip("Set preview resolution");
      previewSize->setMenu(resolutionMenu);
      toolbarTop->addAction(previewSize);
      auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(previewSize));
      btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
      connect(mPreviewSizeGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingChanged);
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
      connect(mZProjectionGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingChanged);
      mZProjectionAction->setVisible(false);
    }

    mMainLayout->addWidget(toolbarTop);
    // addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbarTop);
  }

  // Central images
  {
    mCentralLayout      = new QBoxLayout(QBoxLayout::TopToBottom);
    auto *centralWidget = new QWidget();
    mCentralLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setContentsMargins(0, 0, 0, 0);

    if(showOriginalImage) {
      auto *leftVerticalLayout = new QVBoxLayout();
      mHistoToolbarLeft        = new HistoToolbar(static_cast<int32_t>(ImageView::LEFT), this, &mPreviewImages.originalImage);
      mImageViewLeft.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      leftVerticalLayout->addWidget(&mImageViewLeft);
      leftVerticalLayout->addWidget(mHistoToolbarLeft);
      mCentralLayout->addLayout(leftVerticalLayout);
      mImageViewLeft.resetImage();
    } else {
      mImageViewRight.setEnableThumbnail(true);
    }

    auto *rightVerticalLayout = new QVBoxLayout();
    mHistoToolbarRight        = new HistoToolbar(static_cast<int32_t>(ImageView::RIGHT), this, &mPreviewImages.editedImage);
    mImageViewRight.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightVerticalLayout->addWidget(&mImageViewRight);
    rightVerticalLayout->addWidget(mHistoToolbarRight);
    mCentralLayout->addLayout(rightVerticalLayout);
    mImageViewRight.resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(mCentralLayout);
    mMainLayout->addWidget(centralWidget);

    if(showOriginalImage) {
      connect(&mImageViewLeft, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onLeftViewChanged);
      connect(&mImageViewLeft, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
    }

    connect(&mImageViewRight, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onRightViewChanged);
    connect(&mImageViewRight, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
    connect(&mImageViewRight, &PanelImageView::classesToShowChanged, this, &DialogImageViewer::onSettingChanged);
  }

  // Bottom toolbar
  {
    mSpinnerActTimeStack = new QSpinBox();
    mSpinnerActTimeStack->setValue(0);
    connect(mSpinnerActTimeStack, &QSpinBox::valueChanged, [this] { emit onSettingChanged(); });

    // Create spacer widgets
    auto *leftSpacer = new QWidget;
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *rightSpacer = new QWidget;
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    mPlaybackToolbar = new QToolBar();
    mPlaybackToolbar->addWidget(leftSpacer);
    auto *skipBackward = new QAction(generateSvgIcon("media-skip-backward"), "");
    connect(skipBackward, &QAction::triggered, [this] {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(0);
      mSpinnerActTimeStack->blockSignals(false);
      emit onSettingChanged();
    });
    mPlaybackToolbar->addAction(skipBackward);

    auto *seekBackward = new QAction(generateSvgIcon("media-seek-backward"), "");
    connect(seekBackward, &QAction::triggered, [this] {
      if(mSpinnerActTimeStack->value() > 0) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() - 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      emit onSettingChanged();
    });
    mPlaybackToolbar->addAction(seekBackward);

    // ==========================================================
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
      if(mSpinnerActTimeStack->value() < getMaxTimeStacks()) {
        mSpinnerActTimeStack->blockSignals(true);
        mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
        mSpinnerActTimeStack->blockSignals(false);
      }
      emit onSettingChanged();
    });
    mPlaybackToolbar->addAction(seekForward);
    mPlaybackToolbar->addWidget(rightSpacer);

    if(toolbarParent == nullptr) {
      mMainLayout->addWidget(mPlaybackToolbar);
    } else {
      mPlaybackToolbar->setVisible(false);
      toolbarParent->addToolBar(Qt::ToolBarArea::BottomToolBarArea, mPlaybackToolbar);
    }
  }

  // setLayout(layout);
  mainContainer->setLayout(mMainLayout);
  setWidget(mainContainer);

  // Init
  triggerPreviewUpdate(ImageView::BOTH, true);

  // Video timer
  mPlayTimer = new QTimer();
  QObject::connect(mPlayTimer, &QTimer::timeout, [&] {
    if(mSpinnerActTimeStack->value() < getMaxTimeStacks()) {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(mSpinnerActTimeStack->value() + 1);
      mSpinnerActTimeStack->blockSignals(false);
    } else {
      mSpinnerActTimeStack->blockSignals(true);
      mSpinnerActTimeStack->setValue(0);
      mSpinnerActTimeStack->blockSignals(false);
    }
    emit onSettingChanged();
  });

  // Connect signal to detect docking/floating changes
  connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
    if(floating) {
      setMaximumWidth(10000);    // Remove max width cap
      setMinimumWidth(1200);     // Wider when floating
      setMinimumHeight(600);
      mCentralLayout->setDirection(QBoxLayout::LeftToRight);
      resize(1300, 700);

      if(mWindowMain != nullptr) {
        mWindowMain->removeToolBar(mPlaybackToolbar);
        mMainLayout->addWidget(mPlaybackToolbar);
        mPlaybackToolbar->show();
      }

    } else {
      setMaximumWidth(500);    // Restrict width when docked
      setMinimumHeight(0);
      setMinimumWidth(500);    // Restore min width when docked
      mCentralLayout->setDirection(QBoxLayout::TopToBottom);

      if(mWindowMain != nullptr) {
        mMainLayout->removeWidget(mPlaybackToolbar);
        mWindowMain->addToolBar(Qt::ToolBarArea::BottomToolBarArea, mPlaybackToolbar);
        mPlaybackToolbar->show();
      }
    }
  });
}

DialogImageViewer::~DialogImageViewer()
{
  if(mPreviewThread != nullptr) {
    if(mPreviewThread->joinable()) {
      mPreviewThread->join();
    }
  }
}

void DialogImageViewer::fitImageToScreenSize()
{
  mImageViewLeft.fitImageToScreenSize();
  mImageViewRight.fitImageToScreenSize();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::triggerPreviewUpdate(ImageView view, bool withUserHistoSettings)
{
  if(mPreviewCounter == 0) {
    {
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter++;
    }
    if(mPreviewThread != nullptr) {
      if(mPreviewThread->joinable()) {
        mPreviewThread->join();
      }
    }
    mPreviewThread = std::make_unique<std::thread>([this, withUserHistoSettings, view] {
      int previewCounter = 0;
      do {
        if(withUserHistoSettings) {
          if(nullptr != mHistoToolbarLeft && (view == ImageView::LEFT || view == ImageView::BOTH)) {
            auto [value, scaling, offset] = mHistoToolbarLeft->getHistoSettings();
            mPreviewImages.originalImage.setBrightnessRange(0, value, scaling, offset);
            mImageViewLeft.emitUpdateImage();
          }
          if(nullptr != mHistoToolbarRight && (view == ImageView::RIGHT || view == ImageView::BOTH)) {
            auto [value, scaling, offset] = mHistoToolbarRight->getHistoSettings();
            mPreviewImages.editedImage.setBrightnessRange(0, value, scaling, offset);
            mImageViewRight.emitUpdateImage();
          }
        } else {
          if(view == ImageView::LEFT || view == ImageView::BOTH) {
            mImageViewLeft.emitUpdateImage();
          }
          if(view == ImageView::RIGHT || view == ImageView::BOTH) {
            mImageViewRight.emitUpdateImage();
          }
        }
        // mPreviewImages.thumbnail.setBrightnessRange(0, mSlider->value(), mSliderScaling->value(), mSliderHistogramOffset->value());
        std::this_thread::sleep_for(20ms);
        {
          std::lock_guard<std::mutex> lock(mPreviewMutex);
          previewCounter = mPreviewCounter;
          previewCounter--;
          mPreviewCounter = previewCounter;
        }
      } while(previewCounter > 0);
    });
  } else {
    std::lock_guard<std::mutex> lock(mPreviewMutex);
    mPreviewCounter++;
  }
}

///
/// \brief   Update the preview in the other window after focus lost
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::leaveEvent(QEvent *event)
{
  QDockWidget::leaveEvent(event);    // Call the base class handler if needed
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::imageUpdated(const ctrl::Preview::PreviewResults &info, const std::map<enums::ClassIdIn, QString> &classes)
{
  updatePlaybackToolbarVisible();
  mSpinnerActTimeStack->setMaximum(getMaxTimeStacks());
  mImageViewLeft.imageUpdated(info, classes);
  mImageViewRight.imageUpdated(info, classes);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onLeftViewChanged()
{
  mImageViewRight.blockSignals(true);
  mImageViewLeft.blockSignals(true);

  mImageViewRight.setCursorPosition(mImageViewLeft.getCursorPosition());
  mImageViewRight.setTransform(mImageViewLeft.transform());
  mImageViewRight.horizontalScrollBar()->setValue(mImageViewLeft.horizontalScrollBar()->value());
  mImageViewRight.verticalScrollBar()->setValue(mImageViewLeft.verticalScrollBar()->value());

  mImageViewRight.blockSignals(false);
  mImageViewLeft.blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onRightViewChanged()
{
  mImageViewRight.blockSignals(true);
  mImageViewLeft.blockSignals(true);

  mImageViewLeft.setCursorPosition(mImageViewRight.getCursorPosition());
  mImageViewLeft.setTransform(mImageViewRight.transform());
  mImageViewLeft.horizontalScrollBar()->setValue(mImageViewRight.horizontalScrollBar()->value());
  mImageViewLeft.verticalScrollBar()->setValue(mImageViewRight.verticalScrollBar()->value());

  mImageViewRight.blockSignals(false);
  mImageViewLeft.blockSignals(false);
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
void DialogImageViewer::onSetSateToMove()
{
  mImageViewLeft.setState(PanelImageView::State::MOVE);
  mImageViewRight.setState(PanelImageView::State::MOVE);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onSetStateToPaintRect()
{
  mImageViewLeft.setState(PanelImageView::State::PAINT);
  mImageViewRight.setState(PanelImageView::State::PAINT);
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
  mImageViewLeft.setShowThumbnail(checked);
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
  mImageViewLeft.setShowPixelInfo(checked);
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
  mImageViewLeft.setShowCrosshandCursor(checked);
  mImageViewRight.setShowCrosshandCursor(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowPipelineResults(bool checked)
{
  // mImageViewLeft.setShowPipelineResults(checked);
  mImageViewRight.setShowPipelineResults(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onTileClicked(int32_t tileX, int32_t tileY)
{
  emit tileClicked(tileX, tileY);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::closeEvent(QCloseEvent *event)
{
  event->ignore();    // Block the default close behavior

  // Optionally re-dock if floating
  if(isFloating()) {
    setFloating(false);
  }

  // Optionally just ensure it's visible again
  show();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setCrossHairCursorPositionAndCenter(const QRect &boundingRect)
{
  mImageViewLeft.setLockCrosshandCursor(true);
  mImageViewRight.setLockCrosshandCursor(true);
  mImageViewLeft.setCursorPositionFromOriginalImageCoordinatesAndCenter(boundingRect);
  mImageViewRight.setCursorPositionFromOriginalImageCoordinatesAndCenter(boundingRect);
}

}    // namespace joda::ui::gui
