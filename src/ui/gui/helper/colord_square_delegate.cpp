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
#include "ui/gui/helper/item_data_roles.hpp"
#include "ui/gui/helper/table_view.hpp"

namespace joda::ui::gui {

ColoredSquareDelegate::ColoredSquareDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

ColoredSquareDelegate::~ColoredSquareDelegate()
{
}

void ColoredSquareDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  painter->save();

  // Draw background & selection
  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);
  QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

  // === 1. Get color for square ===
  QColor color       = Qt::red;    // default
  QVariant colorData = index.data(Qt::UserRole);
  if(colorData.canConvert<QColor>()) {
    color = colorData.value<QColor>();
  }

  const bool isHidden = index.data(joda::ui::gui::ItemDataRole::UserRoleElementIsDisabled).toBool();

  // === 2. Draw square ===
  int squareSize = 8;
  int margin     = 4;
  QRect squareRect(opt.rect.left() + margin, opt.rect.center().y() - squareSize / 2, squareSize, squareSize);

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setBrush(color);
  painter->setPen(Qt::NoPen);
  painter->drawRect(squareRect);

  // === 3. Draw text to the right of the square ===
  QString classs = index.data(Qt::DisplayRole).toString();

  //  int32_t imgChannel = index.data(0x102).toInt();
  const QString &text = classs;

  QRect textRect = opt.rect.adjusted(squareSize + 2 * margin, 0, 0, 0);
  if(isHidden) {
    painter->setPen(Qt::gray);
  } else {
    painter->setPen(Qt::black);
  }
  QFont f = painter->font();
  f.setItalic(isHidden);
  painter->setFont(f);
  painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

  painter->restore();
}

}    // namespace joda::ui::gui
