#include "helper/termbox/termbox2.h"
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include "report_printer.h"

SCENARIO("Print table", "[table_print]")
{
  tb_init();
  ReportPrinter::printTable("src/reporting/report.csv");
  struct tb_event event;

  tb_poll_event(&event);

  tb_shutdown();
}
