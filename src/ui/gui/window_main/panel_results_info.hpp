///
/// \file      panel_image.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

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
class DialogResultsTemplateGenerator;

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

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  PlaceholderTableWidget *mLastLoadedResults;
  DialogResultsTemplateGenerator *mResultsTemplate;
  std::set<std::string> mAddedPaths;

private slots:
  void filterResults();
};
}    // namespace joda::ui::gui
