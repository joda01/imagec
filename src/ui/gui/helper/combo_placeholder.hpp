///
/// \file      combo_placeholder.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qapplication.h>
#include <QComboBox>
#include <QPainter>
#include <QStyledItemDelegate>

class ComboWithPlaceholder : public QComboBox
{
public:
  ComboWithPlaceholder(QWidget *parent = nullptr) : QComboBox(parent)
  {
  }

  void setPlaceholderText(const QString &text)
  {
    m_placeholder = text;
    update();
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    QComboBox::paintEvent(event);

    if(currentIndex() < 0 && !m_placeholder.isEmpty()) {
      QStyleOptionComboBox opt;
      initStyleOption(&opt);
      QPainter painter(this);
      opt.currentText = m_placeholder;
      style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &painter, this);
      opt.palette.setColor(QPalette::ButtonText, Qt::gray);
      style()->drawControl(QStyle::CE_ComboBoxLabel, &opt, &painter, this);
    }
  }

private:
  QString m_placeholder;
};
