///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qtmetamacros.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "ui/container/panel_edit_base.hpp"
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"
#include "panel_voronoi_overview.hpp"

namespace joda::ui::qt {

class WindowMain;
class ContainerVoronoi;

class PanelVoronoiEdit : public PanelEdit
{
  Q_OBJECT

public:
  PanelVoronoiEdit(WindowMain *wm, ContainerVoronoi *);
  ~PanelVoronoiEdit();
  void init();

  void setActive(bool setActive)
  {
    if(!mIsActiveShown && setActive) {
      mIsActiveShown = true;
      valueChangedEvent();
    }
    if(!setActive) {
      mIsActiveShown = false;
      std::lock_guard<std::mutex> lock(mPreviewMutex);
      mPreviewCounter = 0;
    }
  }

private:
  static constexpr int32_t PREVIEW_BASE_SIZE = 450;
  ContainerVoronoi *mParentContainer;
  PanelPreview *mPreviewImage    = nullptr;
  WaitingSpinnerWidget *mSpinner = nullptr;
  std::mutex mPreviewMutex;
  int mPreviewCounter                         = 0;
  std::unique_ptr<std::thread> mPreviewThread = nullptr;
  bool mIsActiveShown                         = false;

  void valueChangedEvent() override;
};

}    // namespace joda::ui::qt