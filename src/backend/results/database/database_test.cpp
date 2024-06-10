#include <exception>
#include <filesystem>
#include <random>
#include <string>
#include <thread>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/database/database_interface.hpp"
#include "backend/results/db_column_ids.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/channel/channel_index.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <duckdb/common/types.hpp>

#include "database.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("database:test", "[.][database_test]")
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(1.0f, 1000.0f);

  /* for(uint32_t n = 0; n < 10000; n++) {
     auto &ob = obj[n];

     for(int m = 0; m < 20; m++) {
       float randNr = dis(gen);
       ob.keys.emplace_back(duckdb::Value::UINTEGER(m));
       ob.vals.emplace_back(duckdb::Value::DOUBLE(randNr));
     }
   }*/

  joda::results::db::Database db("test_with_idx.duckdb");
  db.open();
  db.createAnalyze(::joda::results::db::AnalyzeMeta{
      .name = "Hello", .scientists = {"Joachim", "Melanie"}, .location = "SBG", .notes = "Notes"});

  try {
    db.createPlate(::joda::results::db::PlateMeta{
        .analyzeId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32", .plateId = 1, .notes = "May plates"});
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  try {
    db.createGroup(::joda::results::db::GroupMeta{
        .analyzeId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32", .plateId = 1, .groupId = 1, .notes = "May plates"});
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  const int NRIMAGES   = 500;
  const int NRCHANNELS = 5;

  for(uint32_t n = 0; n < NRIMAGES; n++) {
    std::string imgName = "A110_" + std::to_string(n) + ".vsi";

    try {
      db.createImage(::joda::results::db::ImageMeta{.analyzeId         = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                                    .plateId           = 1,
                                                    .groupId           = 1,
                                                    .imageId           = n,
                                                    .originalImagePath = imgName,
                                                    .width             = 10,
                                                    .height            = 10});
    } catch(const std::exception &ex) {
      std::cout << ex.what() << std::endl;
    }

    for(uint8_t ch = 0; ch < NRCHANNELS; ch++) {
      try {
        db.createChannel(::joda::results::db::ChannelMeta{.analyzeId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                                          .channelId = joda::results::ChannelIndex::A,

                                                          .name         = "A",
                                                          .measurements = {}});
      } catch(const std::exception &ex) {
        std::cout << ex.what() << std::endl;
      }

      auto id = DurationCount::start("Insert");
      /*try {
        db.createObjects(::joda::results::db::ObjectMeta{.analyzeId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                                         .imageId   = n,
                                                         .channelId = (joda::results::ChannelIndex) ch,
                                                         .objects   = obj});
      } catch(const std::exception &ex) {
        std::cout << ex.what() << std::endl;
      }*/
      DurationCount::stop(id);
    }

    joda::log::logInfo("Added images >" + std::to_string(n) + "<");
  }

  db.close();

  // DurationCount::stop(id);
  DurationCount::printStats(1);
}

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("database:test", "[.][database_read]")
{
  joda::results::Analyzer res(std::filesystem::path("src/backend/results/database/test/results.duckdb"));
  auto jobs = res.getAnalyzes();
  CHECK(jobs[0].analyzeId == "10217c38-3056-43cb-9397-2a15b7756833");
  for(const auto &job : jobs) {
    std::cout << job.runId << " | ";
    std::cout << job.analyzeId << " | ";
    std::cout << job.location << " | ";
    std::cout << job.name << " | ";
    std::cout << job.notes << " | ";
    for(const auto &name : job.scientists) {
      std::cout << name << " | ";
    }
    std::cout << joda::helper::timepointToIsoString(job.timestamp) << std::endl;
  }

  auto images = res.getImagesForAnalyses("10217c38-3056-43cb-9397-2a15b7756833");
  for(const auto &img : images) {
    std::cout << img.analyzeId << " | ";
    std::cout << img.originalImagePath.filename().string() << " | ";
    std::cout << std::to_string(img.plateId) << " | ";
    std::cout << std::to_string(img.groupId) << " | ";
    std::cout << std::to_string(img.imageId) << " | ";
    std::cout << std::to_string(img.width) << " | ";
    std::cout << std::to_string(img.height) << std::endl;
  }

  auto channels = res.getChannelsForAnalyses("10217c38-3056-43cb-9397-2a15b7756833");
  for(const auto &img : channels) {
    std::cout << img.analyzeId << " | ";
    std::cout << std::to_string(static_cast<uint32_t>(img.channelId)) << " | ";
    std::cout << img.name << std::endl;
  }

  auto plates = res.getPlatesForAnalyses("10217c38-3056-43cb-9397-2a15b7756833");
  for(const auto &img : plates) {
    std::cout << img.analyzeId << " | ";
    std::cout << std::to_string(img.plateId) << " | ";
    std::cout << img.notes << std::endl;
  }

  auto wells = res.getGroupsForPlate("10217c38-3056-43cb-9397-2a15b7756833", 1);
  for(const auto &img : wells) {
    std::cout << img.analyzeId << " | ";
    std::cout << std::to_string(img.plateId) << " | ";
    std::cout << std::to_string(img.wellPosX) << " | ";
    std::cout << std::to_string(img.wellPosY) << " | ";
    std::cout << img.notes << std::endl;
  }

  // res.getData(4261282133957314495, 266);
  auto result = joda::results::analyze::plugins::HeatmapPerPlate::getData(
      res, 1, 15, 15, joda::results::ChannelIndex::CH0, joda::results::MeasureChannelId(0), joda::results::Stats::AVG);
  result.print();
}
