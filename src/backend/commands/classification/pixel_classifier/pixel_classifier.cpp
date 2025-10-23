///
/// \file      pixel_classifier.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "pixel_classifier.hpp"
#include <chrono>
#include <stdexcept>
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/helper.hpp"
#include <opencv2/ml.hpp>

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
PixelClassifier::PixelClassifier(const settings::PixelClassifierSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList & /*result*/)
{
  // Load trained model
  std::set<joda::settings::PixelClassifierFeatures> featuresSet;
  settings::PixelClassifierMethod method;
  const auto absoluteModelPath = std::filesystem::weakly_canonical(context.getWorkingDirectory() / mSettings.modelPath);
  auto model                   = loadModel(absoluteModelPath, featuresSet, method);

  // ===============================
  // Extract features for each pixel
  // ===============================
  cv::Mat features = extractFeatures(image, featuresSet, method == settings::PixelClassifierMethod::ANN_MLP);

  if(features.type() != CV_32F || !features.isContinuous()) {
    features.convertTo(features, CV_32F);
  }

  // ===============================
  // Predict labels
  // ===============================
  cv::Mat predFloat;    // output (H*W) x 1, CV_32F
  if(method == settings::PixelClassifierMethod::RTrees) {
    // RF: directly gives class IDs
    model->predict(features, predFloat);
  } else if(method == settings::PixelClassifierMethod::KNearest) {
    // KN: directly gives class IDs
    model->predict(features, predFloat);
  } else if(method == settings::PixelClassifierMethod::ANN_MLP) {
    // MLP: outputs scores -> take argmax
    cv::Mat scores;    // (H*W) x numClasses, CV_32F
    model->predict(features, scores);
    predFloat.create(scores.rows, 1, CV_32F);
    for(int i = 0; i < scores.rows; i++) {
      cv::Point maxLoc;
      cv::minMaxLoc(scores.row(i), nullptr, nullptr, nullptr, &maxLoc);
      predFloat.at<float>(i, 0) = static_cast<float>(maxLoc.x);
    }
  }

  // ===============================
  // Reshape back to segmentation mask
  // ===============================
  cv::Mat segMask = predFloat.reshape(1, image.rows);    // H x W, CV_32F
  segMask.convertTo(segMask, CV_16UC1);

  // Build binary mask (assuming class 0 = background, >0 = foreground)
  image        = cv::Mat::zeros(segMask.size(), CV_16U);
  cv::Mat mask = (segMask > 0);
  image.setTo(0, ~mask);
  segMask.copyTo(image, mask);

  // Debug output
  //  cv::imwrite("tmp/segmentation_02.png", image);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::train(const cv::Mat &image, const enums::TileInfo &tileInfo, const atom::ObjectList &result,
                            const settings::PixelClassifierTrainingSettings &trainingSettings)
{
  if(trainingSettings.features.empty()) {
    throw std::invalid_argument("At least one feature must be selected!");
  }

  cv::Mat trainSamples;
  cv::Mat labelList;

  switch(trainingSettings.method) {
    case settings::PixelClassifierMethod::RTrees: {
      prepareTrainingDataFromROI(image, tileInfo, trainingSettings.trainingClasses, trainingSettings.categoryToTrain, result, trainSamples, labelList,
                                 trainingSettings.features, false);

      auto statsModel = trainRandomForest(trainingSettings.randomForest, trainSamples, labelList);
      storeModel(statsModel, trainingSettings.outPath, trainingSettings.features, trainingSettings.trainingClasses);

    } break;
    case settings::PixelClassifierMethod::DTrees:
    case settings::PixelClassifierMethod::Boost:
    case settings::PixelClassifierMethod::SVM:
    case settings::PixelClassifierMethod::SVMSGD:
    case settings::PixelClassifierMethod::ANN_MLP: {
      prepareTrainingDataFromROI(image, tileInfo, trainingSettings.trainingClasses, trainingSettings.categoryToTrain, result, trainSamples, labelList,
                                 trainingSettings.features, true);

      auto statsModel = trainAnnMlp(trainingSettings.annMlp, trainSamples, labelList, static_cast<int32_t>(trainingSettings.trainingClasses.size()));
      storeModel(statsModel, trainingSettings.outPath, trainingSettings.features, trainingSettings.trainingClasses);

    } break;
    case settings::PixelClassifierMethod::KNearest: {
      prepareTrainingDataFromROI(image, tileInfo, trainingSettings.trainingClasses, trainingSettings.categoryToTrain, result, trainSamples, labelList,
                                 trainingSettings.features, true);
      auto statsModel = trainAnnMlp(trainingSettings.annMlp, trainSamples, labelList, static_cast<int32_t>(trainingSettings.trainingClasses.size()));
      storeModel(statsModel, trainingSettings.outPath, trainingSettings.features, trainingSettings.trainingClasses);
    } break;
    case settings::PixelClassifierMethod::NormalBayes:
    case settings::PixelClassifierMethod::LogisticRegression:
    case settings::PixelClassifierMethod::EM:
      break;
    case settings::PixelClassifierMethod::Unknown:
      break;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::RTrees> PixelClassifier::trainRandomForest(const joda::settings::RandomForestTrainingSettings &settings, const cv::Mat &trainSamples,
                                                           const cv::Mat &trainLabels)
{
  cv::Ptr<cv::ml::RTrees> rf = cv::ml::RTrees::create();
  rf->setMaxDepth(settings.maxTreeDepth);
  rf->setMinSampleCount(settings.minSampleCount);
  rf->setRegressionAccuracy(settings.regressionAccuracy);
  rf->setUseSurrogates(false);
  rf->setMaxCategories(10);
  rf->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, settings.maxNumberOfTrees, settings.terminationEpsilon));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  rf->train(td);
  return rf;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::ANN_MLP> PixelClassifier::trainAnnMlp(const joda::settings::AnnMlpTrainingSettings &settings, const cv::Mat &trainSamples,
                                                      const cv::Mat &trainLabels, int32_t numClasses)
{
  cv::Mat labelsOneHot = cv::Mat::zeros(trainLabels.rows, numClasses, CV_32F);
  for(int32_t i = 0; i < trainLabels.rows; i++) {
    int32_t cls                    = trainLabels.at<int>(i, 0);
    labelsOneHot.at<float>(i, cls) = 1.0F;
  }
  cv::Ptr<cv::ml::ANN_MLP> mlp = cv::ml::ANN_MLP::create();

  // Collect layer sizes: [input, hidden..., output]
  std::vector<int32_t> layers;
  layers.push_back(trainSamples.cols);    // Input layer
  for(int neurons : settings.neuronsLayer) {
    if(neurons > 0) {
      layers.push_back(neurons);    // Add hidden layer if > 0
    }
  }
  layers.push_back(numClasses);    // Output layer

  cv::Mat layerSizes(1, static_cast<int>(layers.size()), CV_32S, layers.data());
  layerSizes = layerSizes.clone();    // clone, because .data() would go out of scope
  mlp->setLayerSizes(layerSizes);
  mlp->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  mlp->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.001);    // learning rate
  mlp->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, settings.maxIterations, settings.terminationEpsilon));
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, labelsOneHot);
  mlp->train(td);
  return mlp;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::KNearest> PixelClassifier::trainKNearest(const joda::settings::KNearestTrainingSettings &settings, const cv::Mat &trainSamples,
                                                         const cv::Mat &trainLabels)
{
  cv::Ptr<cv::ml::KNearest> rf = cv::ml::KNearest::create();
  rf->setDefaultK(settings.defaultK);
  rf->setIsClassifier(true);
  cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(trainSamples, cv::ml::ROW_SAMPLE, trainLabels);
  rf->train(td);
  return rf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
template <CvModel_t MODEL>
void PixelClassifier::storeModel(cv::Ptr<MODEL> model, const std::filesystem::path &path,
                                 const std::set<joda::settings::PixelClassifierFeatures> &features,
                                 const std::map<enums::ClassId, int32_t> &trainingClasses)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::WRITE | cv::FileStorage::FORMAT_JSON);
  fs << "model"
     << "{";
  model->write(fs);
  fs << "}";

  // Convert set of enums to JSON array of strings
  nlohmann::json j_features = nlohmann::json::array();
  for(auto f : features) {
    j_features.push_back(f);    // uses NLOHMANN_JSON_SERIALIZE_ENUM mapping
  }
  // Write features as an array
  fs << "features"
     << "[";
  for(auto f : features) {
    std::string name = nlohmann::json(f).get<std::string>();    // enum -> string
    fs << name;
  }
  fs << "]";

  fs << "meta"
     << "{";
  fs << "author"
     << "";
  fs << "organization"
     << "";
  fs << "group"
     << "";
  fs << "modifiedAt" << helper::timepointToIsoString(std::chrono::system_clock::now());
  fs << "name"
     << "";
  fs << "notes"
     << "";
  fs << "revision"
     << "";
  fs << "tags"
     << "["
     << "]";
  fs << "uid"
     << "";
  fs << "}";

  fs << "classLabels"
     << "[";

  for(const auto &[_, classsId] : trainingClasses) {
    fs << "{";
    fs << "classId" << classsId;
    fs << "}";
  }

  fs << "]";

  // Write model
  if constexpr(std::is_same_v<cv::ml::RTrees, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::RTrees).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::NormalBayesClassifier, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::NormalBayes).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::KNearest, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::KNearest).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::SVM, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::SVM).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::DTrees, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::DTrees).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::Boost, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::Boost).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::ANN_MLP, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::ANN_MLP).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::LogisticRegression, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::LogisticRegression).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::SVMSGD, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::SVMSGD).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
  if constexpr(std::is_same_v<cv::ml::EM, MODEL>) {
    std::string name = nlohmann::json(joda::settings::PixelClassifierMethod::EM).get<std::string>();    // enum -> string
    fs << "modelType" << name;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Ptr<cv::ml::StatModel> PixelClassifier::loadModel(const std::filesystem::path &path, std::set<joda::settings::PixelClassifierFeatures> &features,
                                                      settings::PixelClassifierMethod &modelType)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);
  cv::FileNode fnT       = fs["modelType"];
  std::string mlModelStr = fnT.string();
  modelType              = nlohmann::json(mlModelStr).get<joda::settings::PixelClassifierMethod>();

  //
  // Load the model type
  //
  cv::Ptr<cv::ml::StatModel> loadedModel;
  switch(modelType) {
    case settings::PixelClassifierMethod::Unknown:
      throw std::runtime_error("Unknown or unsupported ML model type: " + mlModelStr);
    case settings::PixelClassifierMethod::RTrees: {
      loadedModel     = cv::ml::RTrees::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::DTrees: {
      loadedModel     = cv::ml::DTrees::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::Boost: {
      loadedModel     = cv::ml::Boost::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::SVM: {
      loadedModel     = cv::ml::SVM::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::SVMSGD: {
      loadedModel     = cv::ml::SVMSGD::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::ANN_MLP: {
      loadedModel     = cv::ml::ANN_MLP::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::KNearest: {
      loadedModel     = cv::ml::KNearest::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::NormalBayes: {
      loadedModel     = cv::ml::NormalBayesClassifier::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::LogisticRegression: {
      loadedModel     = cv::ml::LogisticRegression::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
    case settings::PixelClassifierMethod::EM: {
      loadedModel     = cv::ml::EM::create();
      cv::FileNode fn = fs["model"];
      loadedModel->read(fn);
    } break;
  }

  // Read sequence of strings
  features.clear();
  cv::FileNode fn = fs["features"];
  // fn.

  for(int i = 0; i < static_cast<int>(fn.size()); i++) {
    std::string value = static_cast<std::string>(fn[i]);
    features.insert(nlohmann::json(value).get<joda::settings::PixelClassifierFeatures>());
  }

  return loadedModel;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void PixelClassifier::prepareTrainingDataFromROI(const cv::Mat &image, const enums::TileInfo &tileInfo,
                                                 const std::map<enums::ClassId, int32_t> &classesToTrain, joda::atom::ROI::Category categoryToTain,
                                                 const atom::ObjectList &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels,
                                                 const std::set<joda::settings::PixelClassifierFeatures> &featuresSet, bool normalizeForMLP)
{
  // Extract features
  cv::Mat features = extractFeatures(image, featuresSet, normalizeForMLP);

  // Collect training samples from labeled ROI mask
  std::vector<int> labels;
  std::vector<int> sampleIdx;

  auto extractSamples = [&image, &sampleIdx, &labels](const cv::Mat &roiMask, int32_t classLabelIndex) {
    for(int y = 0; y < roiMask.rows; y++) {
      for(int x = 0; x < roiMask.cols; x++) {
        uint16_t lbl = roiMask.at<uint16_t>(y, x);
        int idx      = y * image.cols + x;
        if(lbl > 0) {    // skip unlabeled pixels
          sampleIdx.push_back(idx);
          labels.push_back(classLabelIndex);
        }
      }
    }
  };

  // ====================================
  // Train the individual classes
  // ====================================
  for(const auto [classIdToTrain, pixelClassId] : classesToTrain) {
    if(!regionOfInterest.contains(classIdToTrain)) {
      continue;
    }
    cv::Mat roiMask            = cv::Mat::zeros(image.size(), CV_16UC1);
    const auto &objectsToLearn = regionOfInterest.at(classIdToTrain);
    auto addedRois             = objectsToLearn->createBinaryImage(roiMask, 1, categoryToTain, tileInfo);
    if(addedRois > 0) {
      extractSamples(roiMask, pixelClassId);
    }
  }

  // ====================================
  // Convert labels and samples to cv::Mat
  // ====================================
  trainSamples = cv::Mat(static_cast<int>(sampleIdx.size()), features.cols, CV_32F);
  trainLabels  = cv::Mat(static_cast<int>(sampleIdx.size()), 1, CV_32S);

  for(int i = 0; i < static_cast<int>(sampleIdx.size()); i++) {
    features.row(sampleIdx[i]).copyTo(trainSamples.row(i));
    trainLabels.at<int>(i, 0) = labels[i];
  }

  if(trainSamples.empty() || trainLabels.empty()) {
    joda::log::logWarning("No training samples!");
    throw std::invalid_argument("No training samples! Did you created annotations?");
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat PixelClassifier::extractFeatures(const cv::Mat &img, const std::set<joda::settings::PixelClassifierFeatures> &features, bool normalizeForMLP)
{
  CV_Assert(!img.empty());

  cv::Mat gray;
  if(img.channels() == 3) {
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = img.clone();
  }
  gray.convertTo(gray, CV_32F);

  std::vector<cv::Mat> featureMaps;

  // --- Intensity ---
  if(features.contains(joda::settings::PixelClassifierFeatures::Intensity)) {
    featureMaps.push_back(gray.clone());
  }

  // --- Gaussian smoothed ---
  if(features.contains(joda::settings::PixelClassifierFeatures::Gaussian)) {
    cv::Mat gauss;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    featureMaps.push_back(gauss);
  }

  // --- Laplacian of Gaussian ---
  if(features.contains(joda::settings::PixelClassifierFeatures::LaplacianOfGaussian)) {
    cv::Mat gauss;
    cv::Mat log;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    cv::Laplacian(gauss, log, CV_32F, 3);
    featureMaps.push_back(log);
  }

  // --- Weighted deviation (Gaussian-weighted std) ---
  if(features.contains(joda::settings::PixelClassifierFeatures::WeightedDeviation)) {
    cv::Mat grayF;
    cv::Mat mean;
    cv::Mat meanSq;
    cv::Mat stdWeighted;

    // Convert to float to avoid overflow (16-bit * 16-bit can overflow)
    gray.convertTo(grayF, CV_32F);

    // Gaussian smoothing of image and squared image
    cv::GaussianBlur(grayF, mean, cv::Size(5, 5), 1.0);
    cv::GaussianBlur(grayF.mul(grayF), meanSq, cv::Size(5, 5), 1.0);

    // Compute standard deviation: sqrt(E[x^2] - (E[x])^2)
    cv::sqrt(meanSq - mean.mul(mean), stdWeighted);

    featureMaps.push_back(stdWeighted);
  }

  // --- Gradient magnitude (Sobel) ---
  if(features.contains(joda::settings::PixelClassifierFeatures::GradientMagnitude)) {
    cv::Mat gx;
    cv::Mat gy;
    cv::Mat mag;
    cv::Sobel(gray, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gy, CV_32F, 0, 1, 3);
    cv::magnitude(gx, gy, mag);
    featureMaps.push_back(mag);
  }

  // --- Structure tensor eigenvalues & coherence ---
  if(features.contains(joda::settings::PixelClassifierFeatures::StructureTensorEigenvalues) ||
     features.contains(joda::settings::PixelClassifierFeatures::StructureTensorCoherence)) {
    cv::Mat gx;
    cv::Mat gy;
    cv::Sobel(gray, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gy, CV_32F, 0, 1, 3);

    cv::Mat Jxx = gx.mul(gx);
    cv::Mat Jyy = gy.mul(gy);
    cv::Mat Jxy = gx.mul(gy);

    // Smooth tensor components
    cv::GaussianBlur(Jxx, Jxx, cv::Size(5, 5), 1.0);
    cv::GaussianBlur(Jyy, Jyy, cv::Size(5, 5), 1.0);
    cv::GaussianBlur(Jxy, Jxy, cv::Size(5, 5), 1.0);

    // Eigenvalues: λ1, λ2
    cv::Mat tmp = (Jxx - Jyy).mul(Jxx - Jyy) + 4 * Jxy.mul(Jxy);
    cv::sqrt(tmp, tmp);
    cv::Mat l1 = 0.5 * (Jxx + Jyy + tmp);
    cv::Mat l2 = 0.5 * (Jxx + Jyy - tmp);

    if(features.contains(joda::settings::PixelClassifierFeatures::StructureTensorEigenvalues)) {
      featureMaps.push_back(l1);
      featureMaps.push_back(l2);
    }

    if(features.contains(joda::settings::PixelClassifierFeatures::StructureTensorCoherence)) {
      cv::Mat coherence = (l1 - l2) / (l1 + l2 + 1e-6);
      featureMaps.push_back(coherence);
    }
  }

  // --- Hessian determinant & eigenvalues ---
  if(features.contains(joda::settings::PixelClassifierFeatures::HessianDeterminant) ||
     features.contains(joda::settings::PixelClassifierFeatures::HessianEigenvalues)) {
    cv::Mat dxx;
    cv::Mat dyy;
    cv::Mat dxy;
    cv::Sobel(gray, dxx, CV_32F, 2, 0, 3);
    cv::Sobel(gray, dyy, CV_32F, 0, 2, 3);
    cv::Sobel(gray, dxy, CV_32F, 1, 1, 3);

    if(features.contains(joda::settings::PixelClassifierFeatures::HessianDeterminant)) {
      cv::Mat detH = dxx.mul(dyy) - dxy.mul(dxy);
      featureMaps.push_back(detH);
    }

    if(features.contains(joda::settings::PixelClassifierFeatures::HessianEigenvalues)) {
      // Eigenvalues of Hessian
      cv::Mat tmp = (dxx - dyy).mul(dxx - dyy) + 4 * dxy.mul(dxy);
      cv::sqrt(tmp, tmp);
      cv::Mat l1 = 0.5 * (dxx + dyy + tmp);
      cv::Mat l2 = 0.5 * (dxx + dyy - tmp);
      featureMaps.push_back(l1);
      featureMaps.push_back(l2);
    }
  }

  // ---- Convert feature maps to feature matrix (pixels × features) ----

  // Stack into feature matrix
  cv::Mat featureMatrix(img.rows * img.cols, static_cast<int>(featureMaps.size()), CV_32F);
  for(int i = 0; i < static_cast<int>(featureMaps.size()); i++) {
    cv::Mat f = featureMaps[i].reshape(1, img.rows * img.cols);
    f.copyTo(featureMatrix.col(i));
  }

  // ---- Optional normalization for MLP ----
  if(normalizeForMLP) {
    cv::Mat featureMatrixNorm;
    cv::normalize(featureMatrix, featureMatrixNorm, 0, 1, cv::NORM_MINMAX);
    return featureMatrixNorm;
  }

  return featureMatrix;    // each row = pixel, each col = feature
}

template void PixelClassifier::storeModel<cv::ml::RTrees>(cv::Ptr<cv::ml::RTrees> model, const std::filesystem::path &path,
                                                          const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                          const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::NormalBayesClassifier>(cv::Ptr<cv::ml::NormalBayesClassifier> model,
                                                                         const std::filesystem::path &path,
                                                                         const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                                         const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::KNearest>(cv::Ptr<cv::ml::KNearest> model, const std::filesystem::path &path,
                                                            const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                            const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::SVM>(cv::Ptr<cv::ml::SVM> model, const std::filesystem::path &path,
                                                       const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                       const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::DTrees>(cv::Ptr<cv::ml::DTrees> model, const std::filesystem::path &path,
                                                          const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                          const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::Boost>(cv::Ptr<cv::ml::Boost> model, const std::filesystem::path &path,
                                                         const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                         const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::ANN_MLP>(cv::Ptr<cv::ml::ANN_MLP> model, const std::filesystem::path &path,
                                                           const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                           const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::LogisticRegression>(cv::Ptr<cv::ml::LogisticRegression> model, const std::filesystem::path &path,
                                                                      const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                                      const std::map<enums::ClassId, int32_t> &trainingClasses);
template void PixelClassifier::storeModel<cv::ml::SVMSGD>(cv::Ptr<cv::ml::SVMSGD> model, const std::filesystem::path &path,
                                                          const std::set<joda::settings::PixelClassifierFeatures> &features,
                                                          const std::map<enums::ClassId, int32_t> &trainingClasses);

}    // namespace joda::cmd
