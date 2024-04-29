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

#include "panel_giraf_edit.hpp"
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
#include "../../window_main.hpp"
#include "../container_function.hpp"
#include "../dialog_channel_measurment.hpp"
#include "backend/settings/detection/detection_settings.hpp"
#include "container_giraf.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;

PanelGirafEdit::PanelGirafEdit(WindowMain *wm, ContainerGiraf *parentContainer) :
    mWindowMain(wm), mParentContainer(parentContainer)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelGirafEdit");
}

void PanelGirafEdit::init()
{
}

PanelGirafEdit::~PanelGirafEdit()
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

QLabel *PanelGirafEdit::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

}    // namespace joda::ui::qt
