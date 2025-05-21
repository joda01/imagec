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
#include "ui/gui/helper/table_widget.hpp"

namespace joda::settings {
class PipelineHistoryEntry;
}    // namespace joda::settings

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;
class TimeHistoryEntry;

///
/// \class      Dialog History
/// \author
/// \brief
///
class DialogHistory : public QDialog
{
public:
  /////////////////////////////////////////////////////
  DialogHistory(WindowMain *parent, PanelPipelineSettings *panelPipelineSettings);

  void show();
  void updateHistory(enums::HistoryCategory category, const std::string &);
  void loadHistory();
  void createTag();
  void undo();
  static auto generateHistoryEntry(const std::optional<joda::settings::PipelineHistoryEntry> &) -> TimeHistoryEntry *;

private:
  /////////////////////////////////////////////////////
  void updateSelection();
  void restoreHistory(int32_t index);

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain               = nullptr;
  PanelPipelineSettings *mPanelPipeline = nullptr;

  PlaceholderTableWidget *mHistory;
};

}    // namespace joda::ui::gui
