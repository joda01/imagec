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

#pragma once

#include <QPainter>
#include <QStyledItemDelegate>

class ColorRectDelegate : public QStyledItemDelegate
{
public:
  explicit ColorRectDelegate(QObject *parent = nullptr);
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
