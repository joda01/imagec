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
#include <mutex>
#include "ui/gui/helper/layout_generator.hpp"

namespace joda::ui::gui {

class WindowMain;
class ContainerBase;

///
/// \class    PanelEdit
/// \author   Joachim Danmayr
/// \brief
///
class PanelEdit : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelEdit(WindowMain *wm, ContainerBase *containerBase = nullptr, bool withExtraButtons = true);

  WindowMain *getWindowMain()
  {
    return mWindowMain;
  }

protected:
  helper::LayoutGenerator &layout()
  {
    return mLayout;
  }

private:
  /////////////////////////////////////////////////////
  virtual void valueChangedEvent() = 0;
  WindowMain *mWindowMain;
  helper::LayoutGenerator mLayout;
  ContainerBase *mContainerBase = nullptr;

public slots:
  void onSaveAsTemplate();
  void onCopyChannel();
  void onValueChanged();
};
}    // namespace joda::ui::gui
