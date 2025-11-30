
#include <opencv2/core/hal/interface.h>
#include "fft_bandpass.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include "backend/commands/image_functions/fft_bandpass/fft_bandpass.hpp"
#include "backend/commands/image_functions/fft_bandpass/fft_bandpass_settings.hpp"
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/helper/ai_model_parser/ai_model_parser.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("cmd::fftbandbass", "[fft_bandpass]")
{
  std::filesystem::path imgPath = "/workspaces/imagec/tmp/tupless/C4_02.vsi";
  joda::image::reader::ImageReader reader(imgPath);

  auto omeXML = reader.getOmeInformation(0, {});
  auto img    = reader.loadEntireImage({0, 3, 0}, 0, 0, omeXML);

  for(int z = 1; z < omeXML.getNrOfZStack(0); z++) {
    auto imgTmp = reader.loadEntireImage({z, 0, 0}, 0, 0, omeXML);
    cv::max(img, imgTmp, img);
  }

  cv::imwrite("/workspaces/imagec/tmp/original.png", img);

  joda::settings::FFTBandpassSettings settings;
  settings.doScaling = true;

  joda::cmd::FFTBandpass ai(settings);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::settings::ProjectPipelineSetup pipSetup;
  joda::processor::PipelineInitializer pipeLinieInit(setup, pipSetup, imgPath);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, omeXML};
  auto list = std::make_shared<joda::atom::ObjectList>();
  joda::processor::IterationContext iter(list);
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  ai.execute(img);

  cv::imwrite("/workspaces/imagec/tmp/img.png", img / 10);
}
