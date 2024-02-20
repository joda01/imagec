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

#include <QtWidgets>
#include "panel_function.hpp"

namespace joda::ui::qt {

class WindowMain;

class PanelChannelOverview : public QWidget
{
public:
  PanelChannelOverview(WindowMain *);

private:
  void mousePressEvent(QMouseEvent *event) override;
  WindowMain *mWindowMain;
};

}    // namespace joda::ui::qt
