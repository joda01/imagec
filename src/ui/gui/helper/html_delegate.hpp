
#pragma once
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextDocument>

class HtmlDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
  {
    painter->save();

    QTextDocument doc;
    doc.setHtml(index.data().toString());
    doc.setTextWidth(option.rect.width());

    // Draw background and selection (optional but common)
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.text = "";    // Prevent default text rendering
    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    // Translate and draw HTML
    painter->translate(option.rect.topLeft());
    QRect clip(0, 0, option.rect.width(), option.rect.height());
    doc.drawContents(painter, clip);

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
