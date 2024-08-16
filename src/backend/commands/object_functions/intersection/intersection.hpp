///
/// \file      classifier.hpp
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
#include "backend/processor/context/process_context.hpp"
#include "intersection_settings.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class Intersection : public Command
{
public:
  /////////////////////////////////////////////////////
  Intersection(const settings::IntersectionSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::IntersectionSettings &mSettings;
};

}    // namespace joda::cmd
