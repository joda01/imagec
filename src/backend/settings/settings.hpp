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
  static void storeSettings(const std::filesystem::path &pathIn, const joda::settings::AnalyzeSettings &settings);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld,
                      const joda::settings::AnalyzeSettings &settingsNew);
};

}    // namespace joda::settings
