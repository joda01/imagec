
#include <memory>
#include <string>
#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include "plot_heatmap.hpp"

///
/// \brief  Spot test
/// \author Joachim Danmayr
///
TEST_CASE("plot::test:heatmap", "[plot::test:heatmap]")
{
  joda::plot::Heatmap heatmap;

  joda::table::Table data;
  for(int x = 0; x < 10; x++) {
    for(int y = 0; y < 10; y++) {
      data.setData(y, x, joda::table::TableCell{(double) x + y, {}, {}});
    }
  }

  heatmap.setData(std::move(data));
  heatmap.setBackgroundColor({255, 255, 255});
  heatmap.setPlotLabels(true);
  heatmap.setHighlightCell({2, 2});
  heatmap.setPrecision(1);
  heatmap.setGapsBetweenBoxes(5);
  heatmap.setShape(joda::plot::Heatmap::Shape::OVAL);

  auto plotArea = heatmap.plot({1024, 1024});
  cv::imwrite("output.png", plotArea);    // or .jpg, .bmp, etc.
}
