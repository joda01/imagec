///
/// \file      command.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
  auto id                = DurationCount::start("Exec: " + processName);
  preCommandStep(context);
  try {
    execute(context, image, result);
  } catch(const std::exception &ex) {
    joda::log::logError("Cmd: >" + processName + "< failed in execution. Got >" + std::string(ex.what()) + "<");
  }
  postCommandStep(context);
  DurationCount::stop(id);
}

void Command::preCommandStep(const processor::ProcessContext &context)
{
}

void Command::postCommandStep(const processor::ProcessContext &context)
{
}

}    // namespace joda::cmd
