#include "../../../settings/channel/channel_settings.hpp"
#include "backend/image_processing/reader/bioformats/bioformats_loader.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>
#include "rolling_ball.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::rollingball", "[.][rolling_ball]")
{
  auto img = joda::image::BioformatsLoader::loadEntireImage("test_areosold_Evs/hoechst.tiff", 0, 0, 0);
  joda::image::func::RollingBallBackground bg({});
  bg.execute(img);

  img = img * ((float) 255.0F / (float) 65536.0F) * 15;
  cv::imwrite("rolling_ball_test.jpg", img);
}