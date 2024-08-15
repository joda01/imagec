///
/// \file      measure.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include "backend/commands/command.hpp"
#include "measure_settings.hpp"

namespace joda::cmd {

class Measure : public cmd::Command
{
public:
  explicit Measure(const settings::MeasureSettings &settings) : mSettings(settings)
  {
  }

  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    auto &store = *context.loadObjectsFromCache(mSettings.objectIn);
    for(auto imageId : mSettings.planesIn) {
      auto const &image = *context.loadImageFromCache(imageId);
      for(const auto &clusterIdIn : mSettings.clustersIn) {
        auto &clusterObjects = store.at(context.getClusterId(clusterIdIn));
        for(auto &object : clusterObjects) {
          if(mSettings.classesIn.contains(object.getClassId())) {
            object.measureIntensityAndAdd(image);
          }
        }
      }
    }
  }

private:
  const settings::MeasureSettings &mSettings;
};

}    // namespace joda::cmd
