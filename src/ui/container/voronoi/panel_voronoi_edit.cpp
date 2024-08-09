///
/// \file      panel_channel.cpp
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

#include "panel_voronoi_edit.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>
#include "../container_function.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/window_main/window_main.hpp"
#include "container_voronoi.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelVoronoiEdit::PanelVoronoiEdit(WindowMain *wm, ContainerVoronoi *parentContainer) :
    PanelEdit(wm), mParentContainer(parentContainer)
{
  setObjectName("PanelVoronoiEdit");
  init();
}

void PanelVoronoiEdit::init()
{
  helper::LayoutGenerator layout(this);

  {
    auto *col = layout.addVerticalPanel();
    col->addGroup("Meta", {mParentContainer->mChannelName, mParentContainer->mColorAndChannelIndex,
                           mParentContainer->mChannelType});

    col->addGroup("Cross channel", {mParentContainer->mCrossChannelIntensity, mParentContainer->mCrossChannelCount});
  }

  {
    auto *col = layout.addVerticalPanel();
    col->addGroup("Voronoi", {mParentContainer->mVoronoiPoints, mParentContainer->mMaxVoronoiAreaSize,
                              mParentContainer->mOverlayMaskChannelIndex});

    col->addGroup("Object filter",
                  {mParentContainer->mMinParticleSize, mParentContainer->mMaxParticleSize,
                   mParentContainer->mExcludeAreasAtTheEdges, mParentContainer->mExcludeAreasWithoutCenterOfMass});
  }
}

PanelVoronoiEdit::~PanelVoronoiEdit()
{
  {
    std::lock_guard<std::mutex> lock(mPreviewMutex);
    mPreviewCounter = 0;
  }
  if(mPreviewThread != nullptr) {
    if(mPreviewThread->joinable()) {
      mPreviewThread->join();
    }
  }
  delete mPreviewImage;
  delete mSpinner;
}

void PanelVoronoiEdit::valueChangedEvent()
{
}

}    // namespace joda::ui::qt
