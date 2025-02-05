#pragma once

#include <cstdint>
#include <set>
#include "backend/enums/enum_memory_idx.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ImageCacheSettings : public SettingBase
{
public:
  enums::MemoryIdx::Enum memoryId = enums::MemoryIdx::M0;
  std::string name;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getInputImageCache() const override
  {
    return {};
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getOutputImageCache() const override
  {
    return {memoryId};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageCacheSettings, memoryId, name);
};

}    // namespace joda::settings
