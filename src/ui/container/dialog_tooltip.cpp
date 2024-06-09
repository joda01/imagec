///
/// \file      dialog_tooltip.cpp
/// \author    Joachim Danmayr
/// \date      2024-04-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "dialog_tooltip.hpp"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qgraphicseffect.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtmetamacros.h>
#include <QCoreApplication>
#include <QFile>
#include <QResource>
#include <QString>
#include <iostream>
#include <memory>
#include <thread>
#include <nlohmann/json.hpp>

namespace joda::ui::qt {

DialogToolTip::DialogToolTip(QWidget *windowMain, const QString &title, const QString &pathToHelpText) :
    DialogShadow(windowMain)
{
  setWindowTitle(title);
  setMaximumWidth(500);
  setModal(true);

  /////////////////////////////////////////////////////////////////
  auto *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(28, 28, 28, 28);

  QString helpText = "{}";
  QFile file(":/helptext/helptext/" + pathToHelpText);
  if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    helpText = file.readAll();
    file.close();
  }

  std::string citeStr;
  std::string sourceStr;
  std::string helpTextLong;

  try {
    nlohmann::json helpTextParsed = nlohmann::json::parse(helpText.toStdString());
    helpTextLong                  = std::string(helpTextParsed["textLong"]);

    try {
      for(const auto &cite : helpTextParsed["cite"]) {
        citeStr += "<a href=\"" + std::string(cite["href"]) + "\">" + std::string(cite["title"]) +
                   ""
                   "</a><br />";
      }
    } catch(...) {
    }

    try {
      for(const auto &cite : helpTextParsed["source"]) {
        sourceStr += "<a href=\"" + std::string(cite["href"]) + "\">" + std::string(cite["title"]) +
                     ""
                     "</a><br />";
      }
    } catch(...) {
    }
  } catch(...) {
  }

  QString fullHelpText = "<html><h2>" + title + "</h2>" + helpTextLong.data();

  if(!citeStr.empty()) {
    fullHelpText += QString("<hr><h2>Cite</h2>") + citeStr.data();
  }

  if(!sourceStr.empty()) {
    fullHelpText += QString("<hr><h2>Source</h2>") + sourceStr.data();
  }

  fullHelpText += "</html>";

  QLabel *helpTextLabel = new QLabel(fullHelpText);
  helpTextLabel->setOpenExternalLinks(true);
  helpTextLabel->setWordWrap(true);
  QFont fontLineEdit;
  fontLineEdit.setPixelSize(16);
  helpTextLabel->setFont(fontLineEdit);
  mainLayout->addWidget(helpTextLabel);
  mainLayout->addStretch();
  mainLayout->invalidate();
  mainLayout->activate();
  helpTextLabel->adjustSize();
  helpTextLabel->setMinimumHeight(helpTextLabel->height() + 56);
}

void DialogToolTip::onCloseClicked()
{
  close();
}

}    // namespace joda::ui::qt
