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

#include "panel_edit_base.hpp"

namespace joda::ui::qt {

void PanelEdit::onValueChanged()
{
  valueChangedEvent();
}
}    // namespace joda::ui::qt
