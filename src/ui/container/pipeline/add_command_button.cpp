///
/// \file      add_command_button.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "add_command_button.hpp"

namespace joda::ui {

AddCommandButtonBase::AddCommandButtonBase()
{
  connect(this, &QPushButton::clicked, this, &AddCommandButtonBase::onAddCommandClicked);
}

void AddCommandButtonBase::onAddCommandClicked()
{
  addCommandClicked();
}
}    // namespace joda::ui
