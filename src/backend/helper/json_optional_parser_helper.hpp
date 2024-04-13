#pragma once

// Define conversion from std::optional<EdgeDetection> to nlohmann::json
#include <optional>
#include <nlohmann/json_fwd.hpp>

template <typename T>
void to_json(nlohmann::json &j, const std::optional<T> &opt)
{
  if(opt == std::nullopt) {
  } else if(opt.has_value()) {
    j = opt.value();    // Just assign the value if present
  }
}

template <typename T>
void from_json(const nlohmann::json &j, std::optional<T> &opt)
{
  if(!j.is_null()) {
    T value;
    j.get_to(value);
    opt = value;
  } else {
    opt = std::nullopt;    // Convert null to empty optional
  }
}
