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
/// \brief     A short description what happens here.
///

#include "panel_preview.hpp"
#include <qboxlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelPreview::PanelPreview(QWidget *parent) : mPreviewLabel(parent)
{
  QVBoxLayout *hLayout = new QVBoxLayout();
  this->setLayout(hLayout);
  hLayout->addWidget(&mPreviewLabel);
  hLayout->addWidget(createToolBar());
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
QWidget *PanelPreview::createToolBar()
{
  QWidget *container  = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();
  container->setLayout(layout);

  QPushButton *zoon = new QPushButton(QIcon(":/icons/outlined/icons8-search-50.png"), "");
  zoon->setCheckable(true);
  layout->addWidget(zoon);
  layout->addStretch();

  return container;
}

////////////////////////////////////////////////////////////////
// Image view section
//
PanelPreview::PreviewLabel::PreviewLabel(QWidget *parent) : QLabel(parent)
{
  setMouseTracking(true);
}

void PanelPreview::PreviewLabel::setPixmap(const QPixmap &pix, int width, int height)
{
  setMinimumWidth(width);
  setMinimumHeight(height);
  setFixedWidth(width);
  setFixedHeight(height);

  std::cout << std::to_string(width) << " | " << std::to_string(pix.width()) << std::endl;

  zoomFactor = (qreal) width / (qreal) pix.width();
  std::cout << std::to_string(zoomFactor) << std::endl;
  originalPixmap = pix;
  updateZoomedImage();
}

void PanelPreview::PreviewLabel::mouseMoveEvent(QMouseEvent *event)
{
}

void PanelPreview::PreviewLabel::enterEvent(QEnterEvent *)
{
}

void PanelPreview::PreviewLabel::leaveEvent(QEvent *)
{
}

void PanelPreview::PreviewLabel::wheelEvent(QWheelEvent *event)
{
}

void PanelPreview::PreviewLabel::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton) {
    zoomCenter = event->pos();
    zoom(true);
  }
  if(event->button() == Qt::RightButton) {
    zoomCenter = event->pos();
    zoom(false);
  }
}

void PanelPreview::PreviewLabel::fitToWindow()
{
  zoomFactor = (qreal) QLabel::width() / (qreal) originalPixmap.width();
  updateZoomedImage();
}

void PanelPreview::PreviewLabel::zoom(bool direction)
{
  if(direction) {
    zoomFactor += 0.03F;
  } else {
    zoomFactor -= 0.03F;
  }

  std::cout << std::to_string(zoomFactor) << std::endl;

  updateZoomedImage();
}

void PanelPreview::PreviewLabel::updateZoomedImage()
{
  /*scaledPixmap = originalPixmap.scaled(originalPixmap.width() * zoomFactor, originalPixmap.height() * zoomFactor,
                                       Qt::KeepAspectRatio, Qt::SmoothTransformation);*/

  QTransform transform;
  transform.translate(zoomCenter.x(), zoomCenter.y());
  transform.scale(zoomFactor, zoomFactor);
  transform.translate(-zoomCenter.x(), -zoomCenter.y());

  scaledPixmap = originalPixmap.transformed(transform);

  update();
}

void PanelPreview::PreviewLabel::paintEvent(QPaintEvent *event)
{
  QLabel::paintEvent(event);

  QPainter painter(this);
  painter.drawPixmap(QPoint(0, 0), scaledPixmap);
}

}    // namespace joda::ui::qt
