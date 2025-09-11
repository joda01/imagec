///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///

#pragma once

#include <filesystem>
#include <type_traits>
#include "backend/processor/context/process_context.hpp"
#include "pixel_classifier_training_settings.hpp"

template <class T>
concept CvModel_t =
    std::is_base_of<cv::ml::RTrees, T>::value || std::is_base_of<cv::ml::NormalBayesClassifier, T>::value ||
    std::is_base_of<cv::ml::KNearest, T>::value || std::is_base_of<cv::ml::SVM, T>::value || std::is_base_of<cv::ml::DTrees, T>::value ||
    std::is_base_of<cv::ml::Boost, T>::value || std::is_base_of<cv::ml::ANN_MLP, T>::value || std::is_base_of<cv::ml::LogisticRegression, T>::value ||
    std::is_base_of<cv::ml::SVMSGD, T>::value;

namespace joda::ml {

class PixelClassifier
{
public:
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)                = 0;
  virtual void train(const settings::PixelClassifierTrainingSettings &trainingSettings, const cv::Mat &image, const std::set<int32_t> &classesToTrain,
                     const atom::ObjectList &regionOfInterest, const std::filesystem::path &trainedModelOutputFile) = 0;
  static cv::Mat extractFeatures(const cv::Mat &img, const std::set<joda::settings::PixelClassifierFeatures> &features);

  template <CvModel_t MODEL>
  static void storeModel(cv::Ptr<MODEL> model, const std::filesystem::path &path, const std::set<joda::settings::PixelClassifierFeatures> &features);

  template <CvModel_t MODEL>
  static cv::Ptr<MODEL> loadModel(const std::filesystem::path &path, std::set<joda::settings::PixelClassifierFeatures> &features);
};

}    // namespace joda::ml
