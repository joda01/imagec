///
/// \file      dialog_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "dialog_image_view.hpp"
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qslider.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <thread>
#include "backend/helper/image/image.hpp"
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
DialogImageViewer::DialogImageViewer(QWidget *parent) :
    QMainWindow(parent), mImageViewLeft(mPreviewImages.originalImage, mPreviewImages.thumbnail, false, false),
    mImageViewRight(mPreviewImages.previewImage, mPreviewImages.thumbnail, true, true)
{
  // setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMaximizeButtonHint);
  setBaseSize(1200, 600);
  setMinimumSize(1200, 600);

  {
    QToolBar *toolbarTop = new QToolBar();

    QAction *fitToScreen = new QAction(generateIcon("full-screen"), "");
    fitToScreen->setObjectName("ToolButton");
    fitToScreen->setToolTip("Fit image to screen");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    QAction *zoomIn = new QAction(generateIcon("zoom-in"), "");
    zoomIn->setObjectName("ToolButton");
    zoomIn->setToolTip("Zoom in");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomInClicked);
    toolbarTop->addAction(zoomIn);

    QAction *zoomOut = new QAction(generateIcon("zoom-out"), "");
    zoomOut->setObjectName("ToolButton");
    zoomOut->setToolTip("Zoom out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomOutClicked);
    toolbarTop->addAction(zoomOut);

    toolbarTop->addSeparator();

    QActionGroup *buttonGroup = new QActionGroup(toolbarTop);

    QAction *action2 = new QAction(generateIcon("hand"), "");
    action2->setCheckable(true);
    action2->setChecked(true);
    connect(action2, &QAction::triggered, this, &DialogImageViewer::onSetSateToMove);
    buttonGroup->addAction(action2);
    toolbarTop->addAction(action2);

    QAction *paintRectangle = new QAction(generateIcon("rectangle"), "");
    paintRectangle->setCheckable(true);
    connect(paintRectangle, &QAction::triggered, this, &DialogImageViewer::onSetStateToPaintRect);
    buttonGroup->addAction(paintRectangle);
    // toolbarTop->addAction(paintRectangle);

    toolbarTop->addSeparator();

    QAction *showThumbnail = new QAction(generateIcon("picture-in-picture-alternative"), "");
    showThumbnail->setCheckable(true);
    showThumbnail->setChecked(true);
    connect(showThumbnail, &QAction::triggered, this, &DialogImageViewer::onShowThumbnailChanged);
    toolbarTop->addAction(showThumbnail);

    QAction *showPixelInfo = new QAction(generateIcon("abscissa"), "");
    showPixelInfo->setCheckable(true);
    showPixelInfo->setChecked(true);
    connect(showPixelInfo, &QAction::triggered, this, &DialogImageViewer::onShowPixelInfo);
    toolbarTop->addAction(showPixelInfo);

    QAction *showCrossHairCursor = new QAction(generateIcon("crosshair"), "");
    showCrossHairCursor->setToolTip("Right click to place a reference cursor.");
    showCrossHairCursor->setCheckable(true);
    showCrossHairCursor->setChecked(false);
    connect(showCrossHairCursor, &QAction::triggered, this, &DialogImageViewer::onShowCrossHandCursor);
    toolbarTop->addAction(showCrossHairCursor);

    addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbarTop);
  }

  // Central images
  {
    QGridLayout *centralLayout = new QGridLayout();
    QWidget *centralWidget     = new QWidget();
    mImageViewLeft.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralLayout->addWidget(&mImageViewLeft, 0, 0);
    mImageViewLeft.resetImage();

    mImageViewRight.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralLayout->addWidget(&mImageViewRight, 0, 1);
    mImageViewRight.resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    connect(&mImageViewLeft, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onLeftViewChanged);
    connect(&mImageViewRight, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onRightViewChanged);
    connect(&mImageViewLeft, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
    connect(&mImageViewRight, &PanelImageView::tileClicked, this, &DialogImageViewer::onTileClicked);
  }

  // Toolbar buttom
  {
    QToolBar *toolBar = new QToolBar();
    toolBar->setMaximumHeight(32);

    mSlider = new QSlider();
    mSlider->setMinimum(1);
    mSlider->setMaximum(UINT16_MAX);
    mSlider->setValue(600);
    mSlider->setOrientation(Qt::Orientation::Horizontal);
    connect(mSlider, &QSlider::valueChanged, this, &DialogImageViewer::onSliderMoved);
    toolBar->addWidget(mSlider);

    QAction *fitToScreen = new QAction(generateIcon("automatic-contrast"), "");
    fitToScreen->setObjectName("ToolButton");
    fitToScreen->setToolTip("Auto adjust");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::autoAdjustHistogram);
    toolBar->addAction(fitToScreen);

    QAction *action1 = new QAction(generateIcon("normal-distribution-histogram"), "");
    connect(action1, &QAction::triggered, this, &DialogImageViewer::onShowHistogramDialog);
    toolBar->addAction(action1);

    QAction *zoomIn = new QAction(generateIcon("zoom-in"), "");
    zoomIn->setObjectName("ToolButton");
    zoomIn->setToolTip("Zoom in");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomHistogramInClicked);
    toolBar->addAction(zoomIn);

    QAction *zoomOut = new QAction(generateIcon("zoom-out"), "");
    zoomOut->setObjectName("ToolButton");
    zoomOut->setToolTip("Zoom out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomHistogramOutClicked);
    toolBar->addAction(zoomOut);

    addToolBar(Qt::ToolBarArea::BottomToolBarArea, toolBar);
  }
  createHistogramDialog();
  onSliderMoved(0);
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
void DialogImageViewer::autoAdjustHistogram()
{
  mPreviewImages.originalImage.autoAdjustBrightnessRange();
  triggerPreviewUpdate(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onSliderMoved(int position)
{
  blockSignals(true);
  int number = (float) UINT16_MAX / mSliderScaling->value();
  int max    = UINT16_MAX - number;
  mSliderHistogramOffset->setMaximum(max);
  mSlider->setMinimum(mSliderHistogramOffset->value());
  mSlider->setMaximum(mSliderHistogramOffset->value() + number);
  blockSignals(false);

  triggerPreviewUpdate(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::triggerPreviewUpdate(bool withUserHistoSettings)
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
    mPreviewThread = std::make_unique<std::thread>([this, withUserHistoSettings] {
      int previewCounter = 0;
      do {
        if(withUserHistoSettings) {
          mPreviewImages.originalImage.setBrightnessRange(0, mSlider->value(), mSliderScaling->value(), mSliderHistogramOffset->value());
        }
        // mPreviewImages.thumbnail.setBrightnessRange(0, mSlider->value(), mSliderScaling->value(), mSliderHistogramOffset->value());
        mImageViewLeft.emitUpdateImage();
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
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::imageUpdated()
{
  mImageViewLeft.imageUpdated();
  mImageViewRight.imageUpdated();
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
void DialogImageViewer::createHistogramDialog()
{
  mHistogramDialog = new QDialog(this);
  mHistogramDialog->setWindowTitle("Histogram");
  // mHistogramDialog->setModal(true);

  QVBoxLayout *layoutMain = new QVBoxLayout();
  // Tools
  {
    QHBoxLayout *layout = new QHBoxLayout();
    mSliderScaling      = new QScrollBar(mHistogramDialog);
    mSliderScaling->setMinimum(1);
    mSliderScaling->setMaximum(UINT8_MAX);
    mSliderScaling->setValue(1);
    mSliderScaling->setOrientation(Qt::Orientation::Horizontal);
    mSliderScaling->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mSliderScaling, &QSlider::valueChanged, this, &DialogImageViewer::onSliderMoved);

    layout->addWidget(mSliderScaling);
    layoutMain->addWidget(new QLabel("Histogram zoom"));
    layoutMain->addLayout(layout);
  }
  {
    QHBoxLayout *layout    = new QHBoxLayout();
    mSliderHistogramOffset = new QScrollBar(mHistogramDialog);
    mSliderHistogramOffset->setMinimum(0);
    mSliderHistogramOffset->setMaximum(0);
    mSliderHistogramOffset->setValue(0);
    mSliderHistogramOffset->setOrientation(Qt::Orientation::Horizontal);
    mSliderScaling->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(mSliderHistogramOffset, &QScrollBar::valueChanged, this, &DialogImageViewer::onSliderMoved);

    layout->addWidget(mSliderHistogramOffset);
    layoutMain->addWidget(new QLabel("Histogram position"));
    layoutMain->addLayout(layout);
  }

  mHistogramDialog->setLayout(layoutMain);
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
void DialogImageViewer::onShowHistogramDialog()
{
  if(!mHistogramDialog->isVisible()) {
    mHistogramDialog->show();
  }
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
void DialogImageViewer::onZoomHistogramOutClicked()
{
  auto value = mSliderScaling->value() - HISTOGRAM_ZOOM_STEP;
  mSliderScaling->setValue(value);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onZoomHistogramInClicked()
{
  auto value = mSliderScaling->value() + HISTOGRAM_ZOOM_STEP;
  mSliderScaling->setValue(value);
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

}    // namespace joda::ui::gui
