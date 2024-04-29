///
/// \file      dialog_shadow.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "dialog_shadow.h"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QtGui>

namespace joda::ui::qt {

DialogShadow::DialogShadow(QWidget *parent, bool showClose) : QDialog(parent), mShowCloseButton(showClose)
{
  setModal(true);
  setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Dialog);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setGraphicsEffect(new QGraphicsDropShadowEffect(this));
  dynamic_cast<QGraphicsDropShadowEffect *>(graphicsEffect())->setBlurRadius(10);
  dynamic_cast<QGraphicsDropShadowEffect *>(graphicsEffect())->setColor(Qt::black);
  dynamic_cast<QGraphicsDropShadowEffect *>(graphicsEffect())->setOffset(0, 0);

  // Dimmed background effect
  if(parent != nullptr) {
    dimmer = new QWidget(parent);
    dimmer->setStyleSheet("background-color: rgba(0, 0, 0, 100);");
    dimmer->setGeometry(0, 0, 99999, 999999);
    dimmer->raise();
    connect(parent, &QWidget::destroyed, this, &QWidget::close);
  }

  setStyleSheet("QDialog {background-color: white; border-radius: 28px;}");
}

int DialogShadow::exec()
{
  if(mShowCloseButton) {
    QWidget *buttons = new QWidget();
    buttons->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *hBox  = new QHBoxLayout(buttons);
    QPushButton *close = new QPushButton("Close", buttons);
    close->setCursor(Qt::PointingHandCursor);
    close->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(0, 0, 0, 0);"
        "   border: 1px solid rgba(0, 0, 0, 0);"
        "   color: rgb(255, 144, 144);"
        "   padding: 10px 10px;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "   font-weight: normal;"
        "   text-align: center;"
        "   text-decoration: none;"
        "}"

        "QPushButton:hover {"
        "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
        "}"

        "QPushButton:pressed {"
        "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
        "}");

    connect(close, &QPushButton::clicked, this, &DialogShadow::onCloseWindow);

    hBox->addStretch();
    hBox->addWidget(close);
    QLayout *mainL = layout();
    mainL->addWidget(buttons);
  }
  return QDialog::exec();
}

void DialogShadow::onCloseWindow()
{
  close();
}

void DialogShadow::paintEvent(QPaintEvent *ev)
{
  // If the transparency flag/hint aren't set then just use the default paint event.
  if(!(windowFlags() & Qt::FramelessWindowHint) && !testAttribute(Qt::WA_TranslucentBackground)) {
    QDialog::paintEvent(ev);
    return;
  }

  // Initialize the painter.
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setRenderHint(QPainter::SmoothPixmapTransform, false);
  QRect rect(8, 8, width() - 16, height() - 16);

  QPen pStyle;
  pStyle.setCapStyle(Qt::PenCapStyle::FlatCap);
  pStyle.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
  pStyle.setColor(Qt::white);
  pStyle.setStyle(Qt::PenStyle::SolidLine);
  pStyle.setWidth(1);
  p.setPen(pStyle);

  p.setBrush(Qt::white);
  // p.setBrush(Qt::BrushStyle::NoBrush);
  int radius = 28;
  if(radius > 0.0) {
    p.drawRoundedRect(rect, radius, radius, Qt::AbsoluteSize);

    QPainterPath rounded_rect;
    rounded_rect.addRoundedRect(rect, radius, radius);
    p.setClipPath(rounded_rect);

    p.fillPath(rounded_rect, p.brush());
    p.drawPath(rounded_rect);
  } else {
    p.drawRect(rect);
  }

  // C'est finí
  p.end();
}

}    // namespace joda::ui::qt
