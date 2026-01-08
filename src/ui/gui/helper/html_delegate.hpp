
#pragma once
#include <qtextcursor.h>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include "ui/gui/helper/item_data_roles.hpp"

class HtmlDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
  {
    painter->save();

    const bool isHidden = index.data(joda::ui::gui::ItemDataRole::UserRoleElementIsDisabled).toBool();

    QTextDocument doc;
    doc.setHtml(index.data().toString());
    doc.setTextWidth(option.rect.width());

    if(isHidden) {
      QTextCursor cursor(&doc);
      cursor.select(QTextCursor::Document);
      QTextCharFormat format;
      format.setForeground(Qt::gray);    // Or option.palette.color(QPalette::Disabled, QPalette::Text)
      format.setFontItalic(true);
      cursor.mergeCharFormat(format);
    }

    // Standard background drawing
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = "";
    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    painter->translate(option.rect.topLeft());
    doc.drawContents(painter, QRectF(0, 0, option.rect.width(), option.rect.height()));

    painter->restore();
  }

  [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
  {
    QTextDocument doc;
    doc.setHtml(index.data().toString());
    doc.setTextWidth(option.rect.width());
    return {static_cast<int>(doc.idealWidth()), static_cast<int>(doc.size().height())};
  }
};
