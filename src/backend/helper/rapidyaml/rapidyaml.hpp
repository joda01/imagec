///
/// \file      rapidyaml.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <nlohmann/json_fwd.hpp>

namespace joda::yaml {

///
/// \class      Yaml
/// \author     Joachim Danmayr
/// \brief      Converting yaml to JSON
///
class Yaml
{
public:
  /////////////////////////////////////////////////////
  static void init();
  static nlohmann::json convert(std::string yamlString);
};

}    // namespace joda::yaml
