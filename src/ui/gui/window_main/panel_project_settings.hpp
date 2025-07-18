///
/// \file      panel_project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-06
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
#include "backend/helper/random_name_generator.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_settings.hpp"
#include "ui/gui/helper/combo_placeholder.hpp"

namespace joda::ui::gui {

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
  PanelProjectSettings(joda::settings::AnalyzeSettings &settings, WindowMain *parentWindow);
  void fromSettings(const joda::settings::AnalyzeSettings &settings);
  void toSettings();

  [[nodiscard]] QString getJobName() const
  {
    if(mJobName->text().isEmpty()) {
      return mJobName->placeholderText();
    }
    return mJobName->text();
  }

  [[nodiscard]] QString getExperimentName() const
  {
    if(mExperimentName->text().isEmpty()) {
      return mExperimentName->placeholderText();
    }
    return mExperimentName->text();
  }

  [[nodiscard]] QString getExperimentId() const
  {
    if(mExperimentId->text().isEmpty()) {
      return mExperimentId->placeholderText();
    }
    return mExperimentId->text();
  }

  void generateNewJobName()
  {
    mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
  }

signals:
  void updateImagePreview();

private:
  /////////////////////////////////////////////////////
  joda::settings::AnalyzeSettings &mSettings;
  WindowMain *mParentWindow;

  // META ///////////////////////////////////////////////////
  QDialog *mMetaEditDialog;
  QLineEdit *mJobName;
  QLineEdit *mAddressOrganisation;
  QLineEdit *mScientistsFirstName;
  QLineEdit *mExperimentName;
  QLineEdit *mExperimentId;

  // GROUPING ///////////////////////////////////////////////////
  QDialog *mGroupingDialog;
  QPushButton *mOpenGroupingSettings;
  ComboWithPlaceholder *mGroupByComboBox;
  QComboBox *mPlateSize;

  QComboBox *mStackHandlingZ;
  QComboBox *mStackHandlingT;

  QLineEdit *mTStackFrameStart;
  QLineEdit *mTStackFrameEnd;

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

}    // namespace joda::ui::gui
