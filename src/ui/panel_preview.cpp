///
/// \file      panel_preview.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_preview.hpp"
#include <qboxlayout.h>
#include <qgraphicsview.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <memory>
#include <string>
#include "ui/helper/icon_generator.hpp"

namespace joda::ui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelPreview::PanelPreview(int width, int height, QWidget *parent) :
    mImageViewer(parent), mPreviewLabel(mImageViewer.getPreviewObject().previewImage, mImageViewer.getPreviewObject().thumbnail)
{
  mPreviewLabel.setMinimumWidth(width);
  mPreviewLabel.setMinimumHeight(height);
  mPreviewLabel.setMaximumWidth(width);
  mPreviewLabel.setMaximumHeight(height);
  mPreviewLabel.setFixedWidth(width);
  mPreviewLabel.setFixedHeight(height);

  QVBoxLayout *hLayout = new QVBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(hLayout);
  hLayout->addWidget(&mPreviewLabel);
  hLayout->addWidget(createToolBar());

  mPreviewInfo = new QLabel("-");
  hLayout->addWidget(mPreviewInfo);

  hLayout->addStretch();

  connect(&mImageViewer, &DialogImageViewer::tileClicked, this, &PanelPreview::onTileClicked);
  connect(&mPreviewLabel, &PanelImageView::tileClicked, this, &PanelPreview::onTileClicked);
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
QWidget *PanelPreview::createToolBar()
{
  QWidget *container  = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  container->setLayout(layout);

  QButtonGroup *buttonGroup = new QButtonGroup();

  QPushButton *move = new QPushButton(generateIcon("hand"), "");
  move->setObjectName("ToolButton");
  move->setCheckable(true);
  move->setToolTip("Move the image");
  move->setChecked(true);
  // buttonGroup->addButton(move);
  // layout->addWidget(move);

  QPushButton *fitToScreen = new QPushButton(generateIcon("full-screen"), "");
  fitToScreen->setObjectName("ToolButton");
  fitToScreen->setToolTip("Fit image to screen");
  connect(fitToScreen, &QPushButton::pressed, this, &PanelPreview::onFitImageToScreenSizeClicked);
  layout->addWidget(fitToScreen);

  QPushButton *zoomIn = new QPushButton(generateIcon("zoom-in"), "");
  zoomIn->setObjectName("ToolButton");
  zoomIn->setToolTip("Zoom in");
  connect(zoomIn, &QPushButton::pressed, this, &PanelPreview::onZoomInClicked);
  layout->addWidget(zoomIn);

  QPushButton *zoomOut = new QPushButton(generateIcon("zoom-out"), "");
  zoomOut->setObjectName("ToolButton");
  zoomOut->setToolTip("Zoom out");
  connect(zoomOut, &QPushButton::pressed, this, &PanelPreview::onZoomOutClicked);
  layout->addWidget(zoomOut);

  QPushButton *openFullScreen = new QPushButton(generateIcon("external-link"), "");
  openFullScreen->setObjectName("ToolButton");
  openFullScreen->setToolTip("Full screen");
  connect(openFullScreen, &QPushButton::pressed, this, &PanelPreview::onOpenFullScreenClickec);
  layout->addWidget(openFullScreen);

  filled = new QPushButton(generateIcon("fill-color"), "");
  filled->setCheckable(true);
  filled->setObjectName("ToolButton");
  filled->setToolTip("Filled");
  connect(filled, &QPushButton::toggled, this, &PanelPreview::onSettingChanged);
  layout->addWidget(filled);

  layout->addStretch();

  return container;
}

void PanelPreview::onZoomInClicked()
{
  mPreviewLabel.zoomImage(true);
}
void PanelPreview::onZoomOutClicked()
{
  mPreviewLabel.zoomImage(false);
}

void PanelPreview::onFitImageToScreenSizeClicked()
{
  mPreviewLabel.fitImageToScreenSize();
}

void PanelPreview::onOpenFullScreenClickec()
{
  mImageViewer.show();
  mImageViewer.fitImageToScreenSize();
}

void PanelPreview::onTileClicked(int32_t tileX, int32_t tileY)
{
  emit tileClicked(tileX, tileY);
}

}    // namespace joda::ui
