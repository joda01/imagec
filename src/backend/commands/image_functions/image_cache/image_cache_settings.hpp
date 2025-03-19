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
  enum class Mode
  {
    STORE,
    LOAD,
  };

  Mode mode = Mode::STORE;

  enums::MemoryIdx::Enum memoryId = enums::MemoryIdx::M0;
  std::string name;
  enums::MemoryScope memoryScope = enums::MemoryScope::PIPELINE;

  /////////////////////////////////////////////////////
  void check() const
  {
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getInputImageCache() const override
  {
    if(memoryScope == enums::MemoryScope::PIPELINE) {
      return {};
    }
    if(mode == Mode::LOAD) {
      return {memoryId};
    }
    return {};
  }

  [[nodiscard]] std::set<enums::MemoryIdx::Enum> getOutputImageCache() const override
  {
    if(memoryScope == enums::MemoryScope::PIPELINE) {
      return {};
    }
    if(mode == Mode::STORE) {
      return {memoryId};
    }
    return {};
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageCacheSettings, mode, memoryScope, memoryId, name);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageCacheSettings::Mode, {
                                                           {ImageCacheSettings::Mode::STORE, "Store"},
                                                           {ImageCacheSettings::Mode::LOAD, "Load"},
                                                       });

}    // namespace joda::settings
