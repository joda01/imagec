///
/// \file      factory.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/global_enums.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/setting.hpp"
#include <opencv2/core/mat.hpp>
#include "command.hpp"

namespace joda::ui {

template <class T>
concept Command_t = ::std::is_base_of<joda::ui::Command, T>::value;

template <Command_t CMD, class SETTING>
class Factory : public CMD
{
public:
  Factory(SETTING &setting, QWidget *parent = nullptr) : CMD(setting, parent)
  {
  }

private:
};

}    // namespace joda::ui
