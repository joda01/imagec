///
/// \file      random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/processor/context/process_context.hpp"
#include "random_forest_settings.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class RandomForest : public Command
{
public:
  /////////////////////////////////////////////////////
  RandomForest(const settings::RandomForestSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;
  void prepareTrainingDataFromROI(const cv::Mat &image, const std::unique_ptr<atom::SpheralIndex> &objectsToLearn, cv::Mat &samples, cv::Mat &labels);
  cv::Ptr<cv::ml::RTrees> trainRandomForest(std::vector<std::vector<float>> &featList, std::vector<int> &labelList);

private:
  /////////////////////////////////////////////////////
  const settings::RandomForestSettings &mSettings;
};

}    // namespace joda::cmd
