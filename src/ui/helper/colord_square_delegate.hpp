///
/// \file      colord_square_delegate.hpp
/// \author    Joachim Danmayr
/// \date      2024-11-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

namespace joda::ui {

class ColoredSquareDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit ColoredSquareDelegate(QObject *parent = nullptr);
  ~ColoredSquareDelegate() override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
}    // namespace joda::ui
