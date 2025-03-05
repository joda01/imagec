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
#include <qwidget.h>
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/helper/layout_generator.hpp"

class PlaceholderTableWidget;

namespace joda::db {
class QueryFilter;
}

namespace joda::ui::gui {

class WindowMain;
class DialogColumnSettings;

///
/// \class
/// \author
/// \brief
///
class DialogResultsTemplateGenerator : public QWidget
{
public:
  /////////////////////////////////////////////////////
  DialogResultsTemplateGenerator(WindowMain *mainWindow, joda::settings::AnalyzeSettings *analyzeSettings);
  // int32_t exec() override;

private:
  /////////////////////////////////////////////////////
  void refreshView();

  WindowMain *mMainWindow;
  joda::settings::AnalyzeSettings *mAnalyzeSettings;
  PlaceholderTableWidget *mCommands;
  DialogColumnSettings *mColumnEditDialog;
  int32_t mSelectedTableRow = -1;
};

}    // namespace joda::ui::gui
