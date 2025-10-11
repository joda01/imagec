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

using ColorMap_t = std::vector<std::pair<QPen, QPolygonF>>;

class ContourOverlay : public QGraphicsItem
{
  friend class RoiOverlay;

public:
  ContourOverlay(QGraphicsItem *parent = nullptr);

  /////////////////////////////////////////////////////
  void refresh(const ColorMap_t *, const cv::Size &previewSize);

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

  cv::Size mPreviewSize;
  const ColorMap_t *mData = nullptr;
};
