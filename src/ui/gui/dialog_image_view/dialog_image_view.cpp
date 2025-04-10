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
#include <qslider.h>
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
DialogImageViewer::DialogImageViewer(QWidget *parent) :
    QDockWidget(parent), mImageViewLeft(&mPreviewImages.originalImage, &mPreviewImages.thumbnail, nullptr, true),
    mImageViewRight(&mPreviewImages.editedImage, &mPreviewImages.thumbnail, &mPreviewImages.overlay, false)
{
  setWindowTitle("Image view");
  setVisible(false);
  setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  // Set initial size constraints
  setMaximumWidth(500);    // Max width when docked
  setMinimumWidth(500);    // Min width even when docked

  // Connect signal to detect docking/floating changes
  connect(this, &QDockWidget::topLevelChanged, this, [this](bool floating) {
    if(floating) {
      setMaximumWidth(10000);    // Remove max width cap
      setMinimumWidth(1200);     // Wider when floating
      setMinimumHeight(600);
      setMinimumHeight(600);
      mCentralLayout->setDirection(QBoxLayout::LeftToRight);
      resize(1200, 600);
    } else {
      setMaximumWidth(500);    // Restrict width when docked
      setMinimumHeight(0);
      setMinimumWidth(500);    // Restore min width when docked
      mCentralLayout->setDirection(QBoxLayout::TopToBottom);
      setWindowTitle("");
    }
  });

  mImageViewRight.setShowPipelineResults(true);

  auto *mainContainer = new QWidget();
  auto *layout        = new QVBoxLayout();

  {
    QToolBar *toolbarTop = new QToolBar();

    QAction *pinToTop = new QAction(generateIcon("pin"), "");
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

    QAction *action2 = new QAction(generateIcon("hand"), "");
    action2->setCheckable(true);
    action2->setChecked(true);
    connect(action2, &QAction::triggered, this, &DialogImageViewer::onSetSateToMove);
    buttonGroup->addAction(action2);
    // toolbarTop->addAction(action2);

    QAction *paintRectangle = new QAction(generateIcon("rectangle"), "");
    paintRectangle->setCheckable(true);
    connect(paintRectangle, &QAction::triggered, this, &DialogImageViewer::onSetStateToPaintRect);
    buttonGroup->addAction(paintRectangle);
    // toolbarTop->addAction(paintRectangle);

    // toolbarTop->addSeparator();

    QAction *showThumbnail = new QAction(generateSvgIcon("virtual-desktops"), "");
    showThumbnail->setCheckable(true);
    showThumbnail->setChecked(true);
    connect(showThumbnail, &QAction::triggered, this, &DialogImageViewer::onShowThumbnailChanged);
    toolbarTop->addAction(showThumbnail);

    QAction *showPixelInfo = new QAction(generateSvgIcon("coordinate"), "");
    showPixelInfo->setCheckable(true);
    showPixelInfo->setChecked(true);
    connect(showPixelInfo, &QAction::triggered, this, &DialogImageViewer::onShowPixelInfo);
    toolbarTop->addAction(showPixelInfo);

    QAction *showCrossHairCursor = new QAction(generateSvgIcon("crosshairs"), "");
    showCrossHairCursor->setToolTip("Right click to place a reference cursor.");
    showCrossHairCursor->setCheckable(true);
    showCrossHairCursor->setChecked(false);
    connect(showCrossHairCursor, &QAction::triggered, this, &DialogImageViewer::onShowCrossHandCursor);
    toolbarTop->addAction(showCrossHairCursor);

    toolbarTop->addSeparator();

    QAction *showOverlay = new QAction(generateSvgIcon("redeyes"), "");
    showOverlay->setToolTip("Show overlay");
    showOverlay->setCheckable(true);
    showOverlay->setChecked(true);
    connect(showOverlay, &QAction::triggered, [this](bool selected) { mImageViewRight.setShowOverlay(selected); });
    toolbarTop->addAction(showOverlay);

    mFillOVerlay = new QAction(generateSvgIcon("fill-color"), "");
    mFillOVerlay->setToolTip("Filled");
    mFillOVerlay->setCheckable(true);
    connect(mFillOVerlay, &QAction::triggered, this, &DialogImageViewer::onSettingChanged);
    toolbarTop->addAction(mFillOVerlay);

    toolbarTop->addSeparator();

    QAction *fitToScreen = new QAction(generateSvgIcon("zoom-fit-best"), "");
    fitToScreen->setObjectName("ToolButton");
    fitToScreen->setToolTip("Fit image to screen");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    QAction *zoomIn = new QAction(generateSvgIcon("zoom-in"), "");
    zoomIn->setObjectName("ToolButton");
    zoomIn->setToolTip("Zoom in");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomInClicked);
    toolbarTop->addAction(zoomIn);

    QAction *zoomOut = new QAction(generateSvgIcon("zoom-out"), "");
    zoomOut->setObjectName("ToolButton");
    zoomOut->setToolTip("Zoom out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomOutClicked);
    toolbarTop->addAction(zoomOut);

    toolbarTop->addSeparator();

    //
    // Preview size
    //
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
    auto *previewSize = new QAction(generateSvgIcon("computer"), "");
    previewSize->setMenu(resolutionMenu);
    toolbarTop->addAction(previewSize);
    auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(previewSize));
    btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    connect(mPreviewSizeGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingChanged);

    layout->addWidget(toolbarTop);
    // addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbarTop);
  }

  // Central images
  {
    mCentralLayout           = new QBoxLayout(QBoxLayout::TopToBottom);
    auto *centralWidget      = new QWidget();
    auto *leftVerticalLayout = new QVBoxLayout();
    mHistoToolbarLeft        = new HistoToolbar(static_cast<int32_t>(ImageView::LEFT), this, &mPreviewImages.originalImage);
    mImageViewLeft.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftVerticalLayout->addWidget(&mImageViewLeft);
    leftVerticalLayout->addWidget(mHistoToolbarLeft);
    mCentralLayout->addLayout(leftVerticalLayout);
    mImageViewLeft.resetImage();

    auto *rightVerticalLayout = new QVBoxLayout();
    mHistoToolbarRight        = new HistoToolbar(static_cast<int32_t>(ImageView::RIGHT), this, &mPreviewImages.editedImage);
    mImageViewRight.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightVerticalLayout->addWidget(&mImageViewRight);
    rightVerticalLayout->addWidget(mHistoToolbarRight);
    mCentralLayout->addLayout(rightVerticalLayout);
    mImageViewRight.resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(mCentralLayout);
    layout->addWidget(centralWidget);

    connect(&mImageViewLeft, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onLeftViewChanged);
    connect(&mImageViewRight, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onRightViewChanged);
    connect(&mImageViewLeft, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
    connect(&mImageViewRight, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
    connect(&mImageViewRight, &PanelImageView::classesToShowChanged, this, &DialogImageViewer::onSettingChanged);
  }

  // setLayout(layout);
  mainContainer->setLayout(layout);
  setWidget(mainContainer);
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
  if(mHistoToolbarLeft == nullptr || mHistoToolbarRight == nullptr) {
    return;
  }

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
          if(view == ImageView::LEFT) {
            auto [value, scaling, offset] = mHistoToolbarLeft->getHistoSettings();
            mPreviewImages.originalImage.setBrightnessRange(0, value, scaling, offset);
            mImageViewLeft.emitUpdateImage();
          }
          if(view == ImageView::RIGHT) {
            auto [value, scaling, offset] = mHistoToolbarRight->getHistoSettings();
            mPreviewImages.editedImage.setBrightnessRange(0, value, scaling, offset);
            mImageViewRight.emitUpdateImage();
          }
        } else {
          if(view == ImageView::LEFT) {
            mImageViewLeft.emitUpdateImage();
          }
          if(view == ImageView::RIGHT) {
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
  mImageViewLeft.zoomImage(true);
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
  mImageViewLeft.zoomImage(false);
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
  mImageViewLeft.fitImageToScreenSize();
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

}    // namespace joda::ui::gui
