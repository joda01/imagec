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

#pragma once

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

namespace joda::ui::gui {

class ColoredSquareDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  explicit ColoredSquareDelegate(QObject *parent = nullptr);
  ~ColoredSquareDelegate() override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
}    // namespace joda::ui::gui
