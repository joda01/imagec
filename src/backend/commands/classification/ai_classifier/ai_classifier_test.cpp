
#include <opencv2/core/hal/interface.h>
#include "ai_classifier.hpp"

#include <string>
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("ai::classifier::test", "[.][ai_classifieer]")
{
  joda::image::reader::ImageReader::init(1e9);
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation("tmp/sample_input_00.tif", 0);
  auto img    = joda::image::reader::ImageReader::loadEntireImage("tmp/sample_input_00.tif", {0, 0, 0}, 1, 0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath            = "tmp/weights-torchscript.pt";
  aiSets.numberOfModelClasses = 3;
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 2});

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, "tmp/sample_input_0.tif", omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  ai.execute(context, img, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C0});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C1});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  joda::image::reader::ImageReader::destroy();
}
