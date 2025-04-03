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
  void loadTemplates();

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

  [[nodiscard]] int32_t getImageSeries() const
  {
    return static_cast<int32_t>(mImageSeries->currentData().toInt());
  }

  void generateNewJobName()
  {
    mJobName->setPlaceholderText(joda::helper::RandomNameGenerator::GetRandomName().data());
  }

signals:
  void updateImagePreview();

private:
  /////////////////////////////////////////////////////
  bool askForChangeTemplateIndex();
  /////////////////////////////////////////////////////
  joda::settings::AnalyzeSettings &mSettings;
  WindowMain *mParentWindow;

  /////////////////////////////////////////////////////
  QLineEdit *mAddressOrganisation;
  QLineEdit *mJobName;
  QLineEdit *mScientistsFirstName;
  QLineEdit *mExperimentName;
  QLineEdit *mExperimentId;

  QComboBox *mGroupByComboBox;
  QComboBox *mPlateSize;

  QComboBox *mImageSeries;
  QComboBox *mStackHandlingZ;
  QComboBox *mStackHandlingT;
  QComboBox *mCompositeTileSize;

  QComboBox *mTemplateSelection;
  QPushButton *mTemplateBookmarkButton;
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
  void onOpenTemplate();
};

}    // namespace joda::ui::gui
