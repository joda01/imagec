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

namespace joda::db {
class QueryFilter;
class Database;
};    // namespace joda::db

namespace joda::ui {

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
  DialogColumnSettings(db::QueryFilter *filter, QWidget *parent);
  void exec(int32_t selectedColumn);
  void updateClustersAndClasses(db::Database *);
  auto getClusterAndClassFromCombo() const -> std::pair<std::string, std::string>;

private:
  /////////////////////////////////////////////////////
  int32_t mSelectedColumn = -1;
  bool accept             = false;

  db::Database *mDatabase  = nullptr;
  db::QueryFilter *mFilter = nullptr;

  QComboBox *mClusterClassSelector;
  QComboBox *mMeasurementSelector;
  QComboBox *mStatsSelector;
  QComboBox *mCrossChannelStackC;
  QSpinBox *mZStack;
  QSpinBox *mTStack;

private slots:
  void onClusterAndClassesChanged();
};

}    // namespace joda::ui
