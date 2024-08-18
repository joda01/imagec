

#include <exception>
#include <filesystem>
#include <sstream>
#include <string>
#include <thread>
#include "../processor.hpp"
#include "backend/settings/analze_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::test {

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:heatmap", "[pipeline]")
{
  std::ifstream file("src/backend/processor/test/test_run.json");
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(file);
  file.close();
  joda::processor::Processor processor;
  processor::imagesList_t workingdirs;
  processor.execute(settings, workingdirs);
}

}    // namespace joda::test
