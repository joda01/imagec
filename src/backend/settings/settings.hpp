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

  struct ResultSettingsInput
  {
    std::vector<Class> classes;
    std::set<enums::ClassId> outputClasses;
    std::map<enums::ClassId, std::set<enums::ClassId>> intersectingClasses;
    std::map<enums::ClassId, std::set<int32_t>> measuredChannels;
    std::map<enums::ClassId, std::set<enums::ClassId>> distanceFromClasses;
  };

  static auto openSettings(const std::filesystem::path &pathIn) -> joda::settings::AnalyzeSettings;
  static void storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings);
  static void storeSettingsTemplate(const std::filesystem::path &pathIn, joda::settings::AnalyzeSettings settings, const SettingsMeta &settingsMeta);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld, const joda::settings::AnalyzeSettings &settingsNew);
  static void migrateSettings(std::string &settings);
  static auto toResultsSettings(const ResultSettingsInput &settingsIn) -> ResultsSettings;
};

}    // namespace joda::settings
