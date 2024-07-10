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
#include <qboxlayout.h>
#include <qdialog.h>
#include <qgridlayout.h>
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
DialogImageViewer::DialogImageViewer(QWidget *parent) : QDialog(parent)
{
  setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMaximizeButtonHint);
  setModal(false);
  setBaseSize(1200, 600);
  setMinimumSize(1200, 600);

  QVBoxLayout *layout = new QVBoxLayout(this);

  // Central images
  {
    QGridLayout *centralLayout = new QGridLayout();
    mImageViewLeft             = new PanelImageView(this);
    mImageViewLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralLayout->addWidget(mImageViewLeft, 0, 0);
    mImageViewLeft->resetImage();

    mImageViewRight = new PanelImageView(this);
    mImageViewRight->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralLayout->addWidget(mImageViewRight, 0, 1);
    mImageViewRight->resetImage();

    connect(mImageViewLeft, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onLeftViewChanged);
    connect(mImageViewRight, &PanelImageView::onImageRepainted, this, &DialogImageViewer::onRightViewChanged);
    layout->addLayout(centralLayout);
  }

  // Toolbar
  {
    QWidget *toolBar = new QWidget(this);
    toolBar->setMaximumHeight(32);
    QGridLayout *toolGrid = new QGridLayout();

    QHBoxLayout *leftToolBar = new QHBoxLayout();

    mSlider = new QSlider(this);
    mSlider->setMinimum(1);
    mSlider->setMaximum(UINT16_MAX);
    mSlider->setValue(600);
    mSlider->setOrientation(Qt::Orientation::Horizontal);
    connect(mSlider, &QSlider::valueChanged, this, &DialogImageViewer::onSliderMoved);
    leftToolBar->addWidget(mSlider);

    toolGrid->addLayout(leftToolBar, 0, 0);

    QHBoxLayout *rightToolBar = new QHBoxLayout();

    QAction *action1 = new QAction("Action 1", this);
    QAction *action2 = new QAction("Action 2", this);

    // connect(action1, &QAction::triggered, this, &MyDialog::onAction1Triggered);
    // connect(action2, &QAction::triggered, this, &MyDialog::onAction2Triggered);

    // toolBar->addAction(action1);
    // toolBar->addAction(action2);

    mSliderScaling = new QSlider(this);
    mSliderScaling->setMinimum(1);
    mSliderScaling->setMaximum(UINT8_MAX);
    mSliderScaling->setValue(128);
    mSliderScaling->setOrientation(Qt::Orientation::Horizontal);
    connect(mSliderScaling, &QSlider::valueChanged, this, &DialogImageViewer::onSliderMoved);
    rightToolBar->addWidget(mSliderScaling);

    mSliderHistogramOffset = new QScrollBar(this);
    mSliderHistogramOffset->setMinimum(0);
    mSliderHistogramOffset->setMaximum(0);
    mSliderHistogramOffset->setValue(0);
    mSliderHistogramOffset->setOrientation(Qt::Orientation::Horizontal);
    connect(mSliderHistogramOffset, &QScrollBar::valueChanged, this, &DialogImageViewer::onSliderMoved);
    rightToolBar->addWidget(mSliderHistogramOffset);

    toolGrid->addLayout(rightToolBar, 0, 1);
    toolBar->setLayout(toolGrid);
    layout->addWidget(toolBar);
  }

  setLayout(layout);
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
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewLeft->fitImageToScreenSize();
    mImageViewRight->fitImageToScreenSize();
  }
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
        mImageViewLeft->getImage().setBrightnessRange(0, mSlider->value(), mSliderScaling->value(),
                                                      mSliderHistogramOffset->value());
        mImageViewLeft->emitUpdateImage();
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
void DialogImageViewer::setImage(const joda::image::Image &leftImage, const joda::image::Image &rightImage)
{
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewLeft->setImage(leftImage);
    mImageViewRight->setImage(rightImage);
  }
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
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewRight->blockSignals(true);
    mImageViewLeft->blockSignals(true);

    mImageViewRight->horizontalScrollBar()->setValue(mImageViewLeft->horizontalScrollBar()->value());
    mImageViewRight->verticalScrollBar()->setValue(mImageViewLeft->verticalScrollBar()->value());
    mImageViewRight->setTransform(mImageViewLeft->transform());

    mImageViewRight->blockSignals(false);
    mImageViewLeft->blockSignals(false);
  }
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
  if(nullptr != mImageViewRight && nullptr != mImageViewLeft) {
    mImageViewRight->blockSignals(true);
    mImageViewLeft->blockSignals(true);

    mImageViewLeft->horizontalScrollBar()->setValue(mImageViewRight->horizontalScrollBar()->value());
    mImageViewLeft->verticalScrollBar()->setValue(mImageViewRight->verticalScrollBar()->value());
    mImageViewLeft->setTransform(mImageViewRight->transform());

    mImageViewRight->blockSignals(false);
    mImageViewLeft->blockSignals(false);
  }
}

}    // namespace joda::ui::qt
