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
#include <string>
#include <vector>
#include "reporting/reporting.h"
#include <opencv2/core/mat.hpp>

class ImageProcessor
{
public:
  /////////////////////////////////////////////////////
  explicit ImageProcessor(const std::string &&ouputFolder, const std::vector<std::string> &&listOfImagePaths);
  void start();

private:
  /////////////////////////////////////////////////////
  static constexpr int64_t MAX_IMAGE_SIZE_TO_OPEN_AT_ONCE = 0;

  /////////////////////////////////////////////////////
  [[nodiscard]] auto prepareOutputFolders() const -> std::string;
  void analyze(cv::Mat &mat, joda::reporting::Table &reporting, const std::string &outFolder,
               const std::string &imageName, int64_t tileNr) const;

  /////////////////////////////////////////////////////
  std::string mOutputfolder;
  std::vector<std::string> mListOfImagePaths;
  joda::reporting::Table mAllOverReporting;
};
