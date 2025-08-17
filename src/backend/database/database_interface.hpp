///
/// \file      database_interface.hpp
/// \author    Joachim Danmayr
/// \date      2025-03-19
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <filesystem>
#include <vector>
#include "backend/enums/enum_validity.hpp"
#include "backend/helper/file_grouper/file_grouper_types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/threadpool/thread_pool.hpp"
#include "backend/processor/context/image_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include <duckdb/main/query_result.hpp>

namespace joda::db {

struct AnalyzeMeta
{
  joda::settings::ExperimentSettings experiment;
  std::chrono::system_clock::time_point timestampStart;
  std::chrono::system_clock::time_point timestampFinish;
  std::string jobName;
  std::string jobId;
  std::string analyzeSettingsJsonString;
  uint32_t tileWidth  = 0;    // Tile width selected for analysis.
  uint32_t tileHeight = 0;    // Tile height selected for analysis.
  uint32_t series     = 0;    // Image series used for analysis.
};

struct GroupInfo
{
  std::string groupName;
  uint32_t posX = 0;
  uint32_t posY = 0;
};

struct ImageInfo
{
  std::string filename;
  std::string imageFilePath;
  std::string imageFilePathRel;
  enums::ChannelValidity validity;
  uint32_t width  = 0;
  uint32_t height = 0;
  std::string imageGroupName;
  uint64_t imageId;
  std::string physicalSizeUnit = "Px";
};

struct ObjectInfo
{
  uint32_t stackC        = 0;
  uint32_t stackZ        = 0;
  uint32_t stackT        = 0;
  uint32_t measCenterX   = 0;
  uint32_t measCenterY   = 0;
  uint32_t measBoxX      = 0;
  uint32_t measBoxY      = 0;
  uint32_t measBoxWidth  = 0;
  uint32_t measBoxHeight = 0;
  uint64_t imageId       = 0;
};

class DatabaseInterface
{
public:
  DatabaseInterface() = default;
  /////////////////////////////////////////////////////
  virtual void openDatabase(const std::filesystem::path &pathToDb)                                  = 0;
  virtual void closeDatabase()                                                                      = 0;
  virtual std::string startJob(const joda::settings::AnalyzeSettings &, const std::string &jobName) = 0;
  virtual void finishJob(const std::string &jobId)                                                  = 0;

  virtual auto prepareImages(uint8_t plateId, int32_t series, enums::GroupBy groupBy, const std::string &filenameRegex,
                             const std::vector<std::filesystem::path> &imagePaths, const std::filesystem::path &imagesBasePath,
                             const joda::settings::ProjectImageSetup::PhysicalSizeSettings &defaultPhysicalSizeSettings,
                             BS::thread_pool &globalThreadPool) -> std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>> = 0;
  virtual void setImageProcessed(uint64_t)                                                                                                      = 0;

  virtual void insertImagePlane(uint64_t imageId, const enums::PlaneId &, const ome::OmeInfo::ImagePlane &) = 0;

  virtual void setImageValidity(uint64_t imageId, enums::ChannelValidity validity)                              = 0;
  virtual void unsetImageValidity(uint64_t imageId, enums::ChannelValidity validity)                            = 0;
  virtual void setImagePlaneValidity(uint64_t imageId, const enums::PlaneId &, enums::ChannelValidity validity) = 0;
  virtual void setImagePlaneClasssClasssValidity(uint64_t imageId, const enums::PlaneId &, enums::ClassId classId,
                                                 enums::ChannelValidity validity)                               = 0;

  virtual void insertObjects(const joda::processor::ImageContext &, const joda::atom::ObjectList &) = 0;
};

///
/// \class
/// \author
/// \brief
///
class PreviewDatabase : public DatabaseInterface
{
public:
  PreviewDatabase() = default;
  void openDatabase(const std::filesystem::path &pathToDb) override
  {
  }
  void closeDatabase() override
  {
  }
  std::string startJob(const joda::settings::AnalyzeSettings &, const std::string &jobName) override
  {
    return {};
  }
  void finishJob(const std::string &jobId) override
  {
  }

  auto prepareImages(uint8_t plateId, int32_t series, enums::GroupBy groupBy, const std::string &filenameRegex,
                     const std::vector<std::filesystem::path> &imagePaths, const std::filesystem::path &imagesBasePath,
                     const joda::settings::ProjectImageSetup::PhysicalSizeSettings &defaultPhysicalSizeSettings, BS::thread_pool &globalThreadPool)
      -> std::vector<std::tuple<std::filesystem::path, joda::ome::OmeInfo, uint64_t>> override
  {
    return {};
  }
  void setImageProcessed(uint64_t) override
  {
  }

  void insertImagePlane(uint64_t imageId, const enums::PlaneId &, const ome::OmeInfo::ImagePlane &) override
  {
  }

  void setImageValidity(uint64_t imageId, enums::ChannelValidity validity) override
  {
    mImageValidity[imageId] = validity;
  }
  void unsetImageValidity(uint64_t imageId, enums::ChannelValidity validity) override
  {
    mImageValidity[imageId] = validity;
  }
  void setImagePlaneValidity(uint64_t imageId, const enums::PlaneId &, enums::ChannelValidity validity) override
  {
  }
  void setImagePlaneClasssClasssValidity(uint64_t imageId, const enums::PlaneId &, enums::ClassId classId, enums::ChannelValidity validity) override
  {
  }

  void insertObjects(const joda::processor::ImageContext &, const joda::atom::ObjectList &) override
  {
  }

  auto getImageValidity() const -> enums::ChannelValidity
  {
    if(mImageValidity.empty()) {
      return {};
    }
    return mImageValidity.begin()->second;
  }

private:
  std::map<uint64_t, enums::ChannelValidity> mImageValidity;
};

}    // namespace joda::db
