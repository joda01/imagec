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
  PreviewLabel(int width, int height, QWidget *parent = nullptr);
  void setPixmap(const QPixmap &pix);
  void resetImage();
  void fitImageToScreenSize();
  void zoomImage(bool inOut);

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

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t PLACEHOLDER_BASE_SIZE = 450;
  static inline const QString PLACEHOLDER{":/icons/outlined/icons8-picture-1000-lightgray.png"};

  /////////////////////////////////////////////////////
  bool mPlaceholderImageSet = true;
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
  PanelPreview(int width, int height, QWidget *parent);
  void setPixmap(const QPixmap &pix, int width, int height, const QString &info)
  {
    mPreviewLabel.setPixmap(pix);
    mPreviewInfo->setText(info);
  }
  void resetImage(const QString &info)
  {
    mPreviewLabel.resetImage();
    mPreviewInfo->setText(info);
  }

private slots:
  void onFitImageToScreenSizeClicked();
  void onZoomOutClicked();
  void onZoomInClicked();

private:
  /////////////////////////////////////////////////////
  QWidget *createToolBar();
  QLabel *mPreviewInfo;

  /////////////////////////////////////////////////////
  PreviewLabel mPreviewLabel;
};
}    // namespace joda::ui::qt
