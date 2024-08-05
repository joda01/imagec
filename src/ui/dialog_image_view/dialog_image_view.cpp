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
/// \brief     A short description what happens here.
///

#include "dialog_image_view.hpp"
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qslider.h>
#include <cstdint>
#include <string>
#include <thread>
#include "backend/image_processing/image/image.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::DialogImageViewer(QWidget *parent) :
    QMainWindow(parent), mImageViewLeft(mPreviewImages.originalImage, mPreviewImages.thumbnail),
    mImageViewRight(mPreviewImages.previewImage, mPreviewImages.thumbnail)
{
  // setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMaximizeButtonHint);
  setBaseSize(1200, 600);
  setMinimumSize(1200, 600);

  {
    QToolBar *toolbarTop = new QToolBar();
    toolbarTop->setContentsMargins(0, 0, 0, 0);
    toolbarTop->setMaximumHeight(32);

    QAction *fitToScreen = new QAction(QIcon(":/icons/outlined/icons8-full-screen-50.png"), "");
    fitToScreen->setObjectName("ToolButton");
    fitToScreen->setToolTip("Fit image to screen");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    QAction *zoomIn = new QAction(QIcon(":/icons/outlined/icons8-zoom-in-50.png"), "");
    zoomIn->setObjectName("ToolButton");
    zoomIn->setToolTip("Zoom in");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomInClicked);
    toolbarTop->addAction(zoomIn);

    QAction *zoomOut = new QAction(QIcon(":/icons/outlined/icons8-zoom-out-50.png"), "");
    zoomOut->setObjectName("ToolButton");
    zoomOut->setToolTip("Zoom out");
    connect(zoomOut, &QAction::triggered, this, &DialogImageViewer::onZoomOutClicked);
    toolbarTop->addAction(zoomOut);

    toolbarTop->addSeparator();

    QActionGroup *buttonGroup = new QActionGroup(toolbarTop);

    QAction *action2 = new QAction(QIcon(":/icons/outlined/icons8-hand-50.png"), "");
    action2->setCheckable(true);
    action2->setChecked(true);
    connect(action2, &QAction::triggered, this, &DialogImageViewer::onSetSateToMove);
    buttonGroup->addAction(action2);
    toolbarTop->addAction(action2);

    QAction *paintRectangle = new QAction(QIcon(":/icons/outlined/icons8-rectangle-50.png"), "");
    paintRectangle->setCheckable(true);
    connect(paintRectangle, &QAction::triggered, this, &DialogImageViewer::onSetStateToPaintRect);
    buttonGroup->addAction(paintRectangle);
    // toolbarTop->addAction(paintRectangle);

    toolbarTop->addSeparator();

    QAction *showThumbnail = new QAction(QIcon(":/icons/outlined/icons8-picture-in-picture-50.png"), "");
    showThumbnail->setCheckable(true);
    showThumbnail->setChecked(true);
    connect(showThumbnail, &QAction::triggered, this, &DialogImageViewer::onShowThumbnailChanged);
    toolbarTop->addAction(showThumbnail);

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

    // QAction *fitToScreen = new QAction(QIcon(":/icons/outlined/icons8-full-screen-50.png"), "");
    // fitToScreen->setObjectName("ToolButton");
    // fitToScreen->setToolTip("Fit histogram to screen");
    // connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitHistogramToScreenSizeClicked);
    // toolBar->addAction(fitToScreen);

    QAction *action1 = new QAction(QIcon(":/icons/outlined/icons8-normal-distribution-histogram-50.png"), "");
    connect(action1, &QAction::triggered, this, &DialogImageViewer::onShowHistogramDialog);
    toolBar->addAction(action1);

    QAction *zoomIn = new QAction(QIcon(":/icons/outlined/icons8-zoom-in-50.png"), "");
    zoomIn->setObjectName("ToolButton");
    zoomIn->setToolTip("Zoom in");
    connect(zoomIn, &QAction::triggered, this, &DialogImageViewer::onZoomHistogramInClicked);
    toolBar->addAction(zoomIn);

    QAction *zoomOut = new QAction(QIcon(":/icons/outlined/icons8-zoom-out-50.png"), "");
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
void DialogImageViewer::onSliderMoved(int position)
{
  blockSignals(true);
  int number = (float) UINT16_MAX / mSliderScaling->value();
  int max    = UINT16_MAX - number;
  mSliderHistogramOffset->setMaximum(max);
  mSlider->setMinimum(mSliderHistogramOffset->value());
  mSlider->setMaximum(mSliderHistogramOffset->value() + number);
  blockSignals(false);

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
    mPreviewThread = std::make_unique<std::thread>([this] {
      int previewCounter = 0;
      do {
        mPreviewImages.originalImage.setBrightnessRange(0, mSlider->value(), mSliderScaling->value(),
                                                        mSliderHistogramOffset->value());

        mPreviewImages.thumbnail.setBrightnessRange(0, 600, 128, 0);
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

  mImageViewRight.horizontalScrollBar()->setValue(mImageViewLeft.horizontalScrollBar()->value());
  mImageViewRight.verticalScrollBar()->setValue(mImageViewLeft.verticalScrollBar()->value());
  mImageViewRight.setTransform(mImageViewLeft.transform());

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

  mImageViewLeft.horizontalScrollBar()->setValue(mImageViewRight.horizontalScrollBar()->value());
  mImageViewLeft.verticalScrollBar()->setValue(mImageViewRight.verticalScrollBar()->value());
  mImageViewLeft.setTransform(mImageViewRight.transform());

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
    mSliderScaling->setValue(128);
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
void DialogImageViewer::onFitHistogramToScreenSizeClicked()
{
  mSliderScaling->setValue(128);
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

}    // namespace joda::ui::qt
