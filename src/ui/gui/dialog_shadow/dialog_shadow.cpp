///
/// \file      dialog_shadow.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_shadow.h"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <QtGui>

namespace joda::ui::gui {

DialogShadow::DialogShadow(QWidget *parent, bool showClose, const QString &closeButtonText) :
    QDialog(parent), mShowCloseButton(showClose), mCloseButtonString(closeButtonText)
{
  setModal(true);
}

int DialogShadow::exec()
{
  if(mShowCloseButton) {
    QWidget *buttons   = new QWidget();
    QHBoxLayout *hBox  = new QHBoxLayout(buttons);
    QPushButton *close = new QPushButton(mCloseButtonString, buttons);
    close->setCursor(Qt::PointingHandCursor);
    close->setObjectName("closeButton");

    connect(close, &QPushButton::clicked, this, &DialogShadow::onCloseWindow);

    hBox->addStretch();
    hBox->addWidget(close);
    QLayout *mainL = layout();
    mainL->addWidget(buttons);
  }
  return QDialog::exec();
}

void DialogShadow::onCloseWindow()
{
  close();
}

}    // namespace joda::ui::gui
