///
/// \file      image_processor_base.hpp
/// \author    Joachim Danmayr
/// \date      2023-04-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <vector>
#include "helper/helper.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "pipelines/pipeline.hpp"
#include "reporting/reporting.h"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

///
/// \class      ImageProcessorBase
/// \author     Joachim Danmayr
/// \brief      Base methods for the image processor
///
class ImageProcessorBase
{
public:
  /////////////////////////////////////////////////////
  explicit ImageProcessorBase(const std::string &&inputFolder, const std::string &&outputFolder);
  void start();

protected:
  /////////////////////////////////////////////////////
  [[nodiscard]] auto getListOfImagePaths() const -> const std::vector<std::string> &;
  [[nodiscard]] auto getOutputFolder() const -> const std::string &;
  [[nodiscard]] auto getAllOverReporting() -> joda::reporting::Table &;

private:
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> ALLOWED_EXTENSIONS = {"tif", "tiff", "btif", "btiff", "btf"};

  /////////////////////////////////////////////////////
  void lookForImagesInFolderAndSubfolder();
  virtual void analyzeAllImages() = 0;

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  std::vector<std::string> mListOfImagePaths;
  joda::reporting::Table mAllOverReporting;
};

}    // namespace joda::processor
