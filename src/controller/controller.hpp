///
/// \file      controller.hpp
/// \author    Joachim Danmayr
/// \date      2023-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>
#include "backend/database/exporter/xlsx/exporter_xlsx.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/image/image.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/helper/system/system_resources.hpp"
#include "backend/helper/threading/threading.hpp"
#include "backend/processor/processor.hpp"

namespace joda::settings {
class AnalyzeSettings;
class ProjectImageSetup;
}    // namespace joda::settings

namespace joda::db {
class QueryFilter;
}

namespace joda::ctrl {

struct Preview
{
  struct PreviewCount
  {
    int32_t count = 0;
    std::string color;
    bool isHidden = false;
  };

  joda::image::Image thumbnail;
  joda::image::Image originalImage;
  joda::image::Image editedImage;
  joda::image::Image overlay;

  struct PreviewResults
  {
    std::map<joda::enums::ClassId, PreviewCount> foundObjects;
    bool isOverExposed = false;
    bool noiseDetected = false;
  } results;

  int height;
  int width;
  int tStacks = 1;    // Nr. of t stacks the image has.
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
  static void initApplication();
  static auto getSystemResources() -> joda::system::SystemResources;
  static auto calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::filesystem::path &file, int nrOfFiles,
                                      const std::optional<joda::ome::OmeInfo> &imageOmeInfo) -> joda::thread::ThreadingSettings;
  auto calcOptimalThreadNumber(const settings::AnalyzeSettings &settings, const std::optional<joda::ome::OmeInfo> &imageOmeInfo)
      -> joda::thread::ThreadingSettings;
  // FILES ///////////////////////////////////////////////////
  auto getNrOfFoundImages() -> uint32_t;
  auto getListOfFoundImages() const -> const std::vector<std::filesystem::path> &;
  bool isLookingForImages();
  void stopLookingForFiles();
  void setWorkingDirectory(const std::filesystem::path &dir);
  void registerImageLookupCallback(const std::function<void(joda::filesystem::State)> &lookingForFilesFinished);
  static auto populateClassesFromImage(const joda::ome::OmeInfo &omeInfo, int32_t series) -> joda::settings::Classification;

  // PREVIEW ///////////////////////////////////////////////////
  void preview(const settings::ProjectImageSetup &imageSetup, const processor::PreviewSettings &previewSettings,
               const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings, const settings::Pipeline &pipeline,
               const std::filesystem::path &imagePath, int32_t tileX, int32_t tileY, int32_t tStack, Preview &previewOut, const joda::ome::OmeInfo &,
               const settings::ObjectInputClassesExp &classesToHide);
  [[nodiscard]] static auto getImageProperties(const std::filesystem::path &image, int series) -> joda::ome::OmeInfo;

  static auto loadImage(const std::filesystem::path &imagePath, uint16_t series, const joda::image::reader::ImageReader::Plane &imagePlane,
                        const joda::ome::TileToLoad &tileLoad, Preview &previewOut, joda::ome::OmeInfo &omeOut, enums::ZProjection zProjection)
      -> void;

  static auto loadImage(const std::filesystem::path &imagePath, uint16_t series, const joda::image::reader::ImageReader::Plane &imagePlane,
                        const joda::ome::TileToLoad &tileLoad, Preview &previewOut, const joda::ome::OmeInfo *omeIn, enums::ZProjection zProjection)
      -> void;

  // FLOW CONTROL ///////////////////////////////////////////////////
  void start(const settings::AnalyzeSettings &settings, const joda::thread::ThreadingSettings &threadSettings, const std::string &jobName);
  void stop();
  [[nodiscard]] auto getState() const -> const joda::processor::ProcessProgress &;
  [[nodiscard]] const processor::ProcessInformation &getJobInformation() const;

  // EXPORT ///////////////////////////////////////

  void exportData(const std::filesystem::path &pathToDbFile, const joda::exporter::xlsx::ExportSettings &settings,
                  const std::filesystem::path &outputFilePath, const std::optional<std::list<joda::settings::Class>> &classesList);

private:
  /////////////////////////////////////////////////////
  processor::imagesList_t mWorkingDirectory;
  std::unique_ptr<processor::Processor> mActProcessor;
  std::thread mActThread;
};

}    // namespace joda::ctrl
