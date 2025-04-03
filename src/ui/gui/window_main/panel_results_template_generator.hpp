///
/// \file      dialog_results_template_generator.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
class PanelResultsTemplateGenerator : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelResultsTemplateGenerator(WindowMain *mainWindow, joda::settings::AnalyzeSettings *analyzeSettings);
  void refreshView();

private:
  /////////////////////////////////////////////////////
  WindowMain *mMainWindow;
  joda::settings::AnalyzeSettings *mAnalyzeSettings;
  PlaceholderTableWidget *mColumns;
  DialogColumnSettings *mColumnEditDialog;
  int32_t mSelectedTableRow = -1;
};

}    // namespace joda::ui::gui
