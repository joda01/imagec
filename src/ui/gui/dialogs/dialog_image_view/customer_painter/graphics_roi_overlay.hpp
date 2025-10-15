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
#include <qobject.h>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/customer_painter/graphics_contour_overlay.hpp"

class ContourOverlay;

class RoiOverlay : public QObject, public QGraphicsPixmapItem
{
  Q_OBJECT

public:
  RoiOverlay(const std::shared_ptr<joda::atom::ObjectList> &, const joda::settings::Classification *, ContourOverlay *, QWidget *parent);

  /////////////////////////////////////////////////////
  void setOverlay(const cv::Size &imageSize, const cv::Size &previewSize, const joda::enums::TileInfo &tileInfo);
  void refresh();
  void setAlpha(float);
  void setFill(bool fill)
  {
    mFill = fill;
    refresh();
  }
  void setSelectable(bool select);
  void setSelectedRois(const std::set<joda::atom::ROI *> &idxs);
  bool deleteSelectedRois();
  void deleteRois(const std::set<joda::atom::ROI *> &idxs);

signals:
  void paintedPolygonClicked(std::set<joda::atom::ROI *>);

private:
  /////////////////////////////////////////////////////
  void prepareContour(const joda::atom::ROI *roi, const QColor &colBorder);
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  [[nodiscard]] joda::atom::ROI *findRoiAt(const QPointF &itemPoint) const;

  /////////////////////////////////////////////////////
  cv::Size mImageSize;
  cv::Size mPreviewSize;
  joda::enums::TileInfo mTileInfo;
  std::shared_ptr<joda::atom::ObjectList> mObjectMap            = nullptr;
  const joda::settings::Classification *mClassificationSettings = nullptr;
  float mAlpha                                                  = 0.8F;
  bool mFill                                                    = true;
  bool mSelectable                                              = false;

  /////////////////////////////////////////////////////
  std::set<joda::atom::ROI *> mSelectedRois;
  ContourOverlay *mContourOverlay;
  QWidget *mParentWidget;

  // Cntours ///////////////
  ColorMap_t mContoursPerColor;
};
