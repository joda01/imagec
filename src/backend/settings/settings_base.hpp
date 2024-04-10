#pragma once

namespace joda::settings::json {

class SettingsBase
{
public:
  virtual void postParsing() = 0;
};

}    // namespace joda::settings::json
