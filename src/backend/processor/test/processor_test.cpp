

#include <opencv2/core/hal/interface.h>
#include <exception>
#include <filesystem>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include "../initializer/pipeline_initializer.hpp"
#include "../processor.hpp"
#include "backend/enums/enums_classes.hpp"

#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/settings/analze_settings.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::test {

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:heatmap", "[processor]")
{
  std::ifstream file("src/backend/processor/test/test_run.json");
  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(file);
  file.close();
  joda::processor::Processor processor;
  processor::imagesList_t workingdirs;
  workingdirs.setWorkingDirectory(settings.projectSettings.plate.imageFolder);
  workingdirs.waitForFinished();
  processor.execute(settings, "test",
                    joda::ctrl::Controller::calcOptimalThreadNumber(settings, workingdirs.gitFirstFile(), workingdirs.getNrOfFiles(), std::nullopt),
                    workingdirs);
}

}    // namespace joda::test
