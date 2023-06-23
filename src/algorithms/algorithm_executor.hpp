///
/// \file      algorithm_executor.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Executes an algorithm
///

#pragma once

#include <httplib.h>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include "algorithms/algorithm.hpp"
#include "helper/helper.hpp"
#include "image_reader/image_reader.hpp"
#include "image_reader/jpg/image_loader_jpg.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "reporting/reporting.h"
#include "settings/analze_settings_parser.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::algo {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;
static constexpr int32_t TIME_FRAME                     = 0;

// Concept for Pipeline classes message
template <class T>
concept algorithm_t = std::is_base_of<joda::algo::Algorithm, T>::value;

template <algorithm_t ALGORITHM>
class AlgorithmExecutor
{
public:
  AlgorithmExecutor(joda::types::Progress *progress) : mProgress(progress)
  {
    // static_assert(mProgress != nullptr, "Progress must not be nullptr");
  }
  ///
  /// \brief      Executed the algorithm and generates reporting
  /// \author     Joachim Danmayr
  /// \param[in]
  /// \param[out]
  /// \return
  ///
  void executeAlgorithm(const std::string &imagePath, const std::string &outFolder,
                        joda::reporting::Table &allOverReport, uint32_t channel, bool &mStop)
  {
    std::filesystem::path path_obj(imagePath);
    std::string filename = path_obj.filename().stem().string();
    ImageProperties imgProperties;
    bool isJpg = imagePath.ends_with(".jpg");

    std::set<uint32_t> tiffDirectories;
    if(isJpg) {
      imgProperties = JpgLoader::getImageProperties(imagePath);
    } else {
      auto omeInfo    = TiffLoader::getOmeInformation(imagePath);
      tiffDirectories = omeInfo.getDirectoryForChannel(channel, TIME_FRAME);
      if(tiffDirectories.size() <= 0) {
        throw std::runtime_error("Selected channel does not contain images!");
      }
      imgProperties = TiffLoader::getImageProperties(imagePath, *tiffDirectories.begin());
    }

    if(imgProperties.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
      // Image too big to load at once -> Load image in tiles
      int64 runs = imgProperties.nrOfTiles / TILES_TO_LOAD_PER_RUN;
      joda::reporting::Table tileReport;
      mProgress->total = runs;
      for(int64 idx = 0; idx < runs; idx++) {
        ALGORITHM algo(outFolder, &tileReport);
        auto actDirectory = tiffDirectories.begin();
        cv::Mat tilePart  = TiffLoader::loadImageTile(imagePath, *actDirectory, idx, TILES_TO_LOAD_PER_RUN);
        //
        // Do maximum intensity projection
        //
        while(actDirectory != tiffDirectories.end()) {
          actDirectory = std::next(actDirectory);
          if(actDirectory == tiffDirectories.end()) {
            break;
          }
          cv::max(tilePart, TiffLoader::loadImageTile(imagePath, *actDirectory, idx, TILES_TO_LOAD_PER_RUN), tilePart);
        }
        //
        //

        algo.execute(joda::Image{tilePart, filename, channel, idx});
        mProgress->finished = idx + 1;
        if(mStop) {
          break;
        }
      }
      tileReport.flushReportToFile(outFolder + "/" + filename + "/" + "report_" + filename + ".csv");
      ALGORITHM::mergeReport(filename, allOverReport, tileReport);
    } else {
      ALGORITHM algo(outFolder, &allOverReport);
      if(isJpg) {
        auto entireImage = JpgLoader::loadEntireImage(imagePath);
        algo.execute(joda::Image{entireImage, filename, channel, -1});
      } else {
        auto actDirectory = tiffDirectories.begin();

        cv::Mat entireImage = TiffLoader::loadEntireImage(imagePath, *actDirectory);

        //
        // Do maximum intensity projection
        //
        while(actDirectory != tiffDirectories.end()) {
          actDirectory = std::next(actDirectory);
          if(actDirectory == tiffDirectories.end()) {
            break;
          }
          cv::max(entireImage, TiffLoader::loadEntireImage(imagePath, *actDirectory), entireImage);
        }
        //
        //
        algo.execute(joda::Image{entireImage, filename, channel, -1});
      }
    }
  }

private:
  /////////////////////////////////////////////////////
  joda::types::Progress *mProgress;
};
}    // namespace joda::algo
