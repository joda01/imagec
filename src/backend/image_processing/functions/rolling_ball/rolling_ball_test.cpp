#include "../../../image_reader/jpg/image_loader_jpg.hpp"
#include "../../../image_reader/tif/image_loader_tif.hpp"
#include "../../../settings/channel_settings.hpp"
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>
#include "rolling_ball.hpp"

///
/// \brief  Load a config file
/// \author Joachim Danmayr
///
TEST_CASE("func::rollingball", "[rolling_ball]")
{
  auto img = TiffLoader::loadEntireImage("test_areosold_Evs/hoechst.tiff", 0);
  joda::func::img::RollingBallBackground bg(40);
  bg.execute(img);

  img = img * ((float) 255.0F / (float) 65536.0F) * 15;
  cv::imwrite("rolling_ball_test.jpg", img);
}
