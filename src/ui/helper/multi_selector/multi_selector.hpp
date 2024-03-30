///
/// \file      channel_selector.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qlineedit.h>
#include <qvariant.h>
#include <QtWidgets>
#include <set>

namespace joda::ui::qt {
class MultiSelector : public QLineEdit
{
public:
  /////////////////////////////////////////////////////
  MultiSelector(QWidget *parent);
  void setCurrentIndex(std::set<int32_t>);
  int findData(const QVariant &data, int role = Qt::UserRole);
  [[nodiscard]] QVariant currentData(int role = Qt::UserRole) const;
  void addItem(const QString &atext, const QVariant &auserData);
  void addItem(const QIcon &aicon, const QString &atext, const QVariant &auserData);

signals:
  void currentIndexChanged();

private:
  /////////////////////////////////////////////////////
  void showSelectorDialog();

  struct DataEntry
  {
    QString userRole;
    QIcon decorationRole;
  };

  std::map<QVariant, DataEntry> mDataList;
};
}    // namespace joda::ui::qt
