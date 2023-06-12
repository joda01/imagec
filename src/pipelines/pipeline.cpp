///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "pipeline.hpp"
#include <algorithm>
#include <filesystem>
#include "helper/helper.hpp"

namespace joda::pipeline {

using namespace std;
using namespace std::filesystem;

Pipeline::Pipeline(const joda::settings::json::AnalyzeSettings &settings) : mAnalyzeSettings(settings)
{
}

///
/// \brief      Runs the pipeline.
///             Iterates over all found images and executes
///             the pipeline for each image by calling the
///             pure virtual function >execute< which must
///             be defined in one of the derived classes.
/// \author     Joachim Danmayr
/// \return
///
void Pipeline::runJob(const std::string &inputFolder)
{
  mState = State::RUNNING;
  // Prepare
  mOutputFolder = prepareOutputFolder(inputFolder);
  lookForImagesInFolderAndSubfolder(inputFolder);

  // Iterate over each image
  for(const auto &imagePath : mListOfImagePaths) {
    execute(imagePath, mOutputFolder, mAllOverReporting, &mProgress.image);
    mProgress.total.finished++;
    if(mStop) {
      break;
    }
  }

  std::string resultsFile = mOutputFolder + std::filesystem::path::preferred_separator + "results.csv";
  mAllOverReporting.flushReportToFile(resultsFile);
  mState = State::FINISHED;
}

///
/// \brief      Creates the output folder for the results and returns the path.
///             Outputfolder = <inputFolder>/results/<DATE-TIME>
/// \author     Joachim Danmayr
/// \param[in]  inputFolder Inputfolder of the images
/// \return     Outputfolder of the results
///
[[nodiscard]] auto Pipeline::prepareOutputFolder(const std::string &inputFolder) -> std::string
{
  auto nowString    = ::joda::helper::timeNowToString();
  auto outputFolder = inputFolder + std::filesystem::path::preferred_separator + "results" +
                      std::filesystem::path::preferred_separator + nowString;

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

///
/// \brief      Find all images in the given infolder and its subfolder.
/// \author     Joachim Danmayr
///
void Pipeline::lookForImagesInFolderAndSubfolder(const std::string &inputFolder)
{
  mListOfImagePaths.clear();

  for(recursive_directory_iterator i(inputFolder), end; i != end; ++i) {
    if(!is_directory(i->path())) {
      if(ALLOWED_EXTENSIONS.contains(i->path().extension())) {
        mListOfImagePaths.push_back(i->path());
        mProgress.total.total = mListOfImagePaths.size();
      }
    }
    if(shouldThreadBeStopped()) {
      break;
    }
  }
}

}    // namespace joda::pipeline
