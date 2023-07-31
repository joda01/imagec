///
/// \file      pipeline_step.hpp
/// \author    Joachim Danmayr
/// \date      2023-07-31
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A pipeline step
///

#include <map>
#include "image_processing/functions/func_types.hpp"
#include "settings/analze_settings_parser.hpp"
namespace joda::pipeline {

class PipelineStep
{
public:
  virtual void execute(const settings::json::AnalyzeSettings &, const std::map<int, joda::func::DetectionResponse> &,
                       const std::string &detailoutputPath) = 0;
};

}    // namespace joda::pipeline
