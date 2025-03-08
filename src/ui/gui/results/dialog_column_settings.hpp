///
/// \file      dialog_column_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include "backend/settings/results_settings/results_settings.hpp"

namespace joda::db {
class QueryFilter;
class Database;
};    // namespace joda::db
namespace joda::settings {
class AnalyzeSettings;
}

namespace joda::ui::gui {

///
/// \class
/// \author
/// \brief
///
class DialogColumnSettings : public QDialog
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  DialogColumnSettings(settings::ResultsSettings *filter, QWidget *parent);
  void exec(int32_t selectedColumn);
  void updateClassesAndClasses(db::Database *);
  void updateClassesAndClasses(const joda::settings::AnalyzeSettings &settings);
  auto getClasssFromCombo() const -> std::pair<std::string, std::string>;

private:
  void checkForIntersecting();
  /////////////////////////////////////////////////////
  int32_t mSelectedColumn = -1;
  bool accept             = false;

  db::Database *mDatabase            = nullptr;
  settings::ResultsSettings *mFilter = nullptr;

  QComboBox *mClasssClassSelector;
  QComboBox *mClasssIntersection;
  QComboBox *mMeasurementSelector;
  QComboBox *mStatsSelector;
  QComboBox *mCrossChannelStackC;
  QSpinBox *mZStack;
  QSpinBox *mTStack;

private slots:
  void onClassesChanged();
};

}    // namespace joda::ui::gui
