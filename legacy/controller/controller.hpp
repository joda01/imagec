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

#include <cstdint>
#include "backend/helper/directory_iterator.hpp"
#include "backend/helper/file_info_images.hpp"
#include "backend/image_processing/image/image.hpp"
#include "backend/pipelines/pipeline_factory.hpp"

namespace joda::ctrl {

struct PreviewSettings
{
  int lowerBrightness = 0;
  int upperBrightness = UINT16_MAX;
};

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
             const std::string &analyzeName);
  void stop();
  void reset();
  std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State, std::string> getState();
  auto getNrOfFoundImages() -> uint32_t;
  auto getListOfFoundImages() -> const std::vector<helper::fs::FileInfoImages> &;
  bool isLookingForFiles();
  void stopLookingForFiles();
  void getSettings();
  void setWorkingDirectory(const std::string &dir);
  void registerWorkingDirectoryCallback(const std::function<void(joda::helper::fs::State)> &lookingForFilesFinished)
  {
    mWorkingDirectory.addListener(lookingForFilesFinished);
  }
  struct Preview
  {
    joda::image::Image thumbnail;
    joda::image::Image previewImage;
    joda::image::Image originalImage;
    int height;
    int width;
    std::unique_ptr<joda::image::detect::DetectionResults> detectionResult;
    std::string imageFileName;
  };
  void preview(const settings::ChannelSettings &settings, int32_t imgIndex, int32_t tileX, int32_t tileY,
               uint16_t resolution, Preview &previewOut);
  auto getImageProperties(int imgIndex, int series) -> std::tuple<joda::ome::OmeInfo, std::filesystem::path>;
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

  struct JobInformation
  {
    std::filesystem::path ouputFolder;
    std::filesystem::path resultsFilePath;
    std::string jobName;
    std::chrono::system_clock::time_point timestamp;
  };

  [[nodiscard]] JobInformation getJobInformation() const;

private:
  /////////////////////////////////////////////////////
  joda::helper::fs::DirectoryWatcher<helper::fs::FileInfoImages> mWorkingDirectory;
  std::string mActProcessId;
};

}    // namespace joda::ctrl