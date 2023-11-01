///
/// \file      channel_processor.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Processes a channel based on its type
///

#pragma once

#include <stdexcept>
#include "../logger/console_logger.hpp"
#include "../settings/channel_settings.hpp"
#include "backend/image_processing/functions/func_types.hpp"
#include "object_detection/cell_detection_ai/cell_count.hpp"
#include "object_detection/nucleus_detection_ai/nucleus_count.hpp"
#include "object_detection/spot_detection/spot_detection.hpp"
#include "image_processor.hpp"

namespace joda::algo {

///
/// \class      ChannelProcessor
/// \author     Joachim Danmayr
/// \brief      Channel processor
///
class ChannelProcessor
{
public:
  ///
  /// \brief      Executes a processing step for one channel
  /// \author     Joachim Danmayr
  /// \param[in]  imgPath Path to the image which should be analyzed
  ///
  static auto
  processChannel(const joda::settings::json::ChannelSettings &channelSetting, const std::string &imagePath,
                 uint64_t tileIndex,
                 const std::map<int32_t, joda::func::DetectionResponse> *const referenceChannelResults = nullptr)
      -> func::DetectionResponse
  {
    //
    // Detection
    //
    switch(channelSetting.getChannelInfo().getType()) {
      case settings::json::ChannelInfo::Type::NONE:
        break;
      case settings::json::ChannelInfo::Type::NUCLEUS:
        return joda::algo::ImageProcessor<::joda::algo::NucleusCounter>::executeAlgorithm(
            imagePath, channelSetting, tileIndex, referenceChannelResults);
      case settings::json::ChannelInfo::Type::SPOT_REFERENCE:
      case settings::json::ChannelInfo::Type::SPOT:
        return joda::algo::ImageProcessor<::joda::algo::SpotDetection>::executeAlgorithm(
            imagePath, channelSetting, tileIndex, referenceChannelResults);
        break;
      case settings::json::ChannelInfo::Type::CELL:
        return joda::algo::ImageProcessor<::joda::algo::CellCounter>::executeAlgorithm(
            imagePath, channelSetting, tileIndex, referenceChannelResults);
      case settings::json::ChannelInfo::Type::BACKGROUND:
        break;
    }

    throw std::invalid_argument("Channel type not supported!");
  }
};

}    // namespace joda::algo
