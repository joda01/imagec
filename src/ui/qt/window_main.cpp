///
/// \file      window_main.cpp
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

#include "window_main.hpp"
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>

namespace joda::ui::qt {

WindowMain::WindowMain()
{
  setWindowTitle("imageC");
  auto *toolbar = addToolBar("toolbar");

  // Create an action with an icon
  QAction *myAction = new QAction(QIcon(":/icons/opened_folder_20.png"), "My Action", this);
  myAction->setToolTip("Tooltip for My Action");

  // Connect the action to a slot or function if needed
  // connect(myAction, &QAction::triggered, this, &WindowMain::onMyActionTriggered);

  // Add the action to the toolbar
  toolbar->addAction(myAction);
}

}    // namespace joda::ui::qt
