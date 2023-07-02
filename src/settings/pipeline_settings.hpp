///
/// \file      pipeline_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <nlohmann/json.hpp>

class PipelineSettings final
{
public:
  void interpretConfig()
  {
  }

private:
  std::string function;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(PipelineSettings, function);
};
