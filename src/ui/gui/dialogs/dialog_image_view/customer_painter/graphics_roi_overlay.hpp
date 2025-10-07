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
#include "ui/gui/dialogs/dialog_image_view/customer_painter/graphics_contour_overlay.hpp"

class ContourOverlay;

class RoiOverlay : public QGraphicsPixmapItem
{
public:
  RoiOverlay(const std::shared_ptr<joda::atom::ObjectList> &, const joda::settings::Classification *, ContourOverlay *);

  /////////////////////////////////////////////////////
  void setOverlay(const cv::Size &imageSize, const cv::Size &previewSize);
  void refresh();
  void setAlpha(float);
  void setFill(bool fill)
  {
    mFill = fill;
    refresh();
  }
  void setClassesToHide(const std::set<joda::enums::ClassId> &toHide);
  void setSelectable(bool select);

private:
  /////////////////////////////////////////////////////
  void prepareContour(const joda::atom::ROI *roi, const QColor &colBorder);
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  [[nodiscard]] joda::atom::ROI *findRoiAt(const QPointF &itemPoint) const;

  /////////////////////////////////////////////////////
  cv::Size mImageSize;
  cv::Size mPreviewSize;
  std::shared_ptr<joda::atom::ObjectList> mObjectMap            = nullptr;
  const joda::settings::Classification *mClassificationSettings = nullptr;
  float mAlpha                                                  = 0.8F;
  bool mFill                                                    = true;
  bool mSelectable                                              = false;

  /////////////////////////////////////////////////////
  std::set<joda::enums::ClassId> mToHide;
  std::set<joda::atom::ROI *> mSelectedRois;
  ContourOverlay *mContourOverlay;

  // Cntours ///////////////
  ColorMap_t mContoursPerColor;
};
