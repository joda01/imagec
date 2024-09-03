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
  static int32_t getNrOfAllPipelines(const joda::settings::AnalyzeSettings &settings);

  static void checkSettings(const joda::settings::AnalyzeSettings &settings);
  static bool isEqual(const joda::settings::AnalyzeSettings &settingsOld,
                      const joda::settings::AnalyzeSettings &settingsNew);

  static ObjectOutputClusters getOutputClasses(const joda::settings::AnalyzeSettings &settings);
};

}    // namespace joda::settings
