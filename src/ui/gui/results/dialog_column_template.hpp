///
/// \file      dialog_column_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/results_settings/results_settings.hpp"
#include "backend/settings/results_settings/results_template.hpp"

class QComboBoxMulti;

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
class DialogColumnTemplate : public QDialog
{
  Q_OBJECT
public:
  /////////////////////////////////////////////////////
  DialogColumnTemplate(settings::ResultsTemplate *templateSettings, QWidget *parent);
  int exec() override;

private:
  auto toTemplateSettings() const -> settings::ResultsTemplate;

  /////////////////////////////////////////////////////
  bool accept                                  = false;
  settings::ResultsTemplate *mTemplateSettings = nullptr;

  struct Column
  {
    QComboBoxMulti *mMeasurementSelector;
    QComboBoxMulti *mStatsSelector;
  };
  std::vector<Column> mColumns;
};

}    // namespace joda::ui::gui
