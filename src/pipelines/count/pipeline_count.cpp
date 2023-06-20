///
/// \file      pipeline_count.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "pipeline_count.hpp"
#include "algorithms/cell_count_ai/cell_count.hpp"
#include "algorithms/nucleus_count_ai/nucleus_count.hpp"

namespace joda::pipeline {

///
/// \brief      Execute the analyze algorithms for counting
/// \author     Joachim Danmayr
/// \param[in]  imgPath Path to the image which should be analyzed
///
void PipelineCount::execute(const std::string &imgPath, const std::string &outputFolder,
                            joda::reporting::Table &allOverReport, types::Progress *partialProgress)
{
  auto settings = getAnalyzeSetings();

  for(const auto &[_, channel] : settings.getChannels()) {
    switch(channel.getType()) {
      case settings::json::ChannelSettings::Type::NONE:
        break;
      case settings::json::ChannelSettings::Type::NUCLEUS:
        count<::joda::algo::NucleusCounter>(imgPath, outputFolder, allOverReport, partialProgress, 14);
        break;
      case settings::json::ChannelSettings::Type::EV:
        break;
      case settings::json::ChannelSettings::Type::CELL:
        count<::joda::algo::CellCounter>(imgPath, outputFolder, allOverReport, partialProgress, 14);
        break;
      case settings::json::ChannelSettings::Type::BACKGROUND:
        break;
    }
  }
}

}    // namespace joda::pipeline
