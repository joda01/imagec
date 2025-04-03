///
/// \file      color_item.cpp
/// \author    Joachim Danmayr
/// \date      2024-11-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "color_item.hpp"

ColorRectDelegate::ColorRectDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void ColorRectDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  // Fetch the item's color from data
  QVariant colorVariant = index.data(Qt::BackgroundRole);
  QColor color          = colorVariant.isValid() ? colorVariant.value<QColor>() : Qt::black;

  // Draw the background and focus rectangle if needed
  painter->save();
  if(option.state & QStyle::State_Selected) {
    painter->fillRect(option.rect, option.palette.highlight());
  }

  // Define the rectangle's properties
  QRect rect = option.rect.adjusted(5, 9, -5, -5);    // Slight padding
  QSize rectSize(8, 8);                               // Width 20, slightly shorter height
  QRect colorRect(rect.topLeft(), rectSize);

  // Draw the rectangle
  painter->setBrush(color);
  painter->setPen(Qt::NoPen);
  painter->drawRect(colorRect);

  // Draw the text
  QRect textRect = option.rect;
  textRect.setLeft(colorRect.right() + 5);    // Leave space for the rectangle
  painter->setPen(option.palette.text().color());
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

  painter->restore();
}

QSize ColorRectDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  // Ensure enough space for the rectangle and text
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  size.setWidth(size.width() + 25);     // Add space for the rectangle
  size.setHeight(size.height() + 8);    // Add space for the rectangle
  return size;
}
