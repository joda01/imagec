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
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/commands/classifier/classifier_settings.hpp"
#include "backend/commands/command.hpp"
#include "backend/commands/objects_list.hpp"
#include "backend/processor/process_context.hpp"

namespace joda::cmd::functions {

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
  Classifier(const ClassifierSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, ObjectsListMap &result) override;

private:
  /////////////////////////////////////////////////////
  const ClassifierSettings &mSettings;
};

}    // namespace joda::cmd::functions
