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
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include "backend/settings/experiment_settings.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class DialogExperimentSettings : public DialogShadow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogExperimentSettings(QWidget *windowMain, joda::settings::ExperimentSettings &settings);

  int exec() override;

private:
  /////////////////////////////////////////////////////
  void fromSettings();
  void toSettings();

  /////////////////////////////////////////////////////
  joda::settings::ExperimentSettings &mSettings;

  QComboBox *mGroupedHeatmapOnOff;
  QComboBox *mWellHeatmapOnOff;
  QComboBox *mImageHeatmapOnOff;
  QLineEdit *mWellOrderMatrix;

  QLineEdit *mHeatmapSlice;
  QComboBox *mGroupByComboBox;
  QComboBox *mRegexToFindTheWellPosition;
  QLineEdit *mTestFileName;
  QLabel *mTestFileResult;

private slots:
  void onOkayClicked();
  void onCancelClicked();
  void applyRegex();
};

}    // namespace joda::ui::qt
