///
/// \file      image_processor.cpp
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

#include "image_processor.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include "helper/helper.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"
#include "reporting/reporting.h"
#include <opencv2/core/mat.hpp>

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  listOfImagePaths    List of paths to images which should be analyzed
///
ImageProcessor::ImageProcessor(const std::string &&ouputFolder, const std::vector<std::string> &&listOfImagePaths) :
    mOutputfolder(ouputFolder), mListOfImagePaths(listOfImagePaths)
{
}

///
/// \brief      Start analyzing
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ImageProcessor::start()
{
  uint16_t dir   = 14;
  auto outFolder = prepareOutputFolders();
  printProgress(Progress{0, mListOfImagePaths.size()}, Progress{0, 0});
  uint64 cnt = 0;
  for(const auto &imagePath : mListOfImagePaths) {
    std::filesystem::path path_obj(imagePath);
    std::string filename = path_obj.filename().stem().string();

    auto imgProperties = TiffLoader::getImageProperties(imagePath, dir);
    if(imgProperties.imageSize > MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE) {
      // Image too big to load at once -> Load image in tiles
      int tilesToLoadPerRun = 36;
      uint64 runs           = imgProperties.nrOfTiles / tilesToLoadPerRun;
      joda::reporting::Table tileReport;
      for(uint64 n = 0; n < runs; n++) {
        auto tilePart = TiffLoader::loadImageTile(imagePath, dir, n, tilesToLoadPerRun);
        analyze(tilePart, tileReport, outFolder, filename, n);
        printProgress(Progress{0, mListOfImagePaths.size()}, Progress{n, runs});
      }
      tileReport.flushReportToFile(outFolder + "/" + filename + "/" + "report_" + filename + ".csv");
      joda::pipeline::NucleusCounter::mergeReport(filename, mAllOverReporting, tileReport);
    } else {
      auto entireImage = TiffLoader::loadEntireImage(imagePath, dir);
      analyze(entireImage, mAllOverReporting, outFolder, filename, -1);
    }
    cnt++;
    printProgress(Progress{0, mListOfImagePaths.size()}, Progress{100, 100});
  }

  mAllOverReporting.flushReportToFile(outFolder + "/report.csv");
}

///
/// \brief      Start the analyzer algorithm
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void ImageProcessor::analyze(cv::Mat &mat, joda::reporting::Table &reporting, const std::string &outFolder,
                             const std::string &imageName, int64_t tileNr) const
{
  joda::pipeline::NucleusCounter counter(outFolder, &reporting);
  counter.analyzeImage(joda::Image{.mImage = mat, .mName = imageName, .mTileNr = tileNr});
}

///
/// \brief      Prepares the outputfolder and returns the folder to use
///             Throws an exception if folder could not be created.
/// \author     Joachim Danmayr
/// \return     Foldername
///
auto ImageProcessor::prepareOutputFolders() const -> std::string
{
  auto nowString    = ::joda::helper::timeNowToString();
  auto outputFolder = mOutputfolder + "/" + nowString;

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
