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

#pragma once

#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <iostream>
#include <string>

namespace joda::ui::qt {

///
/// \class      PreviewLabel
/// \author     Joachim Danmayr
///
class PreviewLabel : public QGraphicsView
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  PreviewLabel(QWidget *parent = nullptr);
  void setPixmap(const QPixmap &pix, int width, int height);

signals:
  void updateImage();

protected:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *) override;
  void leaveEvent(QEvent *) override;
  void wheelEvent(QWheelEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void fitImageToScreenSize();

private:
  /////////////////////////////////////////////////////
  QPixmap mActPixmapOriginal;
  QGraphicsPixmapItem *mActPixmap = nullptr;
  QGraphicsScene *scene;
  bool isDragging;
  QPoint lastPos;

private slots:
  void onUpdateImage();
};

class PanelPreview : public QWidget
{
  Q_OBJECT

public:
  PanelPreview(QWidget *parent = nullptr);
  void setPixmap(const QPixmap &pix, int width, int height)
  {
    mPreviewLabel.setPixmap(pix, width, height);
  }

private slots:
  // void onZoomInClicked();

private:
  /////////////////////////////////////////////////////
  QWidget *createToolBar();

  /////////////////////////////////////////////////////
  PreviewLabel mPreviewLabel;
};
}    // namespace joda::ui::qt
