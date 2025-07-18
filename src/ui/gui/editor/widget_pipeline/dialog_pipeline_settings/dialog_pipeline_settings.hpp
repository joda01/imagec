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
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "backend/enums/types.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_classes_out.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_spinbox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_text_edit.hpp"

namespace joda::ui::gui {

class WindowMain;

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DialogPipelineSettings : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogPipelineSettings(const joda::settings::Classification &classes, joda::settings::Pipeline &settings, WindowMain *parent);

private:
  void accept() override;
  void fromSettings();

  /////////////////////////////////////////////////////
  joda::settings::Pipeline &mSettings;

  QLineEdit *mPipelineName;
  QTextEdit *mPipelineNotes;

  QComboBox *mClass;
  QComboBox *mCStackIndex;
  QComboBox *zProjection;
  QLineEdit *zStackIndex;
  QLineEdit *tStackIndex;
  QComboBox *defaultClassId;
};
}    // namespace joda::ui::gui
