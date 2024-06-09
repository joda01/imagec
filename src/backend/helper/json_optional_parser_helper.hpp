#pragma once

#include <optional>
#include <nlohmann/json.hpp>

inline void removeNullValues(nlohmann::json &j)
{
  if(j.is_object()) {
    for(auto it = j.begin(); it != j.end();) {
      if(it.value().is_null()) {
        it = j.erase(it);
      } else {
        removeNullValues(it.value());    // Recursively check nested objects
        ++it;                            // Move to the next element
      }
    }
  } else if(j.is_array()) {
    for(auto &element : j) {
      if(element.is_null()) {
        element = nullptr;    // Replace null values in arrays with nullptr
      } else {
        removeNullValues(element);    // Recursively check nested objects
      }
    }
  }
}

namespace nlohmann {

template <typename T>
inline void to_json(nlohmann::json &j, const std::optional<T> &opt)
{
  if(opt == std::nullopt) {
  } else if(opt.has_value()) {
    j = opt.value();    // Just assign the value if present
  }
}

template <typename T>
inline void from_json(const nlohmann::json &j, std::optional<T> &opt)
{
  if(!j.is_null()) {
    T value;
    j.get_to(value);
    opt = value;
  } else {
    opt = std::nullopt;    // Convert null to empty optional
  }
}

}    // namespace nlohmann
