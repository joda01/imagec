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
#include "backend/commands/objects_list.hpp"
#include "backend/processor/process_context.hpp"
#include "classifier_settings.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class Classifier : public Command
{
public:
  /////////////////////////////////////////////////////
  Classifier(const settings::ClassifierSettings &);
  void execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
               ObjectsListMap &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::ClassifierSettings &mSettings;
};

}    // namespace joda::cmd
