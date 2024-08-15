///
/// \file      command.hpp
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

#include <memory>
#include "backend/artifacts/object_list/object_list.hpp"
#include "backend/processor/context/process_context.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::cmd {

class Command
{
public:
  void operator()(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result);

private:
  void preCommandStep(const processor::ProcessContext &context);
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) = 0;
  void postCommandStep(const processor::ProcessContext &context);
};

}    // namespace joda::cmd
