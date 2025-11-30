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
#include "fft_bandpass_settings.hpp"

namespace joda::cmd {

///
/// \class      FFTBandpass
/// \author     Joachim Danmayr
/// \brief
///
class FFTBandpass : public ImageProcessingCommand
{
public:
  /////////////////////////////////////////////////////
  FFTBandpass(const settings::FFTBandpassSettings &settings);
  void execute(cv::Mat &image) override;

private:
  void filter(cv::Mat &ip);

  /////////////////////////////////////////////////////
  const settings::FFTBandpassSettings &mSettings;
};

}    // namespace joda::cmd
