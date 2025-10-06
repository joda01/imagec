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
#include "pixel_classifier_settings.hpp"
#include "pixel_classifier_training_settings.hpp"

template <class T>
concept CvModel_t =
    std::is_base_of<cv::ml::RTrees, T>::value || std::is_base_of<cv::ml::NormalBayesClassifier, T>::value ||
    std::is_base_of<cv::ml::KNearest, T>::value || std::is_base_of<cv::ml::SVM, T>::value || std::is_base_of<cv::ml::DTrees, T>::value ||
    std::is_base_of<cv::ml::Boost, T>::value || std::is_base_of<cv::ml::ANN_MLP, T>::value || std::is_base_of<cv::ml::LogisticRegression, T>::value ||
    std::is_base_of<cv::ml::SVMSGD, T>::value || std::is_base_of<cv::ml::EM, T>::value;

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class PixelClassifier : public Command
{
public:
  /////////////////////////////////////////////////////
  PixelClassifier(const settings::PixelClassifierSettings &);
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override;
  static void train(const cv::Mat &image, const atom::ObjectList &result, const settings::PixelClassifierTrainingSettings &trainingSettings);

private:
  /////////////////////////////////////////////////////
  static void prepareTrainingDataFromROI(const cv::Mat &image, const std::map<enums::ClassId, int32_t> &classesToTrain,
                                         const atom::ObjectList &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels,
                                         const std::set<joda::settings::PixelClassifierFeatures> &featuresSet, bool normalizeForMLP);

  static cv::Mat extractFeatures(const cv::Mat &img, const std::set<joda::settings::PixelClassifierFeatures> &features, bool normalizeForMLP);

  template <CvModel_t MODEL>
  static void storeModel(cv::Ptr<MODEL> model, const std::filesystem::path &path, const std::set<joda::settings::PixelClassifierFeatures> &features,
                         const std::map<enums::ClassId, int32_t> &trainingClasses);

  static cv::Ptr<cv::ml::StatModel> loadModel(const std::filesystem::path &path, std::set<joda::settings::PixelClassifierFeatures> &features,
                                              settings::PixelClassifierMethod &modelType);

  static cv::Ptr<cv::ml::RTrees> trainRandomForest(const joda::settings::RandomForestTrainingSettings &settings, const cv::Mat &trainSamples,
                                                   const cv::Mat &trainLabels);

  static cv::Ptr<cv::ml::ANN_MLP> trainAnnMlp(const joda::settings::AnnMlpTrainingSettings &settings, const cv::Mat &trainSamples,
                                              const cv::Mat &trainLabels, int32_t numClasses);

  static cv::Ptr<cv::ml::KNearest> trainKNearest(const joda::settings::KNearestTrainingSettings &settings, const cv::Mat &trainSamples,
                                                 const cv::Mat &trainLabels);

  /////////////////////////////////////////////////////
  const settings::PixelClassifierSettings &mSettings;
};

}    // namespace joda::cmd
