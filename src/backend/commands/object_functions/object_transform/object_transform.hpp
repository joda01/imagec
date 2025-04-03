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
#include "object_transform_settings.hpp"

namespace joda::cmd {

///
/// \class      ObjectTransform
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class ObjectTransform : public Command
{
public:
  /////////////////////////////////////////////////////
  ObjectTransform(const settings::ObjectTransformSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::ObjectTransformSettings &mSettings;
};

}    // namespace joda::cmd
