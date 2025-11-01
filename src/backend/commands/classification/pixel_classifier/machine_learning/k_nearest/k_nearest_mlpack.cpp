///
/// \file      random_forest.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "k_nearest_mlpack.hpp"

namespace joda::ml {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void KNearestMlPack::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses,
                           const std::filesystem::path &modelStoragePath)
{
}

void KNearestMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, const cv::Mat &features, cv::Mat &prediction,
                             const std::filesystem::path &modelStoragePath)
{
}

}    // namespace joda::ml
