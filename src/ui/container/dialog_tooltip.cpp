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
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

DialogToolTip::DialogToolTip(QWidget *windowMain, const QString &title, const QString &pathToHelpText) :
    QDialog(windowMain)
{
  setWindowTitle(title);
  setMaximumWidth(500);
  setModal(true);
  setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Dialog);

  auto *mainLayout = new QVBoxLayout(this);

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

  // QPushButton *close = new QPushButton("Close");
  // connect(close, &QPushButton::clicked, this, &DialogToolTip::onCloseClicked);
  // mainLayout->addWidget(close);

  const int radius = 12;
  setStyleSheet(QString("QDialog { "
                        "border-radius: %1px; "
                        "border: 2px solid palette(shadow); "
                        "background-color: palette(base); "
                        "}")
                    .arg(radius));

  // The effect will not be actually visible outside the rounded window,
  // but it does help get rid of the pixelated rounded corners.
  QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect();
  // The color should match the border color set in CSS.
  effect->setColor(QApplication::palette().color(QPalette::Shadow));
  effect->setBlurRadius(8);
  setGraphicsEffect(effect);

  // Need to show the box before we can get its proper dimensions.
  show();

  // Here we draw the mask to cover the "cut off" corners, otherwise they show through.
  // The mask is sized based on the current window geometry. If the window were resizable (somehow)
  // then the mask would need to be set in resizeEvent().
  const QRect rect(QPoint(0, 0), geometry().size());
  QBitmap b(rect.size());
  b.fill(QColor(Qt::color0));
  QPainter painter(&b);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(Qt::color1);
  // this radius should match the CSS radius
  painter.drawRoundedRect(rect, radius, radius, Qt::AbsoluteSize);
  painter.end();
  setMask(b);
  // <--
}

void DialogToolTip::onCloseClicked()
{
  close();
}

}    // namespace joda::ui::qt
