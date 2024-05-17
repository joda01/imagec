#include <exception>
#include <random>
#include <string>
#include <thread>
#include "backend/duration_count/duration_count.h"
#include "backend/logger/console_logger.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <duckdb/common/types.hpp>

#include "database.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("database:test", "[database_test]")
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(1.0f, 1000.0f);

  joda::db::objects_t obj;
  for(uint32_t n = 0; n < 10000; n++) {
    auto &ob = obj[n];

    for(int m = 0; m < 20; m++) {
      float randNr = dis(gen);
      ob.keys.emplace_back(duckdb::Value::UINTEGER(m));
      ob.vals.emplace_back(duckdb::Value::DOUBLE(randNr));
    }
  }

  joda::db::Database db("test_with_idx.duckdb");
  db.open();
  db.createExperiment(::joda::db::ExperimentMeta{
      .name = "Hello", .scientists = {"Joachim", "Melanie"}, .location = "SBG", .notes = "Notes"});

  try {
    db.createPlate(::joda::db::PlateMeta{
        .experimentId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32", .plateId = 1, .notes = "May plate"});
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  try {
    db.createWell(::joda::db::WellMeta{
        .experimentId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32", .plateId = 1, .wellId = 1, .notes = "May plate"});
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }

  const int NRIMAGES   = 500;
  const int NRCHANNELS = 5;

  for(int n = 0; n < NRIMAGES; n++) {
    std::string imgName = "A110_" + std::to_string(n) + ".vsi";

    try {
      db.createImage(::joda::db::ImageMeta{.experimentId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                           .plateId      = 1,
                                           .wellId       = 1,
                                           .imageName    = imgName,
                                           .width        = 10,
                                           .height       = 10});
    } catch(const std::exception &ex) {
      std::cout << ex.what() << std::endl;
    }

    for(uint16_t ch = 0; ch < NRCHANNELS; ch++) {
      try {
        db.createChannel(::joda::db::ChannelMeta{.experimentId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                                 .plateId      = 1,
                                                 .wellId       = 1,
                                                 .imageName    = imgName,
                                                 .channelId    = ch});
      } catch(const std::exception &ex) {
        std::cout << ex.what() << std::endl;
      }

      auto id = DurationCount::start("Insert");
      try {
        db.createObjects(::joda::db::ObjectMeta{.experimentId = "d6e95ec1-6b87-45e7-856f-0c0779b57d32",
                                                .plateId      = 1,
                                                .wellId       = 1,
                                                .imageName    = imgName,
                                                .channelId    = ch,
                                                .objects      = obj});
      } catch(const std::exception &ex) {
        std::cout << ex.what() << std::endl;
      }
      DurationCount::stop(id);
    }

    joda::log::logInfo("Added image >" + std::to_string(n) + "<");
  }

  /*
    auto id = DurationCount::start("Insert");

    for(int img = 0; img < 500; img++) {
      db.addImage(1, img, "My image 1");
      joda::log::logInfo("Added element >" + std::to_string(img) + "<");

      auto addChannel = [&db, &img](int numOfObjects, const std::string &name, int ch) {
        db.addChannel(img, ch, name);
        db.addObject(1, img, ch, numOfObjects);
      };

      auto id2 = DurationCount::start("Insert ch");
      addChannel(10000, "EV1", 1);
      DurationCount::stop(id2);

      addChannel(10000, "EV2", 2);
      addChannel(1000, "CE1", 3);
      addChannel(1000, "NUC", 4);
      addChannel(10000, "COL1", 5);
      addChannel(10000, "COL2", 6);
    }
    */
  db.close();

  // DurationCount::stop(id);
  DurationCount::printStats(1);
}
