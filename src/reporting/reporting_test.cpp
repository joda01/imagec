#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "reporting.h"

SCENARIO("Adding three values to a table", "[table]")
{
  joda::reporting::Table table;
  table.appendValueToColumn(0, 0);
  table.appendValueToColumn(0, 382);
  table.appendValueToColumn(0, 527);
  table.appendValueToColumn(0, 474);
  table.appendValueToColumn(0, 460);
  table.appendValueToColumn(0, 498);
  table.appendValueToColumn(0, 428);
  table.appendValueToColumn(0, 889);
  table.appendValueToColumn(0, 798);
  table.appendValueToColumn(0, 403);

  table.appendValueToColumn(1, 4);

  CHECK(0 == table.getTable().at(0).at(0).value);
  CHECK(382 == table.getTable().at(0).at(1).value);
  CHECK(527 == table.getTable().at(0).at(2).value);

  CHECK(10 == table.getStatistics().at(0).getNr());
  CHECK(4859 == table.getStatistics().at(0).getSum());
  CHECK(0 == table.getStatistics().at(0).getMin());
  CHECK(889 == table.getStatistics().at(0).getMax());
  CHECK(485.9 == Catch::Approx(table.getStatistics().at(0).getMean()).epsilon(0.5));

  CHECK(table.getNrOfRows() == 10);
  CHECK(table.getNrOfColumns() == 2);
}
