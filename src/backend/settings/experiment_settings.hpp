///
/// \file      project_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <set>
#include <nlohmann/json.hpp>

namespace joda::settings {

class ExperimentSettings
{
public:
  enum class GroupBy
  {
    OFF,
    DIRECTORY,
    FILENAME
  };

  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  GroupBy groupBy = GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with 3 groupings: _((.)([0-9]+))_
  //
  std::string filenameRegex = "_((.)([0-9]+))_([0-9]+)";

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> wellImageOrder = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};

  //
  // Generate a heatmap for grouped images
  //
  bool generateHeatmapForPlate = false;

  //
  // Generate a heatmap for a well
  //
  bool generateHeatmapForWell = false;

  //
  // Generate a heatmap for each image
  //
  bool generateHeatmapForImage = false;

  //
  // With of the square used for heatmap creation in image
  //
  std::set<int32_t> imageHeatmapAreaSizes;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ExperimentSettings, groupBy, filenameRegex, wellImageOrder,
                                              generateHeatmapForPlate, generateHeatmapForWell, imageHeatmapAreaSizes,
                                              generateHeatmapForImage);
};

// map TaskState values to JSON as strings
NLOHMANN_JSON_SERIALIZE_ENUM(ExperimentSettings::GroupBy, {
                                                              {ExperimentSettings::GroupBy::OFF, "Off"},
                                                              {ExperimentSettings::GroupBy::DIRECTORY, "Directory"},
                                                              {ExperimentSettings::GroupBy::FILENAME, "Filename"},
                                                          })

}    // namespace joda::settings
