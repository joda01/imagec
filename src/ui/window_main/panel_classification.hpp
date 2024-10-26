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
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/helper/table_widget.hpp"

namespace joda::ui {

class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelClassification : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelClassification(joda::settings::ProjectSettings &settings, WindowMain *windowMain);
  void fromSettings(const joda::settings::ClusterClasses &settings);
  void toSettings();
  [[nodiscard]] auto getClustersAndClasses() const -> std::tuple<std::map<enums::ClusterIdIn, QString>, std::map<enums::ClassIdIn, QString>>;

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  static constexpr int COL_ID      = 0;
  static constexpr int COL_ID_ENUM = 1;
  static constexpr int COL_NAME    = 2;
  static constexpr int COL_COLOR   = 3;
  static constexpr int COL_NOTES   = 4;

  static constexpr int NR_OF_CLUSTERS = 15;
  static constexpr int NR_OF_CLASSES  = 15;
  /////////////////////////////////////////////////////
  void initTable();
  void loadTemplates();
  void updateTableLock(bool lock);

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  joda::settings::ProjectSettings &mSettings;
  PlaceholderTableWidget *mClusters;
  PlaceholderTableWidget *mClasses;

  /// TEMPLATE //////////////////////////////////////////////////
  QPushButton *mBookmarkButton;
  QComboBox *mTemplateSelection;

private slots:
  void onSettingChanged();
  void onloadPreset();
};
}    // namespace joda::ui
