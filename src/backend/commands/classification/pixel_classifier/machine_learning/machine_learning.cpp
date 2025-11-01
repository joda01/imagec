///
/// \file      machine_learning.cpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "machine_learning.hpp"
#include <fstream>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::forward(const std::filesystem::path &path, cv::Mat &image, const MachineLearningSettings &settings)
{
  auto featuresIn = settings.features;
  if(!settings.outPath.filename().string().ends_with(joda::fs::MASCHINE_LEARNING_PYTORCH_JSON_MODEL)) {
    featuresIn.emplace(TrainingFeatures::Intensity);
    featuresIn.emplace(TrainingFeatures::Gaussian);
  }

  // Load trained model
  const cv::Mat features = extractFeatures(image, featuresIn, getModelType() == ModelType::ANN_MLP);
  cv::Mat predFloat;    // output (H*W) x 1, CV_32F
  predict(path, image, features, predFloat, path);

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
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::train(const MachineLearningSettings &settings, const cv::Mat &image, const enums::TileInfo &tileInfo,
                            const atom::ObjectList &result)
{
  cv::Mat trainSamples;
  cv::Mat labelList;

  auto trainingClasses = settings.toTrainingsClassesMap();
  prepareTrainingDataFromROI(image, tileInfo, trainingClasses, settings.categoryToTrain, result, trainSamples, labelList, settings.features,
                             getModelType() == ModelType::ANN_MLP);

  train(trainSamples, labelList, trainingClasses.size(), settings.outPath);
  if(!settings.outPath.filename().string().ends_with(joda::fs::MASCHINE_LEARNING_PYTORCH_JSON_MODEL)) {
    saveModel(settings.outPath, settings);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::saveModel(const std::filesystem::path &path, const MachineLearningSettings &settings)
{
  MachineLearningSettings parsed;
  parsed.classLabels  = settings.classLabels;
  parsed.features     = settings.features;
  parsed.meta         = settings.meta;
  parsed.modelTyp     = getModelType();
  parsed.framework    = getFramework();
  nlohmann::json json = parsed;
  removeNullValues(json);
  std::string metaData = json.dump(2);
  if(!metaData.empty()) {
    metaData.erase(0, 1);    // remove 1 character at position 0 this is a '{'
  }

  // Append meta information

  // Open file in read/write mode
  std::fstream file(path.string(), std::ios::in | std::ios::out);
  if(!file) {
    return;
  }

  // Go to end to get file size
  file.seekg(0, std::ios::end);
  std::streamoff size = file.tellg();

  // Read backwards to find the last '}'
  char ch;
  std::streamoff pos = size - 1;
  bool found         = false;

  while(pos >= 0) {
    file.seekg(pos);
    file.get(ch);
    if(ch == '}') {
      found = true;
      break;
    }
    --pos;
  }

  if(found) {
    // Move output pointer to that position
    file.seekp(pos);
    // Overwrite the '}' with ','
    file.put(',');
  }

  file << metaData;
  file.close();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void MachineLearning::prepareTrainingDataFromROI(const cv::Mat &image, const enums::TileInfo &tileInfo,
                                                 const std::map<enums::ClassId, int32_t> &classesToTrain, joda::atom::ROI::Category categoryToTain,
                                                 const atom::ObjectList &regionOfInterest, cv::Mat &trainSamples, cv::Mat &trainLabels,
                                                 const std::set<TrainingFeatures> &featuresSet, bool normalizeForMLP)
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
cv::Mat MachineLearning::extractFeatures(const cv::Mat &img, const std::set<TrainingFeatures> &features, bool normalizeForMLP)
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
  if(features.contains(TrainingFeatures::Intensity)) {
    featureMaps.push_back(gray.clone());
  }

  // --- Gaussian smoothed ---
  if(features.contains(TrainingFeatures::Gaussian)) {
    cv::Mat gauss;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    featureMaps.push_back(gauss);
  }

  // --- Laplacian of Gaussian ---
  if(features.contains(TrainingFeatures::LaplacianOfGaussian)) {
    cv::Mat gauss;
    cv::Mat log;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    cv::Laplacian(gauss, log, CV_32F, 3);
    featureMaps.push_back(log);
  }

  // --- Weighted deviation (Gaussian-weighted std) ---
  if(features.contains(TrainingFeatures::WeightedDeviation)) {
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
  if(features.contains(TrainingFeatures::GradientMagnitude)) {
    cv::Mat gx;
    cv::Mat gy;
    cv::Mat mag;
    cv::Sobel(gray, gx, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gy, CV_32F, 0, 1, 3);
    cv::magnitude(gx, gy, mag);
    featureMaps.push_back(mag);
  }

  // --- Structure tensor eigenvalues & coherence ---
  if(features.contains(TrainingFeatures::StructureTensorEigenvalues) || features.contains(TrainingFeatures::StructureTensorCoherence)) {
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

    if(features.contains(TrainingFeatures::StructureTensorEigenvalues)) {
      featureMaps.push_back(l1);
      featureMaps.push_back(l2);
    }

    if(features.contains(TrainingFeatures::StructureTensorCoherence)) {
      cv::Mat coherence = (l1 - l2) / (l1 + l2 + 1e-6);
      featureMaps.push_back(coherence);
    }
  }

  // --- Hessian determinant & eigenvalues ---
  if(features.contains(TrainingFeatures::HessianDeterminant) || features.contains(TrainingFeatures::HessianEigenvalues)) {
    cv::Mat dxx;
    cv::Mat dyy;
    cv::Mat dxy;
    cv::Sobel(gray, dxx, CV_32F, 2, 0, 3);
    cv::Sobel(gray, dyy, CV_32F, 0, 2, 3);
    cv::Sobel(gray, dxy, CV_32F, 1, 1, 3);

    if(features.contains(TrainingFeatures::HessianDeterminant)) {
      cv::Mat detH = dxx.mul(dyy) - dxy.mul(dxy);
      featureMaps.push_back(detH);
    }

    if(features.contains(TrainingFeatures::HessianEigenvalues)) {
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

}    // namespace joda::ml
