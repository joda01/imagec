#pragma once

#include <qsettings.h>
#include <cstdint>
#include <set>

#include "backend/enums/enums_units.hpp"
#include "backend/settings/setting.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

struct ProjectImageSetup
{
  enum class ZStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  enum class TStackHandling
  {
    EXACT_ONE,
    EACH_ONE
  };

  struct ImageTileSettings
  {
    //
    // Composite tile width
    //
    int32_t tileWidth = 4096;

    //
    // Composite tile height
    //
    int32_t tileHeight = 4096;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageTileSettings, tileWidth, tileHeight);
  };

  struct TStackSettings
  {
    //
    // T-stack range start time point
    //
    int32_t startFrame = 0;

    //
    // T-stack range end time point. -1 means all
    //
    int32_t endFrame = -1;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(TStackSettings, startFrame, endFrame);
  };

  struct PhysicalSizeSettings
  {
    //
    //
    //
    float pixelWidth = 0;

    //
    //
    //
    float pixelHeight = 0;

    //
    //
    //
    enums::Units unit = enums::Units::Automatic;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(PhysicalSizeSettings, pixelWidth, pixelHeight, unit);
  };

  ZStackHandling zStackHandling = ZStackHandling::EXACT_ONE;
  TStackHandling tStackHandling = TStackHandling::EACH_ONE;

  TStackSettings tStackSettings          = {};
  PhysicalSizeSettings pixelSizeSettings = {};

  //
  // If the image is too big too load at once to RAM it is loaded in tiles
  //
  ImageTileSettings imageTileSettings;

  //
  // Define which image series should be used for image loading
  //
  int32_t series = 0;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ProjectImageSetup, zStackHandling, tStackHandling, imageTileSettings, series, tStackSettings,
                                                       pixelSizeSettings);

  void check() const
  {
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::ZStackHandling,
                             {{ProjectImageSetup::ZStackHandling::EXACT_ONE, "ExactOne"}, {ProjectImageSetup::ZStackHandling::EACH_ONE, "EachOne"}});

NLOHMANN_JSON_SERIALIZE_ENUM(ProjectImageSetup::TStackHandling, {
                                                                    {ProjectImageSetup::TStackHandling::EXACT_ONE, "ExactOne"},
                                                                    {ProjectImageSetup::TStackHandling::EACH_ONE, "EachOne"},
                                                                });

}    // namespace joda::settings
