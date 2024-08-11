
#pragma once

#include <cstdint>
#include <string>
#include "backend/results/analyzer/analyzer.hpp"
#include "backend/results/db_column_ids.hpp"
#include <opencv2/core/types.hpp>
#include "helper.hpp"

namespace joda::results::analyze::plugins {

class ControlImage
{
public:
  ///
  /// \brief      Get data for plates
  /// \author     Joachim Danmayr
  ///
  static auto getControlImage(Analyzer &analyzer, uint64_t imageId, ChannelIndex channelId, int32_t tileID,
                              const cv::Rect &boundingBox) -> cv::Mat
  {
    std::unique_ptr<duckdb::QueryResult> result = analyzer.getDatabase().select(
        "SELECT"
        "  control_image_path "
        "FROM channels_images "
        "WHERE"
        " image_id=$1 AND channel_id=$2 ",
        imageId, static_cast<uint8_t>(channelId));

    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    //
    // Get image path to load
    //
    auto materializedResult      = result->Cast<duckdb::StreamQueryResult>().Materialize();
    std::string controlImagePath = materializedResult->GetValue(0, 0).GetValue<std::string>();
    auto imagePath               = analyzer.getAbsolutePathToControlImage(controlImagePath, tileID);

    //
    // Open image
    //
    cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_COLOR);

    //
    // Paint bounding box
    //
    int x = boundingBox.x + boundingBox.width / 2;
    int y = boundingBox.y + boundingBox.height / 2;

    // Point of interest (change this to your desired point)
    cv::Point pointOfInterest(x, y);    // Example: center of the image

    // Draw horizontal line
    cv::line(image, cv::Point(0, pointOfInterest.y), cv::Point(image.cols - 1, pointOfInterest.y),
             cv::Scalar(0, 0, 255, 128), 1);

    // Draw vertical line
    cv::line(image, cv::Point(pointOfInterest.x, 0), cv::Point(pointOfInterest.x, image.rows - 1),
             cv::Scalar(0, 0, 255, 128), 1);

    // Paint rectangle
    cv::rectangle(image, boundingBox, cv::Scalar(0, 0, 255, 128), 2);

    return image;
  }
};
}    // namespace joda::results::analyze::plugins
