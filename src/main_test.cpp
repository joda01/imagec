#include "backend/helper/reader/image_reader.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

int main(int argc, char **argv)
{
  joda::image::reader::ImageReader::init();

  int result = Catch::Session().run(argc, argv);

  // global clean-up...
  joda::image::reader::ImageReader::destroy();
  return result;
}

///
/// \brief  A simple Test case
/// \author Joachim Danmayr
///
TEST_CASE("main", "[main]")
{
  /*REQUIRE( 3 == add(1,2));
  REQUIRE( 3 == add(1,2));*/
}
