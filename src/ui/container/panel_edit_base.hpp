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
/// \brief     A short description what happens here.
///

#pragma once

#include <qtmetamacros.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "controller/controller.hpp"
#include "ui/helper/waitingspinnerwidget.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt {

class WindowMain;

class PanelEdit : public QWidget
{
  Q_OBJECT

public:
  explicit PanelEdit(WindowMain *wm) : mWindowMain(wm)
  {
  }

  WindowMain *getWindowMain()
  {
    return mWindowMain;
  }

private:
  virtual void valueChangedEvent() = 0;
  WindowMain *mWindowMain;

public slots:
  void onValueChanged();
};
}    // namespace joda::ui::qt
