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
      THEN("Check the measured data for the CY7 channel")
      {
        THEN("Check the area size data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::SUM);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(4447, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(5762, 0.0001));
          }
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
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MIN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME}, joda::results::Stats::MAX);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(309, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(350, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::AREA_SIZE, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(10, 0.0001));
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
        THEN("Check the perimeter data")
        {
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
                {joda::results::MeasureChannel::PERIMETER, joda::results::ChannelIndex::ME}, joda::results::Stats::AVG);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(13.4428, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(12.6286, 0.0001));
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
                joda::results::Stats::MEDIAN);
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(12.4853, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(11.0711, 0.0001));
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

        THEN("Check the circularity data")
        {
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::SUM);
            table.print();
            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(200.5224, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(309.6788, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::AVG);
            table.print();

            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.9594, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.9558, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MIN);
            table.print();

            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.6245, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.4756, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MAX);
            table.print();

            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::MEDIAN);
            table.print();

            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(1, 0.0001));
          }
          {
            auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
                *results, 1, 16, 24, joda::results::ChannelIndex::CH0,
                {joda::results::MeasureChannel::CIRCULARITY, joda::results::ChannelIndex::ME},
                joda::results::Stats::STDDEV);
            table.print();

            CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(0.0727, 0.0001));
            CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(0.0818, 0.0001));
          }
        }
      }
      for(int i = 1; i < 18; i++) {
        auto measure = (joda::results::MeasureChannel) i;

        std::cout << "THEN(\"Check the " + toString(measure) + " data\")\n";
        std::cout << "{\n";
        for(int s = 0; s < 6; s++) {
          auto stats = (joda::results::Stats) s;

          auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(
              *results, 1, 16, 24, joda::results::ChannelIndex::CH0, {measure, joda::results::ChannelIndex::ME}, stats);

          std::cout << "{\n";
          std::cout << "auto table = joda::results::analyze::plugins::HeatmapPerPlate::getData(*results, 1, 16, 24, "
                       "joda::results::ChannelIndex::CH0,{joda::results::MeasureChannel::" +
                           toString(measure) +
                           ", "
                           "joda::results::ChannelIndex::ME}, joda::results::Stats::" +
                           toString(stats) + ");\n";

          std::cout << "CHECK_THAT(table.data(0, 9).getVal(), Catch::Matchers::WithinAbs(" << std::fixed
                    << std::setprecision(4) << table.data(0, 9).getVal() << ", 0.0001));\n";
          std::cout << "CHECK_THAT(table.data(1, 9).getVal(), Catch::Matchers::WithinAbs(" << std::fixed
                    << std::setprecision(4) << table.data(1, 9).getVal() << ", 0.0001));\n";
          std::cout << "}\n";
        }
        std::cout << "}\n";
      }
    }
  }

  joda::pipeline::PipelineFactory::shutdown();
  joda::image::BioformatsLoader::destroy();
}
}    // namespace joda::test
