///
/// \file      controller.hpp
/// \author    Joachim Danmayr
/// \date      2023-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/helper/directory_iterator.hpp"
#include "backend/pipelines/pipeline_factory.hpp"

namespace joda::ctrl {

///
/// \class      Controller
/// \author     Joachim Danmayr
/// \brief      Pipeline controller
///
class Controller
{
public:
  /////////////////////////////////////////////////////
  Controller();
  void start(const settings::AnalyzeSettings &settings, const pipeline::Pipeline::ThreadingSettings &threadSettings,
             const std::string &jobName);
  void stop();
  void reset();
  std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State, std::string> getState();
  auto getNrOfFoundImages() -> uint32_t;
  auto getListOfFoundImages() -> const std::vector<FileInfo> &;
  bool isLookingForFiles();
  void stopLookingForFiles();
  void getSettings();
  void setWorkingDirectory(const std::string &dir);
  struct Preview
  {
    std::vector<uchar> data;
    int height;
    int width;
    joda::func::DetectionResults detectionResult;
    std::string imageFileName;
  };
  auto preview(const settings::ChannelSettings &settings, int imgIndex, int tileIndex) -> Preview;
  auto getImageProperties(int imgIndex, int series) -> ImageProperties;
  struct Resources
  {
    uint64_t ramTotal;    // RAM in bytes
    uint64_t ramAvailable;
    uint32_t cpus;    // Nr. of CPUs
  };
  static auto getSystemResources() -> Resources;

  auto calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, int imgIndex)
      -> pipeline::Pipeline::ThreadingSettings;
  [[nodiscard]] std::string getOutputFolder() const;

private:
  /////////////////////////////////////////////////////
  joda::helper::ImageFileContainer mWorkingDirectory;
  std::string mActProcessId;
};

}    // namespace joda::ctrl
