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
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelPreview::PanelPreview(int width, int height, WindowMain *parent) :
    mParent(parent), mImageViewer(parent),
    mPreviewLabel(mImageViewer.getPreviewObject().previewImage, mImageViewer.getPreviewObject().thumbnail, true, true)
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

  mOpenFullScreenButton = new QPushButton(generateIcon("external-link"), "");
  mOpenFullScreenButton->setCheckable(true);
  mOpenFullScreenButton->setObjectName("ToolButton");
  mOpenFullScreenButton->setToolTip("Full screen");
  connect(mOpenFullScreenButton, &QPushButton::clicked, this, &PanelPreview::onOpenFullScreenClicked);
  layout->addWidget(mOpenFullScreenButton);

  connect(&mImageViewer, &DialogImageViewer::hidden, [this] { mOpenFullScreenButton->setChecked(false); });

  filled = new QPushButton(generateIcon("fill-color"), "");
  filled->setCheckable(true);
  filled->setObjectName("ToolButton");
  filled->setToolTip("Filled");
  connect(filled, &QPushButton::toggled, this, &PanelPreview::onSettingChanged);
  layout->addWidget(filled);

  //
  // Preview classes
  //
  mClassesClassesToShow = SettingBase::create<SettingComboBoxMultiClassificationIn>(mParent, generateIcon("circle"), "Classes to paint");
  mClassesClassesToShow->getInputObject()->setMaximumWidth(175);
  mClassesClassesToShow->getInputObject()->setMinimumWidth(175);
  mClassesClassesToShow->setValue(settings::ObjectInputClasses{enums::ClassIdIn::$});
  layout->addWidget(mClassesClassesToShow->getInputObject());

  //
  // Preview size
  //
  mPreviewSize = new QComboBox();
  mPreviewSize->addItem("8192x8192", static_cast<int32_t>(8192));
  mPreviewSize->addItem("4096x4096", static_cast<int32_t>(4096));
  mPreviewSize->addItem("2048x2048", static_cast<int32_t>(2048));
  mPreviewSize->addItem("1024x1024", static_cast<int32_t>(1024));
  mPreviewSize->addItem("512x512", static_cast<int32_t>(512));
  mPreviewSize->addItem("256x256", static_cast<int32_t>(256));
  mPreviewSize->addItem("128x128", static_cast<int32_t>(128));
  mPreviewSize->addItem("64x64", static_cast<int32_t>(64));
  mPreviewSize->setCurrentIndex(mPreviewSize->findData(2048));
  layout->addWidget(mPreviewSize);

  connect(mPreviewSize, &QComboBox::currentIndexChanged, this, &PanelPreview::onSettingChanged);
  connect(mClassesClassesToShow.get(), &SettingBase::valueChanged, this, &PanelPreview::onSettingChanged);

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

void PanelPreview::onOpenFullScreenClicked(bool checked)
{
  if(checked) {
    mImageViewer.show();
    mImageViewer.fitImageToScreenSize();
  } else {
    mImageViewer.hide();
  }
}

void PanelPreview::onTileClicked(int32_t tileX, int32_t tileY)
{
  emit tileClicked(tileX, tileY);
}

}    // namespace joda::ui::gui
