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
    Style style           = Style::OUTLINED;
    bool paintBoundingBox = false;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(SaveCluster, inputCluster, style, paintBoundingBox);
  };

  std::list<SaveCluster> clustersIn = {{.inputCluster = {}, .style = Style::OUTLINED, .paintBoundingBox = false}};

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

  settings::ObjectInputClusters getInputClustersAndClasses() const override
  {
    settings::ObjectInputClusters clusters;

    for(const auto &cl : clustersIn) {
      clusters.emplace(cl.inputCluster);
    }

    return clusters;
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
