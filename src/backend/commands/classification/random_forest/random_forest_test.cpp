
#include <string>
#include "backend/commands/classification/random_forest/random_forest_settings.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>
#include "random_forest.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("ai::classifier::random_forest", "[random_forest]")
{
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation(
      "/workspaces/imagec/tmp/imagec-test/scenarios/scenario_01/test_data_v1_full/images/full/B2_15_5ADVMLE.vsi.vsi", 0, {});
  auto img = joda::image::reader::ImageReader::loadEntireImage(
      "/workspaces/imagec/tmp/imagec-test/scenarios/scenario_01/test_data_v1_full/images/full/B2_15_5ADVMLE.vsi.vsi", {0, 0, 0}, 0, 0, omeXML);

  joda::settings::RandomForestSettings settings;
  joda::cmd::RandomForest randForest(settings);
}
