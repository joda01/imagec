///
/// \file      command.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "command.hpp"
#include <exception>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::cmd {
void Command::operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
  const auto processName = std::string(typeid(*this).name());
  DurationCount durationCount("Exec: " + processName);
  preCommandStep(context);
  try {
    execute(context, image, result);
  } catch(const std::exception &ex) {
    joda::log::logError("Cmd: >" + processName + "< failed in execution. Got >" + std::string(ex.what()) + "<");
  }
  postCommandStep(context);
}

void Command::preCommandStep(const processor::ProcessContext & /*context*/)
{
}

void Command::postCommandStep(const processor::ProcessContext & /*context*/)
{
}

void ImageProcessingCommand::operator()(cv::Mat &image)
{
  const auto processName = std::string(typeid(*this).name());
  DurationCount durationCount("Exec: " + processName);
  try {
    execute(image);
  } catch(const std::exception &ex) {
    joda::log::logError("Cmd: >" + processName + "< failed in execution. Got >" + std::string(ex.what()) + "<");
  }
}

}    // namespace joda::cmd
