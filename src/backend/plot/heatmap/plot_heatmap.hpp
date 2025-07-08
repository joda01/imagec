///
/// \file      plot_heatmap.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <optional>
#include "backend/helper/table/table.hpp"
#include "backend/plot/plot_base.hpp"
#include <opencv2/core/mat.hpp>
namespace joda::plot {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class Heatmap : PlotBase
{
public:
  /////////////////////////////////////////////////////
  struct Cell
  {
    int col = 0;
    int row = 0;
  };

  enum class Shape
  {
    RECTANGLE,
    OVAL
  };

  /////////////////////////////////////////////////////
  auto plot(const Size &size) -> cv::Mat override;
  void setBackgroundColor(const cv::Vec3b &);
  void setColorMap(ColormapName colorMap);
  void setData(const joda::table::Table &&data) override;
  auto getData() const -> const joda::table::Table &;
  void setPlotLabels(bool);
  void setPrecision(int32_t);
  void setHighlightCell(const Cell &);
  void resetHighlight();
  void setShape(Shape);
  void setGapsBetweenBoxes(int32_t);
  [[nodiscard]] auto getCellFromCoordinates(double x, double y) const -> std::optional<std::tuple<Cell, joda::table::TableCell>>;

private:
  /////////////////////////////////////////////////////
  joda::table::Table mData;
  cv::Vec3b mBackgroundColor = {0, 0, 0};
  bool mPlotLabels           = false;
  int32_t mPrecision         = 0;
  int32_t mGap               = 0;
  Shape mShape               = Shape::RECTANGLE;
  ColormapName mColorMap     = ColormapName::VIRIDIS;

  // TEMP ///////////////////////////////////////////////////
  std::optional<Cell> mHighLight;
  Size mSize;
  double mRectWidth  = 0;
  double mRectHeight = 0;
};

}    // namespace joda::plot
