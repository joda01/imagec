///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
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
  [[nodiscard]] const PanelPipelineSettings *getParentContainer() const
  {
    return mParentContainer;
  }
  [[nodiscard]] PanelPipelineSettings *mutableParentContainer() const
  {
    return mParentContainer;
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t HANDLE_WITH = 8;
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  PanelPipelineSettings *mParentContainer;
  WindowMain *mWindowMain;
};

}    // namespace joda::ui::gui
