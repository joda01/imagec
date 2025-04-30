///
/// \file      dialog_class_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-30
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgridlayout.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <string>
#include <vector>
#include "backend/enums/enum_measurements.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class DialogClassSettings : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogClassSettings(QWidget *parent);
  int exec(joda::settings::Class &);

private:
  /////////////////////////////////////////////////////
  void fromSettings(const joda::settings::Class &);
  void toSettings(joda::settings::Class &);
  std::map<enums::Measurement, std::pair<QPushButton *, std::vector<enums::Stats>>> mMeasurements;

  /////////////////////////////////////////////////////
  int32_t mOkayPressed = -1;
  QComboBox *mDialogClassName;
  ColorComboBox *mDialogColorCombo;

private slots:
  void onOkayPressed();
};

}    // namespace joda::ui::gui
