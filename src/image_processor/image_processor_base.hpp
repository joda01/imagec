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
#include <future>
#include <set>
#include <string>
#include <tuple>
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
  struct Progress
  {
    uint32_t finished = 0;
    uint32_t total    = 0;
  };

  /////////////////////////////////////////////////////
  explicit ImageProcessorBase(const std::string &inputFolder, const std::string &outputFolder);
  auto start() -> std::future<void> &;
  [[nodiscard]] auto getFuture() -> std::future<void> &;
  [[nodiscard]] auto isFinished() const -> bool;

  void wait();
  void stop();
  auto getProgress() const -> std::tuple<Progress, Progress>;
  virtual auto getReportFilePath() const -> const std::string & = 0;

protected:
  /////////////////////////////////////////////////////
  [[nodiscard]] auto getListOfImagePaths() const -> const std::vector<std::string> &;
  [[nodiscard]] auto getOutputFolder() const -> const std::string &;
  [[nodiscard]] auto getAllOverReporting() -> joda::reporting::Table &;
  [[nodiscard]] auto isStopped() const -> bool
  {
    return mStopped;
  }
  void setTotalImages(uint32_t total);
  void setProgressTotal(uint32_t finished);
  void setTotalActImages(uint32_t total);
  void setProgressActImage(uint32_t finished);

private:
  /////////////////////////////////////////////////////
  static inline const std::set<std::string> ALLOWED_EXTENSIONS = {".tif", ".tiff", ".btif", ".btiff", ".btf"};

  /////////////////////////////////////////////////////
  void mainThread();
  void lookForImagesInFolderAndSubfolder();
  virtual void analyzeAllImages() = 0;

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  std::vector<std::string> mListOfImagePaths;
  joda::reporting::Table mAllOverReporting;
  Progress mProgressTotal;
  Progress mProgressActImage;
  bool mStopped = false;
  std::future<void> mFuture;
};

}    // namespace joda::processor
