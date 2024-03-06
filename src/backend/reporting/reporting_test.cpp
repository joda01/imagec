#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "reporting.h"

SCENARIO("Adding three values to a table", "[table]")
{
  joda::reporting::Table table;
  table.appendValueToColumn(0, 0, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 382, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 527, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 474, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 460, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 498, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 428, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 889, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 798, joda::func::ParticleValidity::VALID);
  table.appendValueToColumn(0, 403, joda::func::ParticleValidity::VALID);

  table.appendValueToColumn(1, 4, joda::func::ParticleValidity::VALID);
  table.setColumnNames({{0, "Col 1"}, {1, "Col 2"}});

  // table.flushReportToFile("output_test.xlsx");

  CHECK(0 == table.getTable().at(0).at(0).value);
  CHECK(382 == table.getTable().at(0).at(1).value);
  CHECK(527 == table.getTable().at(0).at(2).value);

  CHECK(10 == table.getStatistics().at(0).getNr());
  CHECK(4859 == table.getStatistics().at(0).getSum());
  CHECK(0 == table.getStatistics().at(0).getMin());
  CHECK(889 == table.getStatistics().at(0).getMax());
  CHECK(485.9 == Catch::Approx(table.getStatistics().at(0).getAvg()).epsilon(0.5));

  CHECK(table.getNrOfRows() == 10);
  CHECK(table.getNrOfColumns() == 2);
}
