///
/// \file      graphics_roi_overlay.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcolor.h>
#include <qgraphicseffect.h>
#include <qgraphicsitem.h>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"

class ContourOverlay : public QGraphicsItem
{
public:
  ContourOverlay(const joda::atom::ObjectMap *, const joda::settings::Classification *, QGraphicsItem *parent = nullptr);

  /////////////////////////////////////////////////////
  void setOverlay(const cv::Size &imageSize, const cv::Size &previewSize);
  void refresh();
  void setAlpha(float);

private:
  /////////////////////////////////////////////////////
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

  // Set the item's bounding rectangle (required by QGraphicsItem)
  [[nodiscard]] QRectF boundingRect() const override
  {
    // Return the bounding rect of the parent (the QGraphicsPixmapItem)
    if(parentItem() != nullptr) {
      return parentItem()->boundingRect();
    }
    return QRectF{0, 0, static_cast<qreal>(mPreviewSize.width), static_cast<qreal>(mPreviewSize.height)};
  }

  /////////////////////////////////////////////////////
  cv::Size mImageSize;
  cv::Size mPreviewSize;
  const joda::atom::ObjectMap *mObjectMap                       = nullptr;
  const joda::settings::Classification *mClassificationSettings = nullptr;

  std::vector<std::pair<QColor, std::vector<QPointF>>> mPointsToPaint;
  std::mutex mMutex;
};
