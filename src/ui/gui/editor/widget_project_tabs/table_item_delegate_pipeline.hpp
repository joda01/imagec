
///
/// \file      tabel_item_delegate_preview_Result.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qapplication.h>
#include <qcolor.h>
#include <qnamespace.h>
#include <QPainter>
#include <QStyledItemDelegate>
#include "ui/gui/helper/item_data_roles.hpp"

namespace joda::ui::gui {

class ColorSquareDelegatePipeline : public QStyledItemDelegate
{
public:
  explicit ColorSquareDelegatePipeline(QObject *parent = nullptr) : QStyledItemDelegate(parent)
  {
  }

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
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

    const int32_t imgChannel = index.data(joda::ui::gui::ItemDataRole::UserRoleChannelId).toInt();
    QString suffix;
    if(imgChannel == -1) {
      suffix = "None";
    } else if(imgChannel == -2) {
      suffix = "Undefined";
    } else {
      suffix = "CH" + QString::number(imgChannel);
    }
    QString text = classs + "/" + suffix;

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
};
}    // namespace joda::ui::gui
