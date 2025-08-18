///
/// \file      colord_square_delegate.hpp
/// \author    Joachim Danmayr
/// \date      2024-11-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "colord_square_delegate.hpp"
#include <qtablewidget.h>
#include <iostream>
#include <string>

namespace joda::ui::gui {

ColoredSquareDelegate::ColoredSquareDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

ColoredSquareDelegate::~ColoredSquareDelegate()
{
}

void ColoredSquareDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  // Draw the background
  // painter->save();
  // painter->fillRect(opt.rect, opt.palette.color(QPalette::Base));
  // painter->restore();

  QString color;
  auto *tbl = dynamic_cast<QTableWidget *>(parent());
  if(parent() != nullptr && tbl->columnCount() > 3) {
    color = tbl->item(index.row(), 3)->text();    // 3 is the column containg the color
  }

  // Draw the colored square
  QRect squareRect = opt.rect;
  squareRect.setWidth(8);
  squareRect.setHeight(8);
  squareRect.moveTop(opt.rect.y() + opt.rect.height() / 2 - 4);
  squareRect.moveLeft(opt.rect.left() + 5);
  painter->fillRect(squareRect, QColor(color));    // Change the color as needed

  // Draw the text
  opt.rect.setLeft(squareRect.right() + 5);
  QStyledItemDelegate::paint(painter, opt, index);
}

}    // namespace joda::ui::gui
