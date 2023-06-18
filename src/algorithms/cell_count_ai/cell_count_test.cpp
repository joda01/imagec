#include "image_reader/jpg/image_loader_jpg.hpp"
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
  joda::algo::CellCounter cellCounter("test/output", &tb);
  auto img = JpgLoader::loadEntireImage("test/cells.jpg", 0);
  cellCounter.execute(joda::Image{.mImage = img, .mName = "test", .mTileNr = 0});
}
