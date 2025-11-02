///
/// \file      object_math.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/processor/context/process_context.hpp"
#include "fill_holes_settings.hpp"

namespace joda::cmd {

///
/// \class      FillHoles
/// \author     Joachim Danmayr
/// \brief
///
class FillHoles : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  FillHoles(const settings::FillHolesSettings &);
  void execute(cv::Mat &image) override;

private:
  /////////////////////////////////////////////////////
  const settings::FillHolesSettings &mSettings;
};

}    // namespace joda::cmd
