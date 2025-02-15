///
/// \file      icon_generator.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qicon.h>

namespace joda::ui::gui {

inline QIcon generateIcon(const QString &iconName)
{
  QIcon log;
  log.addFile(":/icons/icons/icons8-" + iconName + "-16.png", QSize(16, 16));
  log.addFile(":/icons/icons/icons8-" + iconName + "-24.png", QSize(24, 24));
  log.addFile(":/icons/icons/icons8-" + iconName + "-48.png", QSize(48, 48));
  log.addFile(":/icons/icons/icons8-" + iconName + "-96.png", QSize(96, 96));
  return log;
}

}    // namespace joda::ui::gui
