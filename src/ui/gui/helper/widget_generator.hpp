///
/// \file      widget_generator.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include "ui/gui/helper/debugging.hpp"

namespace joda::ui::gui {

inline std::tuple<QLabel *, QPushButton *, QHBoxLayout *> createHelpTextLabel(const QString &helpText, int32_t marginLeft = 12)
{
  static constexpr int32_t HELP_ICON_SIZE = 8;
  auto *hLayout                           = new QHBoxLayout();
  hLayout->setContentsMargins(0, 0, 0, 0);
  auto *helperText = new QLabel();
  helperText->setObjectName("functionHelperText");
  helperText->setText(helpText);
  helperText->setContentsMargins(marginLeft, 0, 0, 0);
  // Create a QFont instance
  QFont font;
  font.setPixelSize(12);
  font.setItalic(true);
  font.setBold(false);
  font.setWeight(QFont::Light);
  helperText->setFont(font);
  helperText->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
  hLayout->addWidget(helperText);

  // Info icon
  auto *mHelpButton = new QPushButton();
  mHelpButton->setObjectName("helpButton");
  mHelpButton->setStyleSheet(
      "QPushButton#helpButton {"
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 0px solid rgb(111, 121, 123);"
      "   color: rgb(0, 104, 117);"
      "   padding: 2px 2px 0px 0px;"
      "   margin: 0px 0px 0px 0px;"
      "   border-radius: 4px;"
      "   font-size: 8px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "   min-height: 0px;"
      "}");
  mHelpButton->setCursor(Qt::PointingHandCursor);
  mHelpButton->setIconSize({HELP_ICON_SIZE, HELP_ICON_SIZE});
  mHelpButton->setIcon(QIcon(":/icons/icons/icons8-info-48-fill.png"));
  CHECK_GUI_THREAD(mHelpButton)
  mHelpButton->setVisible(false);
  hLayout->addWidget(mHelpButton);
  hLayout->addStretch(0);
  return {helperText, mHelpButton, hLayout};
}
}    // namespace joda::ui::gui
