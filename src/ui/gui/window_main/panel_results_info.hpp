///
/// \file      panel_image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <optional>
#include <utility>
#include "backend/helper/database/database.hpp"
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/container/container_base.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::ui::gui {

class WindowMain;
class PanelResultsTemplateGenerator;

///
/// \class
/// \author
/// \brief
///
class PanelResultsInfo : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelResultsInfo(WindowMain *windowMain);
  void addResultsFileToHistory(const std::filesystem::path &dbFile, const std::string &jobName, const std::chrono::system_clock::time_point &time);
  void clearHistory();
  void fromSettings(const joda::settings::AnalyzeSettings &settings);
  void refreshTableView();

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PlaceholderTableWidget *mLastLoadedResults;
  PanelResultsTemplateGenerator *mResultsTemplate;
  std::set<std::string> mAddedPaths;

private slots:
  void filterResults();
};
}    // namespace joda::ui::gui
