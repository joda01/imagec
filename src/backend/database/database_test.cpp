#include "backend/duration_count/duration_count.h"
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
  db.addImage(1, 1, "My image 1");
  db.addImage(1, 2, "My image 2");
  db.addChannel(1, 1, "Channel 1");
  db.addChannel(1, 2, "Channel 2");

  db.addChannel(2, 1, "Channel 1");
  db.addChannel(2, 2, "Channel 2");

  auto id = DurationCount::start("Insert");
  db.addObject(1, 1, 1, 777, 999);
  DurationCount::stop(id);
  DurationCount::printStats(1);
}
