///
/// \file      analyze_settings.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <fstream>
#include <set>
#include <catch2/catch_config.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ChannelSettings final
{
  //
  // Corresponding index (directory) in the TIF image
  // [0, 1, 2, ...]
  //
  std::set<uint32_t> index;

  //
  // What is seen in this channel
  // [NUCLEUS, EV, BACKGROUND, CELL_BRIGHTFIELD, CELL_DARKFIELD]
  //
  std::string type;

  //
  // Which threshold algorithm should be used
  // [MANUAL, LI, MIN_ERROR, TRIANGLE]
  //
  std::string threshold_algorithm;

  //
  // Minimum threshold value.
  // [0-65535]
  //
  uint32_t threshold_min;

  //
  // Maximum threshold value (default 65535)
  // [0-65535]
  //
  uint32_t threshold_max;

  //
  // Every particle with a diameter lower than that is ignored during analysis.
  // Value in [px]
  //
  float min_particle_size;

  //
  // Every particle with a diameter bigger than that is ignored during analysis.
  // Value in [px]
  //
  float max_particle_size;

  //
  // Every particle with a circularity lower than this value is ignored during analysis.
  // Value in [0-1]
  //
  float min_circularity;

  //
  // Used for coloc algorithm to define a tolerance around each particle.
  // Value in [px]
  //
  float snap_area_size;

  //
  // How much of the edge should be cut off.
  // Value in [px]
  //
  float margin_crop;

  //
  // Do a z-projection before analysis starts
  // [NONE, MAX]
  //
  std::string zprojection;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ChannelSettings, index, type, threshold_algorithm, threshold_min, threshold_max,
                                 min_particle_size, max_particle_size, min_circularity, snap_area_size, margin_crop,
                                 zprojection);
};

class AnalyzeSettings final
{
public:
  //
  // Pipeline to analyze the pictures with
  // [NUCLEUS_COUNT, EV_COUNT, EV_COLOC, EV_COLOC_IN_CELLS]
  //
  std::string pipeline;

  //
  // Settings for the image channels
  //
  std::vector<ChannelSettings> channels;

  //
  // Minimum of area overlapping to identify two particles as colocalize
  // Value between [0-1]
  //
  float min_coloc_factor;

  //
  // How many micrometers are represented by one pixel
  // Value in [um]
  //
  float pixel_in_micrometer;

  //
  // With or without control images
  // [true, false]
  //
  bool with_control_images;

  //
  // Detailed report on
  // [true, false]
  //
  bool with_detailed_report;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalyzeSettings, pipeline, channels, min_coloc_factor, pixel_in_micrometer,
                                 with_control_images, with_detailed_report);

  void loadConfig(const std::string &cfgPath)
  {
    std::ifstream input(cfgPath);
    *this = json::parse(input);
  }
};
