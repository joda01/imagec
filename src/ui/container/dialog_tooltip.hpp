#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include "backend/settings/channel/channel_reporting_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class DialogToolTip : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogToolTip(QWidget *windowMain, const QString &title, const QString &helpText);

private:
private slots:
  void onCloseClicked();
};
}    // namespace joda::ui::qt
