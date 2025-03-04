///
/// \file      dialog_results_template_generator.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qdialog.h>
#include <qtablewidget.h>
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableWidget;

namespace joda::db {
class QueryFilter;
}

namespace joda::ui::gui {

class WindowMain;

///
/// \class
/// \author
/// \brief
///
class DialogResultsTemplateGenerator : public QDialog
{
public:
  /////////////////////////////////////////////////////
  DialogResultsTemplateGenerator(WindowMain *mainWindow, db::QueryFilter *filter);

private:
  /////////////////////////////////////////////////////
  WindowMain *mMainWindow;
  db::QueryFilter *mFilter;
  helper::LayoutGenerator mLayout;

  PlaceholderTableWidget *mCommands;
};

}    // namespace joda::ui::gui
