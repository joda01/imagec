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

///

#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/image/image.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/helper/threading/threading.hpp"
#include "backend/processor/processor.hpp"

namespace joda::settings {
class AnalyzeSettings;
class ProjectImageSetup;
}    // namespace joda::settings

namespace joda::ctrl {

struct Preview
{
  struct PreviewCount
  {
    int32_t count = 0;
    std::string color;
  };

  joda::image::Image thumbnail;
  joda::image::Image previewImage;
  joda::image::Image originalImage;
  std::map<settings::ClassificatorSettingOut, PreviewCount> foundObjects;
  int height;
  int width;
  std::string imageFileName;
};

///
/// \class      Controller
/// \author     Joachim Danmayr
/// \brief      Pipeline controller
///
class Controller
{
public:
  Controller() = default;
  ~Controller();

  // SYSTEM ///////////////////////////////////////////////////
  static auto getSystemResources() -> joda::system::SystemResources;
  static auto calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::filesystem::path &file, int nrOfFiles)
      -> joda::thread::ThreadingSettings;
  auto calcOptimalThreadNumber(const settings::AnalyzeSettings &settings) -> joda::thread::ThreadingSettings;
  // FILES ///////////////////////////////////////////////////
  auto getNrOfFoundImages() -> uint32_t;
  auto getListOfFoundImages() -> const std::map<uint8_t, std::vector<std::filesystem::path>> &;
  bool isLookingForImages();
  void stopLookingForFiles();
  void setWorkingDirectory(uint8_t plateNr, const std::filesystem::path &dir);
  void registerImageLookupCallback(const std::function<void(joda::filesystem::State)> &lookingForFilesFinished);

  // PREVIEW ///////////////////////////////////////////////////
  void preview(const settings::ProjectImageSetup &imageSetup, const processor::PreviewSettings &previewSettings,
               const settings::AnalyzeSettings &settings, const settings::Pipeline &pipeline, const std::filesystem::path &imagePath, int32_t tileX,
               int32_t tileY, Preview &previewOut, const joda::ome::OmeInfo &, const settings::ObjectInputClusters &clustersClassesToShow);
  [[nodiscard]] static auto getImageProperties(const std::filesystem::path &image, int series = 0) -> joda::ome::OmeInfo;

  // FLOW CONTROL ///////////////////////////////////////////////////
  void start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings, const std::string &jobName);
  void stop();
  [[nodiscard]] auto getState() const -> const joda::processor::ProcessProgress &;
  [[nodiscard]] const processor::ProcessInformation &getJobInformation() const;

private:
  /////////////////////////////////////////////////////
  processor::imagesList_t mWorkingDirectory;
  std::unique_ptr<processor::Processor> mActProcessor;
  std::thread mActThread;
};

}    // namespace joda::ctrl
