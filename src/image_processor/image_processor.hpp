///
/// \file      image_processor.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-02
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

///
/// \class      ImageProcessor
/// \author     Joachim Danmayr
/// \brief      Processes a set of images
///
#include <filesystem>
#include <string>
#include <vector>
#include "helper/helper.hpp"
#include "image_processor/image_processor_base.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "pipelines/pipeline.hpp"
#include "reporting/reporting.h"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

// Concept for Pipeline classes message
template <class T>
concept pipeline_t = std::is_base_of<joda::pipeline::Pipeline, T>::value;

///
/// \class      ImageProcessor
/// \author     Joachim Danmayr
/// \brief      Image processor must be of type pipeline
///
template <pipeline_t PIPELINE>
class ImageProcessor : public ImageProcessorBase
{
public:
  /////////////////////////////////////////////////////
  using ImageProcessorBase::ImageProcessorBase;

  ///
  /// \brief      Returns the output file path of the csv report
  /// \author     Joachim Danmayr
  ///
  [[nodiscard]] auto getReportFilePath() const -> const std::string & override
  {
    return mOutputFilePathAllOverReport;
  }

  ///
  /// \brief      Start the processor
  /// \author     Joachim Danmayr
  ///
  void analyzeAllImages() override
  {
    uint16_t dir                 = 14;
    auto outFolder               = prepareOutputFolders();
    mOutputFilePathAllOverReport = outFolder + "/report.csv";
    setTotalImages(getListOfImagePaths().size());

    uint64 cnt = 0;
    for(const auto &imagePath : getListOfImagePaths()) {
      std::filesystem::path path_obj(imagePath);
      std::string filename = path_obj.filename().stem().string();

      auto imgProperties = TiffLoader::getImageProperties(imagePath, dir);
      if(imgProperties.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
        // Image too big to load at once -> Load image in tiles
        int tilesToLoadPerRun = 36;
        uint64 runs           = imgProperties.nrOfTiles / tilesToLoadPerRun;
        joda::reporting::Table tileReport;
        setTotalActImages(runs);
        for(uint64 n = 0; n < runs; n++) {
          auto tilePart = TiffLoader::loadImageTile(imagePath, dir, n, tilesToLoadPerRun);
          analyze(tilePart, tileReport, outFolder, filename, n);
          setProgressActImage(n + 1);
          if(isStopped()) {
            break;
          }
        }
        tileReport.flushReportToFile(outFolder + "/" + filename + "/" + "report_" + filename + ".csv");
        PIPELINE::mergeReport(filename, getAllOverReporting(), tileReport);
      } else {
        auto entireImage = TiffLoader::loadEntireImage(imagePath, dir);
        analyze(entireImage, getAllOverReporting(), outFolder, filename, -1);
      }
      cnt++;
      setProgressTotal(cnt);
      if(isStopped()) {
        break;
      }
    }

    getAllOverReporting().flushReportToFile(mOutputFilePathAllOverReport);
  }

private:
  /////////////////////////////////////////////////////
  static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 0;

  /////////////////////////////////////////////////////

  void analyze(cv::Mat &mat, joda::reporting::Table &reporting, const std::string &outFolder,
               const std::string &imageName, int64_t tileNr) const
  {
    PIPELINE pipeline(outFolder, &reporting);
    pipeline.analyzeImage(joda::Image{.mImage = mat, .mName = imageName, .mTileNr = tileNr});
  }

  [[nodiscard]] auto prepareOutputFolders() const -> std::string
  {
    auto nowString    = ::joda::helper::timeNowToString();
    auto outputFolder = getOutputFolder() + "/" + nowString;

    bool directoryExists = false;
    if(!std::filesystem::exists(outputFolder)) {
      directoryExists = std::filesystem::create_directories(outputFolder);
      if(!directoryExists) {
        throw std::runtime_error("Can not create output folder!");
      }
    } else {
      directoryExists = true;
    }
    return outputFolder;
  }

  /////////////////////////////////////////////////////
  std::string mOutputFilePathAllOverReport;
};
}    // namespace joda::processor
