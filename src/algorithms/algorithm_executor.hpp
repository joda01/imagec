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
#include <string>
#include <utility>
#include "algorithms/algorithm.hpp"
#include "helper/helper.hpp"
#include "image_reader/image_reader.hpp"
#include "image_reader/jpg/image_loader_jpg.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "reporting/reporting.h"
#include "settings/analze_settings_parser.hpp"

namespace joda::algo {

static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 71680768;
static constexpr int64_t TILES_TO_LOAD_PER_RUN          = 36;

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
                        joda::reporting::Table &allOverReport, uint16_t channel, bool &mStop)
  {
    std::filesystem::path path_obj(imagePath);
    std::string filename = path_obj.filename().stem().string();
    ImageProperties imgProperties;
    bool isJpg = imagePath.ends_with(".jpg");
    if(isJpg) {
      imgProperties = JpgLoader::getImageProperties(imagePath, channel);
    } else {
      imgProperties = TiffLoader::getImageProperties(imagePath, channel);
    }

    if(imgProperties.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
      // Image too big to load at once -> Load image in tiles
      int tilesToLoadPerRun = TILES_TO_LOAD_PER_RUN;
      int64 runs            = imgProperties.nrOfTiles / tilesToLoadPerRun;
      joda::reporting::Table tileReport;
      mProgress->total = runs;
      for(int64 idx = 0; idx < runs; idx++) {
        ALGORITHM algo(outFolder, &tileReport);
        auto tilePart = TiffLoader::loadImageTile(imagePath, channel, idx, tilesToLoadPerRun);
        algo.execute(joda::Image{.mImage = tilePart, .mName = filename, .mTileNr = idx});
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
        auto entireImage = JpgLoader::loadEntireImage(imagePath, channel);
        algo.execute(joda::Image{.mImage = entireImage, .mName = filename, .mTileNr = -1});
      } else {
        auto entireImage = TiffLoader::loadEntireImage(imagePath, channel);
        algo.execute(joda::Image{.mImage = entireImage, .mName = filename, .mTileNr = -1});
      }
    }
  }

private:
  /////////////////////////////////////////////////////

  joda::types::Progress *mProgress;
};
}    // namespace joda::algo
