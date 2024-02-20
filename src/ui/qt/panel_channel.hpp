///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <QtWidgets>
#include "panel_function.hpp"

namespace joda::ui::qt {

class PanelChannel : public QWidget
{
public:
  PanelChannel();

private:
  QHBoxLayout *createLayout();
  QVBoxLayout *addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor, int margin = 16) const;
};

}    // namespace joda::ui::qt
