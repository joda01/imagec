///
/// \file      dialog_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include "backend/settings/experiment_settings.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class WindowMain;

class DialogExperimentSettings : public DialogShadow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogExperimentSettings(WindowMain *windowMain, joda::settings::ExperimentSettings &settings);

  int exec() override;

private:
  /////////////////////////////////////////////////////
  void fromSettings();
  void toSettings();

  void createLayout();
  void createScientistGroupBox();
  void createExperimentGroupBox();

  /////////////////////////////////////////////////////
  WindowMain *mParentWindow;
  joda::settings::ExperimentSettings &mSettings;

  QComboBox *mGroupByComboBox;

  /////////////////////////////////////////////////////
  static constexpr int32_t NR_OF_SCIENTISTS = 1;

  QGroupBox *mScientistsGroup;
  QLineEdit *mAddressOrganisation;
  std::vector<QLineEdit *> mScientists{NR_OF_SCIENTISTS};

  QGroupBox *mExperimentGroup;
  QLineEdit *mWorkingDir;

  QLabel *mWellOrderMatrixLabel;
  QLineEdit *mWellOrderMatrix;

  QLabel *mRegexToFindTheWellPositionLabel;
  QComboBox *mRegexToFindTheWellPosition;

  QLineEdit *mTestFileName;
  QLabel *mTestFileNameLabel;

  QLabel *mTestFileResult;
  QLabel *mTestFileResultLabel;

  QTextEdit *mNotes;

private slots:
  void onGroupByChanged();
  void onOpenWorkingDirectoryClicked();
  void onOkayClicked();
  void onCancelClicked();
  void applyRegex();
};

}    // namespace joda::ui::qt
