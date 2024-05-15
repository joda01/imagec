#include <string>
#include <thread>
#include "backend/duration_count/duration_count.h"
#include "backend/logger/console_logger.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#include "database.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("database:test", "[database_test]")
{
  joda::db::Database db("test.db");
  db.open();
  db.addExperiment(1, "My experiment");
  auto id = DurationCount::start("Insert");

  for(int img = 0; img < 5; img++) {
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
  db.close();

  DurationCount::stop(id);
  DurationCount::printStats(1);
}
