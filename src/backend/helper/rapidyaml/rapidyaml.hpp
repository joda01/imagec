///
/// \file      rapidyaml.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
