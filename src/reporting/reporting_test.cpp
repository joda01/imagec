#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "reporting.h"

SCENARIO("Adding three values to a table", "[table]")
{
  joda::reporting::Table table;
  table.appendValueToColumn(0, 2);
  table.appendValueToColumn(0, 3);
  table.appendValueToColumn(0, 4);

  CHECK(2 == table.getTable().at(0).at(0));
  CHECK(3 == table.getTable().at(0).at(1));
  CHECK(4 == table.getTable().at(0).at(2));

  CHECK(3 == table.getStatisitcs().at(0).getNr());
  CHECK(9 == table.getStatisitcs().at(0).getSum());
  CHECK(29 == table.getStatisitcs().at(0).getQuadratSum());
  CHECK(2 == table.getStatisitcs().at(0).getMin());
  CHECK(4 == table.getStatisitcs().at(0).getMax());
}
