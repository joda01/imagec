///
/// \file      pipeline_test.cpp
/// \author    Joachim Danmayr
/// \date      2024-06-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include <exception>
#include <filesystem>
#include <sstream>
#include <string>
#include <thread>
#include "backend/helper/file_info_images.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include "backend/image_processing/reader/jpg/image_loader_jpg.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/settings/analze_settings.hpp"
#include "controller/controller.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <nlohmann/json_fwd.hpp>
#include "common_test.hpp"

using namespace std::chrono_literals;
namespace fs = std::filesystem;

namespace joda::test {

bool initialized = false;
std::unique_ptr<joda::ctrl::Controller> controller;
void generateTestingCode(joda::results::Analyzer *results);

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
SCENARIO("pipeline:test:heatmap", "[pipeline]")
{
  if(!initialized) {
    initialized = true;
    joda::image::TiffLoader::initLibTif();
    joda::image::BioformatsLoader::init();
    joda::pipeline::PipelineFactory::init();

    try {
      if(fs::exists("test/test_data/images/pipeline_01/imagec") &&
         fs::is_directory("test/test_data/images/pipeline_01/imagec")) {
        fs::remove_all("test/test_data/images/pipeline_01/imagec");
        std::cout << "Folder removed successfully." << std::endl;
      } else {
        std::cout << "Path does not exist or is not a directory." << std::endl;
      }
    } catch(...) {
    }
    controller = executePipeline("test/scenarios/pipeline_01.json", "test/test_data/images/pipeline_01");
  }

  GIVEN("A specific initial condition")
  {
    WHEN("Loading the results")
    {
      auto results   = getAnalyze(controller->getOutputFolder());
      auto profiling = parseProfiling(controller->getOutputFolder());

      THEN("We expect a created database with filled out data.")
      {
        REQUIRE(results->getAnalyzes().size() == 1);
        std::string analysisId = results->getAnalyzes().begin()->analyzeId;

        auto images = results->getImagesForAnalyses(analysisId);
        CHECK(images.size() == 2);
      }
      THEN("We expect a created database with filled out data.")
      {
        try {
          REQUIRE(results->getAnalyzes().size() == 1);
          std::string analysisId = results->getAnalyzes().begin()->analyzeId;

          auto images = results->getImagesForAnalyses(analysisId);
          CHECK(images.size() == 2);
        } catch(const std::exception &ex) {
          std::cout << ex.what() << std::endl;
        }
      }
      THEN("Check the measured data for the channels")
      {
        //////////////////////////////////////////////////////
        // Enter generated code here

        THEN("Check the CONFIDENCE for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5225.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8100.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(21.2775, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(17.7840, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4447.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5762.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(309.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(350.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(31.2793, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.9310, 0.0001));
          }
        }
        THEN("Check the PERIMETER for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13.4428, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12.6286, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12.4853, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(11.0711, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2809.5386, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4091.6647, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(70.3259, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(92.3675, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(10.3443, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9.3274, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9594, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9558, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(200.5224, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(309.6788, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6245, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4756, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0727, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0818, 0.0001));
          }
        }
        THEN("Check the VALID for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(209.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(324.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INVALID for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1138.2297, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(845.2994, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1315.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(718.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(237890.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(273877.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(29.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2042.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2041.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(648.7594, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(560.1901, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1076.1435, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1185.7469, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(959.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1209.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(224914.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(384182.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2037.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(549.0688, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(638.2966, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.2919, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0833, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(897.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1323.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(29.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.3420, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.9698, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.4689, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0988, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(934.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1328.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(22.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(28.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.4945, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0673, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(175.3702, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(158.5727, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(142.3684, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(137.8258, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(36652.3710, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51377.5560, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(99.6667, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.8333, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1457.1877, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(483.9114, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(105.2981, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(53.0924, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(140.8565, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(129.7160, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(113.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(107.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(29439.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(42028.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(54.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(341.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(329.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(68.4928, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(58.5727, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for CH0 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(260.4928, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(213.2500, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(197.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(176.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(54443.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(69093.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(109.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(109.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(501.9667, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(137.1735, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for CH0 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for CH0 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for CH0 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_COUNT for CH0 data")
        {
        }
        THEN("Check the CONFIDENCE for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(23970.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(39695.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(85.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(27.3901, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(22.9529, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(18.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7724.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10719.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(323.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(340.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(33.5491, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27.6812, 0.0001));
          }
        }
        THEN("Check the PERIMETER for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16.4989, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15.1259, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.3137, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13.8995, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4652.6983, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(7063.8135, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(77.6396, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(87.2965, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(10.8258, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9.0847, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9383, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9478, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(264.5897, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(442.6438, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.4530, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.5286, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.1010, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0859, 0.0001));
          }
        }
        THEN("Check the VALID for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(282.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(467.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INVALID for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1310.4468, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(835.3704, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1527.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(707.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(369546.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(390118.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(39.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2041.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(607.3728, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(574.5130, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(943.7092, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1127.9036, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(809.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1158.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(266126.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(526731.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(505.7028, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(605.9999, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.1348, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.8694, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1448.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2274.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(28.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.2993, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.9125, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.5567, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.9122, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1567.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2294.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.9560, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.1655, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(614.6635, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(539.5777, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(556.0694, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(511.6250, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(173335.0936, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(251982.7672, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(368.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(344.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7710.0650, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2012.7265, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(444.2330, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(134.7385, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(482.2908, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(442.2141, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(465.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(424.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(136006.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(206514.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1109.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(959.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(112.4142, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(98.2333, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for CH1 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(949.7199, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(725.6188, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(697.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(618.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(267821.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(338864.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(368.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(344.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH1,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2724.1508, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(497.5957, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for CH1 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for CH1 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for CH1 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_COUNT for CH1 data")
        {
        }
        THEN("Check the CONFIDENCE for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(244608.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(440181.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6272.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11368.7692, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(11644.5294, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11117.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8228.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(443382.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(593871.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25301.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(62046.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5757.4111, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10843.3874, 0.0001));
          }
        }
        THEN("Check the PERIMETER for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(367.6733, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(347.9981, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(363.2275, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(311.7300, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14339.2589, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(17747.9044, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12.4853, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6.2426, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(619.4012, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(975.7586, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(116.5676, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(165.3209, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9576, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9436, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(37.3465, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(48.1221, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.7157, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.5995, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0696, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1046, 0.0001));
          }
        }
        THEN("Check the VALID for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(39.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(51.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INVALID for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1263.2308, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(918.1569, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1403.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(755.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(49266.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(46826.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(50.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2021.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(605.8372, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(630.7166, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(904.4615, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1070.0784, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(821.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1110.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(35274.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(54574.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1960.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2047.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(535.1321, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(638.1742, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(122.0256, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(114.8627, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(117.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(102.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4759.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5858.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(238.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(327.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(41.9740, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(55.6762, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(123.6667, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(116.7843, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(125.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(111.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4823.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5956.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(211.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(283.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(41.7406, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(55.8033, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(19845.1006, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(23723.7473, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20148.4507, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(23226.1022, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(773958.9230, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1209911.1147, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6419.0769, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9578.6667, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(26285.1390, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(38393.1144, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4071.1140, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6587.8228, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3619.2308, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6333.4902, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5777.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8053.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(141150.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(323008.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6176.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9214.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2903.2003, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3366.3972, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for CH3 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(50956.4872, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49044.9216, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(53963.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(50134.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1987303.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2501291.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6568.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9987.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(65443.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(65446.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH3,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12397.8037, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(15493.4719, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for CH3 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for CH3 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for CH3 data")
        {
        }
        THEN("Check the CROSS_CHANNEL_COUNT for CH3 data")
        {
        }
        THEN("Check the CONFIDENCE for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4497.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4497.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2055129.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3311165.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4497.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4497.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4631.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1255.0263, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1105.1091, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(34.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(32.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(573547.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(790153.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(249374.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(135870.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12311.0986, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(7439.9613, 0.0001));
          }
        }
        THEN("Check the PERIMETER for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(64.8196, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(65.4086, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20.9706, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(20.1421, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(29622.5525, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(46767.1417, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6608.1021, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4067.5269, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(339.2165, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(238.6195, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.8911, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.8639, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9674, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9433, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(407.2489, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(617.6710, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0718, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1032, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.1639, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1886, 0.0001));
          }
        }
        THEN("Check the VALID for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(457.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(715.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INVALID for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1079.1772, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(868.2028, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1171.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(709.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(493184.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(620765.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(593.8145, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(601.2817, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(846.7571, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1113.0685, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(785.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1109.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(386968.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(795844.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2045.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(547.3623, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(553.3821, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16.3348, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16.4587, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(7.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7465.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(11768.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(895.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(574.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(53.3653, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(44.0750, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.7090, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16.9524, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7179.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12121.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(586.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(773.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(44.7144, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(50.4706, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(27407.3740, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26461.3213, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(27511.4500, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26571.2469, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12525169.9080, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(18919844.7615, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2354.5380, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(36437.6000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(37478.1101, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2828.0879, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2809.3945, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24854.6958, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(23628.5441, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(25624.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24640.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11358596.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(16894409.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(35470.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(34949.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4563.6799, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5443.7881, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30642.2713, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(29800.5664, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(29615.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(28740.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14003518.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(21307405.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(22640.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(65535.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(65535.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5941.3514, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5400.3915, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(48.9717, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49.8035, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(171.8323, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(175.0220, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(67.6406, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(68.7778, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(227.2658, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(229.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(22380.0809, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(35609.5004, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(78527.3506, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(125140.7492, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(97.4780, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(89.5584, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(436.3101, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(593.7143, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(32.7616, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(33.1490, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(116.8006, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(122.7639, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(35.3873, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(33.6070, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(134.9672, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(129.7441, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(49.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(49.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(192.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(191.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16172.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(24029.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(61680.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(92767.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(84.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(82.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(352.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(563.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(27.1190, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(28.2154, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(103.9306, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(115.1986, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(90.0306, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(77.6671, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(335.9059, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(243.7469, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(88.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(88.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(263.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(266.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(41144.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(55532.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(153509.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(174279.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(409.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2417.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(342.3274, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(61.9050, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2166.0550, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(238.6585, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_COUNT for CH4 data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.3523, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.3273, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.5996, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.5692, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(161.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(234.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(274.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(407.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(62.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(33.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(116.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(50.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.4349, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.9963, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH4,
                {joda::results::MeasureChannel::CROSS_CHANNEL_COUNT, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.9345, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3.5046, 0.0001));
          }
        }
        THEN("Check the CONFIDENCE for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.8275, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.8981, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.8919, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(41.3744, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(77.2348, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0417, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.2640, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2081, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1832, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(10854.3200, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8570.4767, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(11008.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5442.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(542716.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(737061.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(27084.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(34435.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7984.4913, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9886.7072, 0.0001));
          }
        }
        THEN("Check the PERIMETER for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(450.4734, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(368.4019, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(490.3646, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(343.3280, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(22523.6679, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(31682.5630, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9.6569, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(967.6581, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1127.3810, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(281.4574, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(342.5798, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6061, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.6470, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.5691, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.6530, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(30.3059, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(55.6428, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.1732, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1053, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2143, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.2402, 0.0001));
          }
        }
        THEN("Check the VALID for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0984, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.1101, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(50.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(86.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2981, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.3132, 0.0001));
          }
        }
        THEN("Check the INVALID for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9015, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.8898, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(458.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(695.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.2981, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.3132, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1235.2800, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(735.1279, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1402.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(576.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(61764.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(63221.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(45.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2018.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2028.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(635.5835, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(621.1753, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(964.2200, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1248.9419, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(936.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1420.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(48211.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(107409.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1958.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2037.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(531.7110, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(647.1410, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(147.2000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(114.1512, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(153.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(108.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7360.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9817.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(414.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(355.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(95.4715, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.7025, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(148.6800, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(109.3721, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(142.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(108.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7434.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(9406.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(336.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(393.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(98.2326, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.8091, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(65.2594, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(73.1184, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(282.6689, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(291.0017, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(72.8300, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(78.3442, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(289.1126, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(298.6250, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3262.9688, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(6288.1788, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14133.4451, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25026.1486, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.6905, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4193, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6.2811, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.5994, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(97.4780, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(100.8138, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(444.6045, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(442.3461, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20.2039, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(17.9539, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(93.9112, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(73.0544, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(14.1400, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.3953, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(44.8600, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(104.0116, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(707.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2270.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2243.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8945.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(63.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(81.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(295.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(349.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0213, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(31.2942, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(97.3741, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(134.5829, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for A data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(355.4800, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(198.4767, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1812.5200, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(747.9419, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(216.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(183.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(771.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(628.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(17774.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(17069.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(90626.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(64323.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(66.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(67.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(265.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(276.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1343.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2417.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1003.7757, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(150.1378, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::A,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(6430.6546, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(494.2071, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_COUNT for A data")
        {
        }
        THEN("Check the CONFIDENCE for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9748, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9701, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(144.2732, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(238.6540, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.7436, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.6667, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CONFIDENCE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0469, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0541, 0.0001));
          }
        }
        THEN("Check the AREA_SIZE for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.9527, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(19.0854, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3693.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4695.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(293.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(292.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(31.2526, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(25.5582, 0.0001));
          }
        }
        THEN("Check the PERIMETER for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(15.7969, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(13.6436, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13.8995, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12.4853, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2337.9436, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(3356.3224, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.8284, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(67.7401, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(80.4680, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(9.2576, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8.6001, 0.0001));
          }
        }
        THEN("Check the CIRCULARITY for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9474, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9479, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(140.2166, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(233.1733, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6129, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4756, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0786, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0833, 0.0001));
          }
        }
        THEN("Check the VALID for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(148.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(246.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::VALID, joda::results::ChannelIndex::ME}, joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INVALID for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INVALID, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_X for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1206.4054, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(825.0894, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1487.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(705.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(178548.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(202972.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(37.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(2042.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2041.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_X, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(636.7047, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(556.5894, 0.0001));
          }
        }
        THEN("Check the CENTER_OF_MASS_Y for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1031.3919, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1170.1870, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(913.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1192.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(152646.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(287866.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1991.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2046.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CENTER_OF_MASS_Y, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(498.5930, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(629.1507, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_WIDTH for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0473, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.4106, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(747.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1085.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(24.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_WIDTH, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.0513, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.7786, 0.0001));
          }
        }
        THEN("Check the BOUNDING_BOX_HEIGHT for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.2703, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.4634, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(5.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(4.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(780.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1098.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(21.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(26.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::BOUNDING_BOX_HEIGHT, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3.2232, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2.9563, 0.0001));
          }
        }
        THEN("Check the INTENSITY_AVG for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_AVG, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MIN for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MIN, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the INTENSITY_MAX for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::INTENSITY_MAX, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_AVG for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(154.7549, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(143.3088, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(721.3954, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(600.5099, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(138.4091, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(137.4097, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(642.2833, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(566.8869, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(22903.7248, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(35253.9622, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(106766.5142, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(147725.4265, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(107.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(103.8333, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(418.6364, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(381.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1519.5870, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(525.0445, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(8367.4676, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(2173.6507, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(115.7635, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(33.4307, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(646.2089, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(185.0762, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MIN for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(109.6014, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(110.5691, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(503.1351, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(463.7317, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(110.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(107.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(494.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(435.5000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(16221.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(27200.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(74464.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(114078.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(68.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(285.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(197.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(208.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1055.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(924.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(20.7114, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(20.0011, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(132.5440, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(118.2646, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_INTENSITY_MAX for B data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(249.0608, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(197.4797, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1229.8514, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(845.5163, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(178.5000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(170.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(823.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(720.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(36861.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(48580.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(182018.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(207997.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(110.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(106.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(444.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(386.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(7280.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1798.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(46219.0000, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(8762.0000, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH0},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(586.7552, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(126.5110, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::B,
                {joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX, joda::results::ChannelIndex::CH1},
                joda::results::Stats::STDDEV);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(3739.8580, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(653.1595, 0.0001));
          }
        }
        THEN("Check the CROSS_CHANNEL_COUNT for B data")
        {
        }

        //////////////////////////////////////////////////////////
      }
      // generateTestingCode(results.get());
    }
  }

  joda::pipeline::PipelineFactory::shutdown();
  joda::image::BioformatsLoader::destroy();
}

void generateTestingCode(joda::results::Analyzer *results)
{
  std::set<joda::results::ChannelIndex> channels;
  channels.emplace(joda::results::ChannelIndex::CH0);
  channels.emplace(joda::results::ChannelIndex::CH1);
  channels.emplace(joda::results::ChannelIndex::CH3);
  channels.emplace(joda::results::ChannelIndex::CH4);
  channels.emplace(joda::results::ChannelIndex::A);
  channels.emplace(joda::results::ChannelIndex::B);

  std::stringstream str;

  for(const auto channelIdx : channels) {
    for(int i = 1; i < 18; i++) {
      auto measure = (joda::results::MeasureChannel) i;

      str << "THEN(\"Check the " + toString(measure) + " for " + toString(channelIdx) + " data\")\n";
      str << "{\n";
      for(int s = 0; s < 6; s++) {
        auto stats = (joda::results::Stats) s;

        auto meChannel = [&measure, &stats, &results, &str, &channelIdx](joda::results::ChannelIndex meE) {
          auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(*results, 1, 16, 24, channelIdx,
                                                                                 {measure, meE}, stats);

          if(!std::isnan(table.data(0, 9).getVal()) || !std::isnan(table.data(1, 9).getVal())) {
            str << "{\n";
            str << "auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(*results, 1, 16, 24, "
                   "joda::results::ChannelIndex::" +
                       toString(channelIdx) + ",{joda::results::MeasureChannel::" + toString(measure) +
                       ", "
                       "joda::results::ChannelIndex::" +
                       toString(meE) + "}, joda::results::Stats::" + toString(stats) + ");\n";

            if(!std::isnan(table.data(0, 9).getVal())) {
              str << "CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(" << std::fixed
                  << std::setprecision(4) << table.data(0, 9).getVal() << ", 0.0001));\n";
            }
            if(!std::isnan(table.data(1, 9).getVal())) {
              str << "CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(" << std::fixed
                  << std::setprecision(4) << table.data(1, 9).getVal() << ", 0.0001));\n";
            }
            str << "}\n";
          }
        };

        if(measure == joda::results::MeasureChannel::CROSS_CHANNEL_COUNT ||
           measure == joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_AVG ||
           measure == joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MAX ||
           measure == joda::results::MeasureChannel::CROSS_CHANNEL_INTENSITY_MIN) {
          for(int me = 0; me < 2; me++) {
            meChannel((joda::results::ChannelIndex) me);
          }
        } else {
          meChannel((joda::results::ChannelIndex::ME));
        }
      }
      str << "}\n";
    }
  }

  // Step 2: Open an output file stream
  std::ofstream outFile("output.cpp");

  // Step 3: Check if the file was successfully opened
  if(!outFile) {
    std::cerr << "Error opening file for writing!" << std::endl;
  }

  // Step 4: Write the string stream to the file
  outFile << str.str();

  // Step 5: Close the file stream
  outFile.close();
}
}    // namespace joda::test