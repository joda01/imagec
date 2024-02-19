#pragma once

#include <qwidget.h>
#include <QtWidgets>
#include "panel_function.hpp"

namespace joda::ui::qt {

class PanelFunction : public QWidget
{
public:
  PanelFunction(QWidget *parent = nullptr) : QWidget(parent)
  {
    // setStyleSheet("QLineEdit { border-radius: 5px; padding-top: 10px; padding-bottom: 10px;}");
    setAcceptDrops(true);
    // setMaximumHeight(80);
    // setMinimumHeight(80);
    QVBoxLayout *layout = new QVBoxLayout(this);

    const QIcon passwordIcon(":/icons/save_20.png");

    QLineEdit *_lineEdit = new QLineEdit();
    _lineEdit->setClearButtonEnabled(true);
    _lineEdit->addAction(passwordIcon, QLineEdit::LeadingPosition);
    _lineEdit->setPlaceholderText("Name");
    layout->addWidget(_lineEdit);

    setLayout(layout);
  }

protected:
};
}    // namespace joda::ui::qt
