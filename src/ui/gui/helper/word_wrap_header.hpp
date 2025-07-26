///
/// \file      word_wrap_header.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <QHeaderView>
#include <QPainter>
#include <QStyleOptionHeader>
#include <QTextDocument>

class WordWrapHeader : public QHeaderView
{
public:
  WordWrapHeader(Qt::Orientation orientation, QWidget *parent = nullptr) : QHeaderView(orientation, parent)
  {
    setDefaultAlignment(Qt::AlignCenter);
  }

protected:
  void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override
  {
    if(painter == nullptr) {
      return;
    }

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect     = rect;
    opt.section  = logicalIndex;
    auto txtTmp  = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    opt.text     = "";
    opt.position = QStyleOptionHeader::Middle;

    // Draw the native header background and border
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    // Prepare a QTextDocument for word-wrapped text
    QTextDocument doc;
    doc.setDefaultTextOption(QTextOption(Qt::AlignCenter));
    doc.setTextWidth(rect.width() - 8);    // Padding
    // doc.setDefaultFont(opt.font);
    doc.setHtml("<div align='center'>" + txtTmp.toHtmlEscaped() + "</div>");

    // Determine where to draw the text
    QRect textRect = style()->subElementRect(QStyle::SE_HeaderLabel, &opt, this).adjusted(4, 2, -4, -2);
    painter->save();
    painter->translate(textRect.topLeft());
    QRect clip(0, 0, textRect.width(), textRect.height() + 40);
    doc.drawContents(painter, clip);
    painter->restore();
  }

  QSize sectionSizeFromContents(int logicalIndex) const override
  {
    QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    QTextDocument doc;
    // doc.setTextWidth(150);    // Example max width
    doc.setDefaultFont(font());
    doc.setHtml("<div align='center'>" + text.toHtmlEscaped() + "</div>");
    QSize size = doc.size().toSize();
    return QSize(size.width() + 8, size.height() + 40);    // Add some padding
  }
};
