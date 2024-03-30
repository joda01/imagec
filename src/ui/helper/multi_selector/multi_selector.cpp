///
/// \file      channel_selector.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "multi_selector.hpp"
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qwidget.h>

namespace joda::ui::qt {

///
/// \brief     Constructor
/// \author    Joachim Danmayr
/// \return
///
MultiSelector::MultiSelector(QWidget *parent) : QLineEdit(parent)
{
}

void MultiSelector::setCurrentIndex(std::set<int32_t>)
{
}
int MultiSelector::findData(const QVariant &data, int role)
{
}
QVariant MultiSelector::currentData(int role) const
{
}
void MultiSelector::addItem(const QString &atext, const QVariant &auserData)
{
}
void MultiSelector::addItem(const QIcon &aicon, const QString &atext, const QVariant &auserData)
{
}

///
/// \brief     Shows a dialog which allows to select options
/// \author    Joachim Danmayr
///
void MultiSelector::showSelectorDialog()
{
  QDialog selectDialog(this);
  selectDialog.setModal(true);

  QGridLayout myGrid;
  selectDialog.setLayout(&myGrid);

  myGrid.addWidget(new QLabel("Test"));
  myGrid.addWidget(new QLabel("Test2"));

  selectDialog.exec();
}

}    // namespace joda::ui::qt
