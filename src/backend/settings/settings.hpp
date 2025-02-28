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
  static std::string toString(const joda::settings::AnalyzeSettings &settings);
  static auto openSettings(const std::filesystem::path &pathIn) -> joda::settings::AnalyzeSettings;
  static void storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings);
  static void storeSettingsTemplate(const std::filesystem::path &pathIn, joda::settings::AnalyzeSettings settings);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld, const joda::settings::AnalyzeSettings &settingsNew);
  static void migrateSettings(std::string &settings);
};

}    // namespace joda::settings
