#pragma once

#include <set>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "settings_base.hpp"

namespace joda::settings::json {

class FileStructureSettings : public SettingsBase
{
public:
  enum class GroupBy
  {
    OFF,
    FOLDER,
    FILENAME
  };

  [[nodiscard]] auto getGroupBy() const -> GroupBy
  {
    return group_by_enum;
  }

  [[nodiscard]] auto getGroupByString() const -> const std::string &
  {
    return group_by;
  }

  [[nodiscard]] auto getFileRegex() const -> const std::string &
  {
    return image_filename_regex;
  }

  [[nodiscard]] auto getWellImageOrder() const -> const std::vector<std::vector<int32_t>> &
  {
    return well_image_order;
  }

  void postParsing() override;

private:
  //
  // Image grouping option [NONE, FOLDER, FILENAME]
  //
  std::string group_by;
  GroupBy group_by_enum = GroupBy::OFF;

  //
  // Used to extract coordinates of a well form the image name
  // Regex with 3 groupings: _((.)([0-9]+))_
  //
  std::string image_filename_regex = "_((.)([0-9]+))_";

  //
  // Matrix of image numbers how the images are ordered in a map.
  // First dimension of the vector are the rows, second the columns
  //
  std::vector<std::vector<int32_t>> well_image_order;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(FileStructureSettings, group_by, image_filename_regex, well_image_order);
};

class AnalyzeSettingsReporting final : public SettingsBase
{
public:
  /////////////////////////////////////////////////////
  [[nodiscard]] auto getFileStructureSettings() const -> const FileStructureSettings &
  {
    return fileStructure;
  }

  void postParsing() override
  {
    fileStructure.postParsing();
  }

private:
  /////////////////////////////////////////////////////
  FileStructureSettings fileStructure;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AnalyzeSettingsReporting, fileStructure);
};
}    // namespace joda::settings::json
