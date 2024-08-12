///
/// \file      setting.hpp
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
#include <utility>
#include "backend/global_enums.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::settings {

class Setting
{
public:
  /////////////////////////////////////////////////////
  virtual void check() const = 0;

protected:
  /////////////////////////////////////////////////////
  void throwError(const std::string &what) const
  {
    const auto name = std::string(typeid(*this).name());
    throw std::invalid_argument(static_cast<std::string>(name + "::" + what));
  }
};

}    // namespace joda::settings
