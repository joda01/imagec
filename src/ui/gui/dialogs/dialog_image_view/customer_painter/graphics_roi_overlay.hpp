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

class ContourOverlay;

class RoiOverlay : public QGraphicsPixmapItem
{
public:
  RoiOverlay(const joda::atom::ObjectMap *, const joda::settings::Classification *);

  /////////////////////////////////////////////////////
  void setOverlay(const cv::Size &imageSize, const cv::Size &previewSize);
  void refresh();
  void setAlpha(float);

private:
  /////////////////////////////////////////////////////
  cv::Size mImageSize;
  cv::Size mPreviewSize;
  const joda::atom::ObjectMap *mObjectMap                       = nullptr;
  const joda::settings::Classification *mClassificationSettings = nullptr;
  QGraphicsOpacityEffect *mOpacityEffect;
  float mAlpha = 0.8F;
  bool mFill   = true;
};
