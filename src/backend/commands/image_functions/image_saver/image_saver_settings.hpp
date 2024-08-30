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

  enum class OutputCanvas
  {
    FILE,
    IMAGE_$,
  };

  struct Cluster
  {
    struct Class
    {
      enum class Style
      {
        OUTLINED,
        FILLED
      };

      enums::ClassId classIn = enums::ClassId::UNDEFINED;
      std::string color      = "#FF0000";
      Style style            = Style::OUTLINED;
      bool paintBoundingBox  = false;

      void check() const
      {
      }

      NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Class, classIn, color, style, paintBoundingBox);
    };

    enums::ClusterIdIn clusterIn = enums::ClusterIdIn::$;
    std::vector<Class> classesIn;

    void check() const
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Cluster, clusterIn, classesIn);
  };

  std::list<Cluster> clustersIn;

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
  OutputCanvas outputCanvas = OutputCanvas::FILE;

  /////////////////////////////////////////////////////
  void check() const
  {
    if(canvas == ImageSaverSettings::Canvas::IMAGE_PLANE) {
      CHECK(planesIn.has_value(), "Define a plane on which the output should be painted!");
    }
    CHECK(compression >= 0 && compression <= 6, "Image compression must be in between [0-6].");
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(ImageSaverSettings, canvas, planesIn, compression, namePrefix,
                                                       clustersIn);
};

NLOHMANN_JSON_SERIALIZE_ENUM(ImageSaverSettings::Cluster::Class::Style,
                             {
                                 {ImageSaverSettings::Cluster::Class::Style::OUTLINED, "Outlined"},
                                 {ImageSaverSettings::Cluster::Class::Style::FILLED, "Filled"},
                             });

NLOHMANN_JSON_SERIALIZE_ENUM(ImageSaverSettings::Canvas, {
                                                             {ImageSaverSettings::Canvas::BLACK, "Black"},
                                                             {ImageSaverSettings::Canvas::WHITE, "White"},
                                                             {ImageSaverSettings::Canvas::IMAGE_$, "$"},
                                                             {ImageSaverSettings::Canvas::IMAGE_PLANE, "ImagePlane"},
                                                         });

}    // namespace joda::settings
