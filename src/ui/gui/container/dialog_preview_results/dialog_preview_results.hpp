///
/// \file      dialog_pipeline_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qtableview.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/enums/types.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "controller/controller.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/container/setting/setting_spinbox.hpp"
#include "ui/gui/container/setting/setting_text_edit.hpp"

namespace joda::ui::gui {

class WindowMain;
class TableModelPreviewResult;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DialogPreviewResults : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogPreviewResults(const joda::settings::Classification &classes, const joda::ctrl::Preview::PreviewResults *results, WindowMain *windowMain);
  void setResults(const joda::ctrl::Preview::PreviewResults &);
  void refresh();

private:
  /////////////////////////////////////////////////////
  QTableView *mResultsTable;
  TableModelPreviewResult *mTableModel = nullptr;
};
}    // namespace joda::ui::gui
