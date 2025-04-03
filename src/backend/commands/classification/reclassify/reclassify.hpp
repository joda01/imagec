///
/// \file      classifier.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/processor/context/process_context.hpp"
#include "reclassify_settings.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class Reclassify : public Command
{
public:
  /////////////////////////////////////////////////////
  Reclassify(const settings::ReclassifySettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;

private:
  /////////////////////////////////////////////////////
  const settings::ReclassifySettings &mSettings;
};

}    // namespace joda::cmd
