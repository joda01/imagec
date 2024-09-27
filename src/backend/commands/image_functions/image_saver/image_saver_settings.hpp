#pragma once

#include <cstdint>
#include <list>
#include <optional>
#include <set>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/json_optional_parser_helper.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

static inline const std::vector<std::string> IMAGE_SAVER_COLORS = {
    "#FF5733",    // - Vibrant Orange
    "#33FF57",    // - Bright Green
    "#FF33FF",    // - Magenta
    "#33A1FF",    // - Sky Blue
    "#FFFF33",    // - Bright Yellow
    "#FF33A1",    // - Hot Pink
    "#33FFD1",    // - Aqua
    "#FF8C33",    // - Deep Orange
    "#9933FF",    // - Purple
    "#33FFCC",    // - Mint Green
    "#FF3380",    // - Electric Pink
    "#33FF88",    // - Neon Green
    "#FFCC33",    // - Golden Yellow
    "#33CFFF",    // - Light Cyan
    "#FF3366",    // - Coral Red
    "#66FF33",    // - Lime Green
    "#FF33C4",    // - Bright Rose
    "#33FF99",    // - Spring Green
    "#FF6F33",    // - Pumpkin Orange
    "#3399FF",    // - Electric Blue
};

struct ImageSaverSettings : public SettingBase
{
public:
  enum class Canvas
  {
    WHITE,
    BLACK,
    IMAGE_$,
    IMAGE_PLANE
  };

  enum class Output
  {
    FILE,
    IMAGE_$,
  };

  enum class Style
  {
    OUTLINED,
    FILLED
  };

  struct SaveCluster
  {
    ClassificatorSetting inputCluster;
    std::string color     = "#FF0000";
    Style style           = Style::OUTLINED;
    bool paintBoundingBox = false;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(SaveCluster, inputCluster, color, style, paintBoundingBox);
  };

  std::list<SaveCluster> clustersIn = {{.inputCluster = {}, .color = "#FF5733", .style = Style::OUTLINED, .paintBoundingBox = false}};

  //
  // PNG compression level (0 = no compression)
  //
  int32_t compression = 1;

  //
  // Image name prefix
  //
  std::string namePrefix;

  //
  // Where should the output be painted on
  //
  Canvas canvas = Canvas::IMAGE_$;

  //
  // On which image it should be painter
  //
  std::optional<enums::ImageId> planesIn = std::nullopt;

  //
  // Where the output should be printed to
  //
  Output outputSlot = Output::FILE;

  //
  // Place to subfolder
  //
  std::string subFolder = "images/${imageName}";

  /////////////////////////////////////////////////////
  void check() const
  {
    if(canvas == ImageSaverSettings::Canvas::IMAGE_PLANE) {
      CHECK_ERROR(planesIn.has_value(), "Define a plane on which the output should be painted!");
    }
    CHECK_ERROR(compression >= 0 && compression <= 6, "Image compression must be in between [0-6].");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageSaverSettings, subFolder, canvas, planesIn, compression, namePrefix, clustersIn);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageSaverSettings::Style, {
                                                            {ImageSaverSettings::Style::OUTLINED, "Outlined"},
                                                            {ImageSaverSettings::Style::FILLED, "Filled"},
                                                        });

NLOHMANN_JSON_SERIALIZE_ENUM(ImageSaverSettings::Canvas, {
                                                             {ImageSaverSettings::Canvas::BLACK, "Black"},
                                                             {ImageSaverSettings::Canvas::WHITE, "White"},
                                                             {ImageSaverSettings::Canvas::IMAGE_$, "$"},
                                                             {ImageSaverSettings::Canvas::IMAGE_PLANE, "ImagePlane"},
                                                         });

}    // namespace joda::settings
