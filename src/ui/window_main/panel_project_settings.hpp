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

///

#pragma once

#include <qcombobox.h>
#include <qwidget.h>
#include <QtWidgets>
#include "backend/helper/random_name_generator.hpp"
#include "backend/settings/project_settings/project_settings.hpp"

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
  PanelProjectSettings(joda::settings::ProjectSettings &settings, WindowMain *parentWindow);
  void fromSettings(const joda::settings::ProjectSettings &settings);
  void toSettings();
  [[nodiscard]] QString getJobName() const
  {
    if(mJobName->text().isEmpty()) {
      return mJobName->placeholderText();
    }
    return mJobName->text();
  }
  void generateNewJobName()
  {
    mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
  }

private:
  /////////////////////////////////////////////////////
  joda::settings::ProjectSettings &mSettings;
  WindowMain *mParentWindow;

  /////////////////////////////////////////////////////
  QLineEdit *mAddressOrganisation;
  QLineEdit *mJobName;
  QLineEdit *mScientistsFirstName;

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
