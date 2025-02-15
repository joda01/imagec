#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include "ui/gui/dialog_shadow/dialog_shadow.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

class DialogToolTip : public DialogShadow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogToolTip(QWidget *windowMain, const QString &title, const QString &helpText);

private slots:
  void onCloseClicked();
};
}    // namespace joda::ui::gui
