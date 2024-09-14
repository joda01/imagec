///
/// \file      dialog_shadow.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgraphicseffect.h>
#include <qlabel.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui {

class DialogShadow : public QDialog
{
  Q_OBJECT

public:
  DialogShadow(QWidget *parent, bool showClose = true, const QString &closeButtonText = "Close");
  int exec() override;

private:
  /////////////////////////////////////////////////////
  void showEvent(QShowEvent *event) override
  {
    if(nullptr != dimmer) {
      QDialog::showEvent(event);
      dimmer->show();
    }
  }

  void hideEvent(QHideEvent *event) override
  {
    if(nullptr != dimmer) {
      QDialog::hideEvent(event);
      dimmer->hide();
    }
  }

  QWidget *dimmer = nullptr;
  QString mCloseButtonString;
  bool mShowCloseButton = true;

private slots:
  void onCloseWindow();
};
}    // namespace joda::ui
