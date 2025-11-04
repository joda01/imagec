///
/// \file      dialog_history.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-25
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
#include <qlabel.h>
#include "backend/enums/enum_history.hpp"
#include "ui/gui/helper/table_view.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::settings {
class PipelineHistoryEntry;
class Pipeline;
}    // namespace joda::settings

namespace joda::ui::gui {

class WindowMain;
class TimeHistoryEntry;
class TableModelHistory;

///
/// \class      Dialog History
/// \author
/// \brief
///
class DialogHistory : public QDialog
{
public:
  /////////////////////////////////////////////////////
  DialogHistory(WindowMain *parent, joda::settings::Pipeline *panelPipelineSettings);
  void show();

private:
  /////////////////////////////////////////////////////
  void createTag();

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain = nullptr;
  TableModelHistory *mTableModelHistory;
  PlaceholderTableView *mTableHistory;
  joda::settings::Pipeline *mPipelineSettings;
};

}    // namespace joda::ui::gui
