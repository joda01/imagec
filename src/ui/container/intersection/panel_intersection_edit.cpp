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

#include "panel_intersection_edit.hpp"
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
#include "container_intersection.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelIntersectionEdit::PanelIntersectionEdit(WindowMain *wm, ContainerIntersection *parentContainer) :
    PanelEdit(wm), mParentContainer(parentContainer)
{
  setObjectName("PanelIntersectionEdit");
  init();
}

void PanelIntersectionEdit::init()
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

    col->addGroup("Intersection", {mParentContainer->mCrossChannelIntersection, mParentContainer->mMinIntersection});
  }

  //
  // Signals from extern
  //
  connect(getWindowMain()->getFoundFilesCombo(), &QComboBox::currentIndexChanged, this,
          &PanelIntersectionEdit::onValueChanged);
  connect(getWindowMain()->getImageSeriesCombo(), &QComboBox::currentIndexChanged, this,
          &PanelIntersectionEdit::onValueChanged);
}

PanelIntersectionEdit::~PanelIntersectionEdit()
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

void PanelIntersectionEdit::valueChangedEvent()
{
}

}    // namespace joda::ui::qt
