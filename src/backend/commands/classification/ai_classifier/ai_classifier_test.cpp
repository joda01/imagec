
#include <opencv2/core/hal/interface.h>
#include "ai_classifier.hpp"

#include <string>
#include "backend/commands/image_functions/image_saver/image_saver.hpp"
#include "backend/helper/ai_model_parser/ai_model_parser.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("ai::classifier::test::nucleus", "[ai_classifier]")
{
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation("models/nucleisegmentationboundarymodel_torchscript/sample_input_00.tif", 0);
  auto img = joda::image::reader::ImageReader::loadEntireImage("models/nucleisegmentationboundarymodel_torchscript/sample_input_00.tif", {0, 0, 0}, 0,
                                                               0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath = "models/nucleisegmentationboundarymodel_torchscript/weights-torchscript.pt";
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 2});

  aiSets.modelParameters = joda::onnx::AiModelParser::getModelInfo("models/nucleisegmentationboundarymodel_torchscript/weights-torchscript.pt");

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, "models/nucleisegmentationboundarymodel_torchscript/sample_input_00.tif", omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  ai.execute(context, img, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C0});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C1});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  CHECK(result.size() == 1);
  CHECK(result.begin()->second->size() == 44);
}

TEST_CASE("ai::classifier::test::livecell", "[ai_classifier]")
{
  auto omeXML = joda::image::reader::ImageReader::getOmeInformation("models/livecellsegmentationboundarymodel_torchscript/sample_input_0.tif", 0);
  auto img = joda::image::reader::ImageReader::loadEntireImage("models/livecellsegmentationboundarymodel_torchscript/sample_input_0.tif", {0, 0, 0},
                                                               0, 0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath = "models/livecellsegmentationboundarymodel_torchscript/weights-torchscript.pt";
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 2});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 3});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 4});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 5});

  aiSets.modelParameters = joda::onnx::AiModelParser::getModelInfo("models/livecellsegmentationboundarymodel_torchscript/weights-torchscript.pt");

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, "models/livecellsegmentationboundarymodel_torchscript/sample_input_0.tif", omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  ai.execute(context, img, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{
      .inputClass = joda::enums::ClassIdIn::C0, .style = joda::settings::ImageSaverSettings::Style::OUTLINED, .paintBoundingBox = false});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{
      .inputClass = joda::enums::ClassIdIn::C1, .style = joda::settings::ImageSaverSettings::Style::OUTLINED, .paintBoundingBox = false});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2,
                                                                                   .style = joda::settings::ImageSaverSettings::Style::OUTLINED});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C3,
                                                                                   .style      = joda::settings::ImageSaverSettings::Style::FILLED});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C4,
                                                                                   .style      = joda::settings::ImageSaverSettings::Style::FILLED});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C5,
                                                                                   .style      = joda::settings::ImageSaverSettings::Style::FILLED});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  CHECK(result.size() == 1);
  CHECK(result.begin()->second->size() == 193);
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("ai::classifier::test::onnx", "[ai_classifier]")
{
  std::string path = "/workspaces/imagec/build/build/output/Test ImageC v15/test folder/B2_15_5ADVMLE.vsi.vsi";
  auto omeXML      = joda::image::reader::ImageReader::getOmeInformation(path, 0);
  auto img         = joda::image::reader::ImageReader::loadEntireImage(path, {0, 3, 0}, 0, 0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath = "models/university_of_sbg_cell_segmentation_v3/weights.onnx";
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});

  aiSets.modelParameters = joda::onnx::AiModelParser::getModelInfo("models/university_of_sbg_cell_segmentation_v3/weights.onnx");

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, path, omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  ai.execute(context, img, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(
      joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C0, .paintBoundingBox = false});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C1});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  CHECK(result.size() == 1);
  CHECK(result.begin()->second->size() == 56);
}

TEST_CASE("ai::classifier::test::pytorch::yolo", "[ai_classifier]")
{
  std::string path = "/workspaces/imagec/build/build/output/Test ImageC v15/test folder/B2_15_5ADVMLE.vsi.vsi";
  auto omeXML      = joda::image::reader::ImageReader::getOmeInformation(path, 0);
  auto img         = joda::image::reader::ImageReader::loadEntireImage(path, {0, 3, 0}, 0, 0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath = "models/university_of_sbg_cell_segmentation_v3/weights.pt";
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});

  aiSets.modelParameters = joda::onnx::AiModelParser::getModelInfo("models/university_of_sbg_cell_segmentation_v3/weights.pt");

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, path, omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  cv::Mat aiorward = img.clone();
  ai.execute(context, aiorward, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(
      joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C0, .paintBoundingBox = false});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C1});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  CHECK(result.size() == 1);
  CHECK(result.begin()->second->size() == 56);
}

TEST_CASE("ai::classifier::test::pytorch::coco", "[ai_classifier]")
{
  std::string path = "/workspaces/imagec/build/build/output/Test ImageC v15/test folder/B2_15_5ADVMLE.vsi.vsi";
  auto omeXML      = joda::image::reader::ImageReader::getOmeInformation(path, 0);
  auto img         = joda::image::reader::ImageReader::loadEntireImage(path, {0, 3, 0}, 0, 0, omeXML);

  joda::settings::AiClassifierSettings aiSets;
  aiSets.modelPath = "models/coco/yolo11m.pt";
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 0});
  aiSets.modelClasses.emplace_back(joda::settings::ObjectClass{.modelClassId = 1});

  aiSets.modelParameters = joda::onnx::AiModelParser::getModelInfo("models/coco/yolo11m.pt");

  joda::cmd::AiClassifier ai(aiSets);
  joda::atom::ObjectList result;
  joda::settings::ProjectImageSetup setup;
  joda::processor::PipelineInitializer pipeLinieInit(setup);
  joda::processor::GlobalContext glob;
  glob.resultsOutputFolder = "/workspaces/imagec/tmp";
  joda::processor::PlateContext plate;
  joda::processor::ImageContext imgCtx{pipeLinieInit, path, omeXML};
  joda::processor::IterationContext iter;
  joda::processor::ProcessContext context(glob, plate, imgCtx, iter);

  cv::Mat aiorward = img.clone();
  ai.execute(context, aiorward, result);
  joda::settings::ImageSaverSettings imageSaver;
  imageSaver.classesIn.emplace_back(
      joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C0, .paintBoundingBox = false});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C1});
  imageSaver.classesIn.emplace_back(joda::settings::ImageSaverSettings::SaveClasss{.inputClass = joda::enums::ClassIdIn::C2});

  joda::cmd::ImageSaver imgSaver(imageSaver);
  imgSaver.execute(context, img, result);

  CHECK(result.size() == 1);
  CHECK(result.begin()->second->size() == 56);
}
