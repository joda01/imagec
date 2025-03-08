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
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/table_widget.hpp"

namespace joda::ui::gui {

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
  void fromSettings(const joda::settings::Classification &settings);
  void toSettings();
  [[nodiscard]] auto getClasses() const -> std::map<enums::ClassIdIn, QString>;

signals:
  void settingsChanged();

private:
  /////////////////////////////////////////////////////
  static constexpr int COL_ID      = 0;
  static constexpr int COL_ID_ENUM = 1;
  static constexpr int COL_NAME    = 2;
  static constexpr int COL_COLOR   = 3;
  static constexpr int COL_NOTES   = 4;

  static constexpr int NR_OF_CLASSES = 30;
  /////////////////////////////////////////////////////
  void createDialog();
  void initTable();
  void loadTemplates();
  void updateTableLock(bool lock);
  void newTemplate();
  void saveAsNewTemplate();
  void openEditDialog(int row, int column);
  void populateClassesFromImage();

  /////////////////////////////////////////////////////
  WindowMain *mWindowMain;
  joda::settings::ProjectSettings &mSettings;
  PlaceholderTableWidget *mClasses;

  /// DIALOG //////////////////////////////////////////////////
  QDialog *mEditDialog;
  QComboBox *mDialogClassName;
  ColorComboBox *mDialogColorCombo;
  int32_t mSelectedRow = 0;

  /// TEMPLATE //////////////////////////////////////////////////
  bool askForChangeTemplateIndex();
  int32_t mActSelectedIndex = 0;
  bool mDontAsk             = false;
  bool mIsLocked            = false;
  QPushButton *mBookmarkButton;
  QComboBox *mTemplateSelection;

private slots:
  void onSettingChanged();
  void onloadPreset(int index);
  void onOkayPressed();
};
}    // namespace joda::ui::gui
