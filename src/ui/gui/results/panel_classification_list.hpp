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
#include <qdockwidget.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <optional>
#include <utility>
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"

namespace joda::db {
class Database;
}

namespace joda::settings {
class ResultsSettings;
}

namespace joda::ui::gui {
class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelClassificationList : public QDockWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelClassificationList(WindowMain *windowMain, settings::ResultsSettings *resultsSettings);
  void setDatabase(joda::db::Database *database);

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  static constexpr int COL_ID      = 0;
  static constexpr int COL_ID_ENUM = 1;
  static constexpr int COL_NAME    = 2;
  static constexpr int COL_COLOR   = 3;
  static constexpr int COL_NOTES   = 4;

  /////////////////////////////////////////////////////
  void loadClasses();
  void openEditDialog(int row, int column);
  void createTableItem(int32_t rowIdx, enums::ClassId classId, const std::string &name, const std::string &color, const std::string &notes);

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  settings::ResultsSettings *mResultsSettings;
  joda::db::Database *mDatabase = nullptr;
  PlaceholderTableWidget *mClasses;
  std::list<joda::settings::Class> mClassesList;

  /// DIALOG //////////////////////////////////////////////////
  DialogClassSettings *mClassSettingsDialog;

  void onSettingChanged();
};
}    // namespace joda::ui::gui
