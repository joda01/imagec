#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../settings/channel_settings.hpp"
#include "backend/reporting/reporting.h"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "cell_count.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("cell:counter", "[cell_counter]")
{
  joda::reporting::Table tb;
  joda::pipeline::detection::CellCounter cellCounter;
  auto img = JpgLoader::loadEntireImage("test/cells.jpg");
  joda::settings::json::ChannelSettings set;
  cellCounter.execute(img, img, set);
}
