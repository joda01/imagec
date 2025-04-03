///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <memory>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/setting.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

class Command
{
public:
  void operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result);
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) = 0;

protected:
  void TRACE(const std::string &what) const
  {
    const auto name = std::string(typeid(*this).name());
    joda::log::logTrace(static_cast<std::string>(name + "::" + what));
  }

  void INFO(const std::string &what) const
  {
    const auto name = std::string(typeid(*this).name());
    joda::log::logInfo(static_cast<std::string>(name + "::" + what));
  }

  void WARN(const std::string &what) const
  {
    const auto name = std::string(typeid(*this).name());
    joda::log::logWarning(static_cast<std::string>(name + "::" + what));
  }

  void THROW(const std::string &what) const
  {
    const auto name = std::string(typeid(*this).name());
    throw std::invalid_argument(static_cast<std::string>(name + "::" + what));
  }

private:
  void preCommandStep(const processor::ProcessContext &context);
  void postCommandStep(const processor::ProcessContext &context);
};

}    // namespace joda::cmd
