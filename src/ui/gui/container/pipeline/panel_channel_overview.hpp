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

#include <QtWidgets>
#include <memory>

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;

class PanelChannelOverview : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelChannelOverview(WindowMain *, PanelPipelineSettings *);

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t HANDLE_WITH = 8;
  /////////////////////////////////////////////////////
  void startDrag();
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  PanelPipelineSettings *mParentContainer;
  WindowMain *mWindowMain;
};

}    // namespace joda::ui::gui
