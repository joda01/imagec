///
/// \file      container_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qwidget.h>
#include "backend/settings/pipeline_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

///
/// \class      ContainerBase
/// \author     Joachim Danmayr
/// \brief      Base container class for channels and virtual channels
///
class ContainerBase
{
public:
  ContainerBase()
  {
  }
  virtual ~ContainerBase()
  {
  }

  struct ConvertedChannels
  {
    std::optional<nlohmann::json> channelSettings;
    std::optional<nlohmann::json> pipelineStepVoronoi;
  };

  [[nodiscard]] virtual ConvertedChannels toJson() const                          = 0;
  virtual void fromJson(std::optional<joda::settings::json::ChannelSettings>,
                        std::optional<joda::settings::json::PipelineStepVoronoi>) = 0;
  virtual QWidget *getOverviewPanel()                                             = 0;
  virtual QWidget *getEditPanel()                                                 = 0;
  virtual void setActive(bool)                                                    = 0;
  virtual std::tuple<std::string, float> getMinColocFactor()                      = 0;
  virtual void setMinColocFactor(const std::string &group, float newValue)        = 0;
};

}    // namespace joda::ui::qt
