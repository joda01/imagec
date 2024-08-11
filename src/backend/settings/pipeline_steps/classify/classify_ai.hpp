#pragma once

#include <nlohmann/json.hpp>

namespace joda::settings {

class AiSettings final
{
public:
  //
  // Name of the onnx AI model which should be used for detection.
  //
  std::string modelPath;

  //
  // Minimum probability
  //
  float minProbability = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AiSettings, modelPath, minProbability);
};
}    // namespace joda::settings
