///
/// \file      panel_project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-06
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
#include <qwidget.h>
#include <QtWidgets>
#include "backend/settings/experiment_settings.hpp"

namespace joda::ui::qt {

class WindowMain;

///
/// \class      PanelProjectSettings
/// \author     Joachim Danmayr
/// \brief
///
class PanelProjectSettings : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  PanelProjectSettings(joda::settings::ExperimentSettings &settings, WindowMain *parentWindow);
  void fromSettings(joda::settings::ExperimentSettings &settings);
  void toSettings();

private:
  /////////////////////////////////////////////////////
  joda::settings::ExperimentSettings &mSettings;
  WindowMain *mParentWindow;

  /////////////////////////////////////////////////////
  static constexpr int32_t NR_OF_SCIENTISTS = 1;
  QLineEdit *mAddressOrganisation;
  std::vector<QLineEdit *> mScientists{NR_OF_SCIENTISTS};

  QComboBox *mGroupByComboBox;
  QComboBox *mPlateSize;
  QLineEdit *mWorkingDir;
  QLabel *mWellOrderMatrixLabel;
  QLineEdit *mWellOrderMatrix;
  QLabel *mRegexToFindTheWellPositionLabel;
  QComboBox *mRegexToFindTheWellPosition;
  QLineEdit *mTestFileName;
  QLabel *mTestFileNameLabel;
  QLabel *mTestFileResult;
  QTextEdit *mNotes;

private slots:
  void applyRegex();
  void onOpenWorkingDirectoryClicked();
  void onSettingChanged();
};

}    // namespace joda::ui::qt