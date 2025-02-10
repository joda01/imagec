///
/// \file      color_item.cpp
/// \author    Joachim Danmayr
/// \date      2024-11-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
