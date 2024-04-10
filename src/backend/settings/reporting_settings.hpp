///
/// \file      reporting_settings.hpp
/// \author
/// \date      2024-04-08
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
#include <set>
#include "backend/pipelines/reporting/reporting_defines.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings::json {

class ReportingSettings
{
public:
  class DetailReport
  {
    friend class ReportingSettings;

  public:
    auto getMeasurementChannels() const -> const std::set<uint32_t> &
    {
      return measurement;
    }

  private:
    std::set<uint32_t> measurement{
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CONFIDENCE,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::AREA_SIZE,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::PERIMETER,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CIRCULARITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::VALIDITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INVALIDITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CENTER_OF_MASS_X,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CENTER_OF_MASS_Y,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTERSECTION_CROSS_CHANNEL,
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(DetailReport, measurement);
  };
  class OverviewReport
  {
    friend class ReportingSettings;

  public:
    auto getMeasurementChannels() const -> const std::set<uint32_t> &
    {
      return measurement;
    }

  private:
    std::set<uint32_t> measurement{
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CONFIDENCE,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::AREA_SIZE,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::CIRCULARITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::VALIDITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INVALIDITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MIN_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_MAX_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTERSECTION_CROSS_CHANNEL,
    };
    ;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(OverviewReport, measurement);
  };
  class Heatmap
  {
    friend class ReportingSettings;

  public:
    auto getMeasurementChannels() const -> const std::set<uint32_t> &
    {
      return measurement;
    }

    [[nodiscard]] auto getCreateHeatmapForGroup() const -> bool
    {
      return generate_heatmap_for_plate;
    }

    [[nodiscard]] auto getCreateHeatmapForImage() const -> bool
    {
      return generate_heatmap_for_image;
    }

    [[nodiscard]] auto getCreateHeatmapForWells() const -> bool
    {
      return generate_heatmap_for_well;
    }

    [[nodiscard]] auto getImageHeatmapAreaWidth() const -> std::set<int32_t>
    {
      return image_heatmap_area_width;
    }

  private:
    std::set<uint32_t> measurement{
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::VALIDITY,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTENSITY_AVG_CROSS_CHANNEL,
        (uint32_t) joda::pipeline::reporting::MeasurementChannels::INTERSECTION_CROSS_CHANNEL,
    };

    //
    // Generate a heatmap for grouped images
    //
    bool generate_heatmap_for_plate = false;

    //
    // Generate a heatmap for a well
    //
    bool generate_heatmap_for_well = false;

    //
    // Generate a heatmap for each image
    //
    bool generate_heatmap_for_image = false;

    //
    // With of the square used for heatmap creation in image
    //
    std::set<int32_t> image_heatmap_area_width;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Heatmap, measurement, generate_heatmap_for_image,
                                                generate_heatmap_for_plate, generate_heatmap_for_well,
                                                image_heatmap_area_width);
  };

  auto getDetailReportSettings() const -> const DetailReport &
  {
    return detail;
  }

  auto getOverviewReportSettings() const -> const OverviewReport &
  {
    return overview;
  }

  auto getHeatmapSettings() const -> const Heatmap &
  {
    return heatmap;
  }

  void setReportingSettings(const std::set<uint32_t> &detail, const std::set<uint32_t> &overview,
                            const std::set<uint32_t> &heatmap)
  {
    this->detail.measurement   = detail;
    this->overview.measurement = overview;
    this->heatmap.measurement  = heatmap;
  }

private:
  /////////////////////////////////////////////////////
  DetailReport detail;
  OverviewReport overview;
  Heatmap heatmap;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ReportingSettings, detail, overview, heatmap);
};

}    // namespace joda::settings::json
