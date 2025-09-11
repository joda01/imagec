///
/// \file      pixel_classifier_interface.cpp
/// \author    Joachim Danmayr
/// \date      2025-09-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "pixel_classifier_interface.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
template <CvModel_t MODEL>
void PixelClassifier::storeModel(cv::Ptr<MODEL> model, const std::filesystem::path &path,
                                 const std::set<joda::settings::PixelClassifierFeatures> &features)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::WRITE);
  model->write(fs);

  // Now write custom feature flags
  fs << "features"
     << "{";
  fs << "useIntensity" << features.contains(joda::settings::PixelClassifierFeatures::INTENSITY);
  fs << "useGaussian" << features.contains(joda::settings::PixelClassifierFeatures::GAUSSIAN);
  fs << "useLaplacian" << features.contains(joda::settings::PixelClassifierFeatures::LAPLACIAN);
  fs << "useGradient" << features.contains(joda::settings::PixelClassifierFeatures::GRADIENT);
  fs << "useVariance" << features.contains(joda::settings::PixelClassifierFeatures::VARIANCE);
  fs << "useHessian" << features.contains(joda::settings::PixelClassifierFeatures::HESSIAN);
  fs << "}";
}

template <CvModel_t MODEL>
cv::Ptr<MODEL> PixelClassifier::loadModel(const std::filesystem::path &path, std::set<joda::settings::PixelClassifierFeatures> &features)
{
  cv::FileStorage fs(path.string(), cv::FileStorage::READ);
  cv::Ptr<cv::ml::RTrees> rtrees = cv::Algorithm::read<cv::ml::RTrees>(fs.root());

  cv::FileNode fn = fs["features"];

  auto readFlag = [&fn, &features](const std::string &key, joda::settings::PixelClassifierFeatures enumC) {
    bool tmp = false;
    fn["key"] >> tmp;
    if(tmp) {
      features.emplace(enumC);
    }
  };
  readFlag("useIntensity", joda::settings::PixelClassifierFeatures::INTENSITY);
  readFlag("useGaussian", joda::settings::PixelClassifierFeatures::GAUSSIAN);
  readFlag("useLaplacian", joda::settings::PixelClassifierFeatures::LAPLACIAN);
  readFlag("useGradient", joda::settings::PixelClassifierFeatures::GRADIENT);
  readFlag("useVariance", joda::settings::PixelClassifierFeatures::VARIANCE);
  readFlag("useHessian", joda::settings::PixelClassifierFeatures::HESSIAN);
  fs.release();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
cv::Mat PixelClassifier::extractFeatures(const cv::Mat &img, const std::set<joda::settings::PixelClassifierFeatures> &features)
{
  CV_Assert(!img.empty());

  cv::Mat gray;
  if(img.channels() == 3) {
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  } else {
    gray = img.clone();
  }

  std::vector<cv::Mat> featureMaps;

  // 1. Raw intensity
  if(features.contains(joda::settings::PixelClassifierFeatures::INTENSITY)) {
    featureMaps.push_back(gray);
  }

  // 2. Gaussian blur (sigma=1.0 as example)
  if(features.contains(joda::settings::PixelClassifierFeatures::GAUSSIAN)) {
    cv::Mat gauss;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    featureMaps.push_back(gauss);
  }

  // 3. Laplacian of Gaussian (edge + blob detection)
  if(features.contains(joda::settings::PixelClassifierFeatures::LAPLACIAN)) {
    cv::Mat gauss, lap;
    cv::GaussianBlur(gray, gauss, cv::Size(5, 5), 1.0);
    cv::Laplacian(gauss, lap, CV_32F, 3);
    featureMaps.push_back(lap);
  }

  // 4. Gradient magnitude
  if(features.contains(joda::settings::PixelClassifierFeatures::GRADIENT)) {
    cv::Mat gradX, gradY, gradMag;
    cv::Sobel(gray, gradX, CV_32F, 1, 0, 3);
    cv::Sobel(gray, gradY, CV_32F, 0, 1, 3);
    cv::magnitude(gradX, gradY, gradMag);
    featureMaps.push_back(gradMag);
  }

  // 5. Local variance (texture)
  if(features.contains(joda::settings::PixelClassifierFeatures::VARIANCE)) {
    cv::Mat mean, meanSq, var;
    cv::blur(gray, mean, cv::Size(5, 5));
    cv::blur(gray.mul(gray), meanSq, cv::Size(5, 5));
    cv::sqrt(meanSq - mean.mul(mean), var);
    featureMaps.push_back(var);
  }

  // 6. Hessian determinant (blob/ridge detection)
  if(features.contains(joda::settings::PixelClassifierFeatures::HESSIAN)) {
    cv::Mat dxx, dyy, dxy;
    cv::Sobel(gray, dxx, CV_32F, 2, 0, 3);
    cv::Sobel(gray, dyy, CV_32F, 0, 2, 3);
    cv::Sobel(gray, dxy, CV_32F, 1, 1, 3);
    cv::Mat detHessian = dxx.mul(dyy) - dxy.mul(dxy);
    featureMaps.push_back(detHessian);
  }

  // ---- Convert feature maps to feature matrix (pixels × features) ----
  int rows = img.rows * img.cols;
  int dims = static_cast<int>(featureMaps.size());
  cv::Mat featureMatrix(rows, dims, CV_32F);

  for(int f = 0; f < dims; ++f) {
    cv::Mat flat = featureMaps[f].reshape(1, rows);    // flatten
    flat.convertTo(featureMatrix.col(f), CV_32F);
  }

  return featureMatrix;    // each row = pixel, each col = feature
}

}    // namespace joda::ml
