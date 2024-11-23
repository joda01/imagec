///
/// \file      object_math.hpp
/// \author    Joachim Danmayr
/// \date      2024-10-11
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
#include "objects_to_image_settings.hpp"

namespace joda::cmd {

///
/// \class      ObjectsToImage
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class ObjectsToImage : public Command
{
public:
  /////////////////////////////////////////////////////
  ObjectsToImage(const settings::ObjectsToImageSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::ObjectsToImageSettings &mSettings;
};

}    // namespace joda::cmd
