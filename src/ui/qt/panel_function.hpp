#pragma once

#include <qlabel.h>
#include <qwidget.h>
#include <QtWidgets>
#include "panel_function.hpp"

namespace joda::ui::qt {

class PanelFunction : public QWidget
{
public:
  PanelFunction(const QString &icon, const QString &placeHolder, const QString &helpText, QWidget *parent = nullptr) :
      QWidget(parent)
  {
    setObjectName("panelFunction");
    setStyleSheet(
        "QLineEdit { border-radius: 4px; border: 1px solid rgba(32, 27, 23, 0.6); padding-top: 10px; padding-bottom: "
        "10px;}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    setAcceptDrops(true);
    // setMaximumHeight(80);
    // setMinimumHeight(80);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 0);

    const QIcon passwordIcon(":/icons/" + icon);

    QLineEdit *_lineEdit = new QLineEdit();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    _lineEdit->setFont(fontLineEdit);
    _lineEdit->setClearButtonEnabled(true);
    _lineEdit->addAction(passwordIcon, QLineEdit::LeadingPosition);
    _lineEdit->setPlaceholderText(placeHolder);
    QIntValidator *validator = new QIntValidator(0, 65000, _lineEdit);
    _lineEdit->setValidator(validator);
    layout->addWidget(_lineEdit);

    auto *helperText = new QLabel();
    helperText->setObjectName("functionHelperText");
    helperText->setText(helpText);
    helperText->setContentsMargins(12, 0, 0, 0);
    // Create a QFont instance
    QFont font;
    font.setPixelSize(12);
    font.setItalic(true);
    font.setBold(false);
    font.setWeight(QFont::Light);
    helperText->setFont(font);
    helperText->setStyleSheet("QLabel#functionHelperText { color : #808080; }");

    layout->addWidget(helperText);

    setLayout(layout);
  }

protected:
};
}    // namespace joda::ui::qt
