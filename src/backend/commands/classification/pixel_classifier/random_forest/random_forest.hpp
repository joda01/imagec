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

#include "backend/commands/classification/pixel_classifier/pixel_classifier_interface.hpp"
#include "backend/commands/command.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class RandomForest : public joda::ml::PixelClassifier
{
public:
  /////////////////////////////////////////////////////
  RandomForest();
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;
  void train(const cv::Mat &image, const std::set<joda::enums::ClassId> &classesToTrain, const atom::ObjectMap &regionOfInterest,
             const std::filesystem::path &trainedModelOutputFile) override;

private:
  /////////////////////////////////////////////////////
  static void prepareTrainingDataFromROI(const cv::Mat &image, const std::set<joda::enums::ClassId> &classesToTrain,
                                         const atom::ObjectMap &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels);
  static cv::Ptr<cv::ml::RTrees> trainRandomForest(const cv::Mat &featList, const cv::Mat &labelList);

  static cv::Mat extractFeatures(const cv::Mat &img);
};

}    // namespace joda::cmd
