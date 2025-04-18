#include "backend/settings/analze_settings.hpp"

namespace joda::settings {

///
/// \class      Settings
/// \author     Joachim Danmayr
/// \brief      Settings helper class
///
class Settings
{
public:
  template <class T>
  static std::string toString(const T &settings)
  {
    nlohmann::json json = settings;
    removeNullValues(json);
    return json.dump(2);
  }
  static auto openSettings(const std::filesystem::path &pathIn) -> joda::settings::AnalyzeSettings;
  static void storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings);
  static void storeSettingsTemplate(const std::filesystem::path &pathIn, joda::settings::AnalyzeSettings settings);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld, const joda::settings::AnalyzeSettings &settingsNew);
  static void migrateSettings(std::string &settings);
};

}    // namespace joda::settings
