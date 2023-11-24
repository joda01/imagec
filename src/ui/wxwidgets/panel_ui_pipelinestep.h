///
/// \file      panel_ui_pipelinestep.h
/// \author    Joachim Danmayr
/// \date      2023-11-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/settings/pipeline_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::wxwidget {

class PanelUiPipelineStep
{
public:
  /////////////////////////////////////////////////////
  virtual void loadValues(const joda::settings::json::PipelineStepSettings &) = 0;
  virtual nlohmann::json getValues()                                          = 0;
  virtual uint64_t getUniqueID()                                              = 0;
};
}    // namespace joda::ui::wxwidget
