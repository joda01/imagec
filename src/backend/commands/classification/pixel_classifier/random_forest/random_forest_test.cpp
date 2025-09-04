
#include "random_forest.hpp"

#include <string>
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("ai::classifier::random_forest", "[random_forest]")
{
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation("/workspaces/imagec/tmp/Anna Images/G8_03.vsi", 0, {});
  auto img    = joda::image::reader::ImageReader::loadEntireImage("/workspaces/imagec/tmp/Anna Images/G8_03.vsi", {0, 0, 0}, 0, 0, omeXML);

  joda::cmd::RandomForest randForest;
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::settings::ProjectPipelineSetup pipSetup;
  joda::processor::PipelineInitializer pipeLinieInit(setup, pipSetup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, "/workspaces/imagec/tmp/Anna Images/G8_03.vsi", omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  randForest.execute(context, img, result);
}
