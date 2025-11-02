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

#include "random_forest_mlpack.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include <nlohmann/json_fwd.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#include <mlpack/core.hpp>
#include <mlpack/methods/random_forest/random_forest.hpp>

namespace joda::ml {

struct RandomForestMetrics
{
  size_t numTrees;
  size_t numFeatures;
  size_t estimatedNumClasses;
  arma::Row<size_t> treeDepths;
  arma::Row<size_t> treeNodeCounts;
  arma::Row<size_t> treeLeafCounts;
};

template <typename TreeType>
void ComputeTreeStats(const TreeType &node, size_t depth, size_t &maxDepth, size_t &nodeCount, size_t &leafCount,
                      std::unordered_set<size_t> &usedFeatures, std::unordered_set<size_t> &observedClasses)
{
  nodeCount++;

  // If no children => leaf
  if(node.NumChildren() == 0) {
    leafCount++;
    // Leaves store class probabilities; pick the max label index.
    if constexpr(requires(const TreeType &t) { t.ClassProbabilities(); }) {
      arma::vec probs = node.ClassProbabilities();
      if(!probs.is_empty())
        observedClasses.insert(probs.index_max());
    }

    if(depth > maxDepth)
      maxDepth = depth;
    return;
  }

  if constexpr(requires(const TreeType &t) { t.SplitDimension(); })
    usedFeatures.insert(node.SplitDimension());

  for(size_t i = 0; i < node.NumChildren(); ++i)
    ComputeTreeStats(node.Child(i), depth + 1, maxDepth, nodeCount, leafCount, usedFeatures, observedClasses);
}

template <typename RFType>
RandomForestMetrics GetRandomForestMetrics(const RFType &rf)
{
  RandomForestMetrics m;
  m.numTrees = rf.NumTrees();

  m.treeDepths.set_size(m.numTrees);
  m.treeNodeCounts.set_size(m.numTrees);
  m.treeLeafCounts.set_size(m.numTrees);

  std::unordered_set<size_t> allUsedFeatures;
  std::unordered_set<size_t> allObservedClasses;

  for(size_t i = 0; i < m.numTrees; ++i) {
    const auto &tree = rf.Tree(i);
    size_t depth = 0, nodeCount = 0, leafCount = 0;
    std::unordered_set<size_t> usedFeatures;
    std::unordered_set<size_t> observedClasses;

    ComputeTreeStats(tree, 1, depth, nodeCount, leafCount, usedFeatures, observedClasses);

    m.treeDepths[i]     = depth;
    m.treeNodeCounts[i] = nodeCount;
    m.treeLeafCounts[i] = leafCount;

    allUsedFeatures.insert(usedFeatures.begin(), usedFeatures.end());
    allObservedClasses.insert(observedClasses.begin(), observedClasses.end());
  }

  m.numFeatures         = allUsedFeatures.size();
  m.estimatedNumClasses = allObservedClasses.size();
  return m;
}
///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::predict(const std::filesystem::path &path, const cv::Mat &image, cv::Mat &prediction)
{
  // ============================================
  // Load model together with model settings
  // ============================================
  std::ifstream file(path.string());
  if(!file.is_open()) {
    return;
  }
  nlohmann::json json;
  file >> json;
  file.close();
  MachineLearningSettings modelSettings = json;
  mlpack::RandomForest<mlpack::GiniGain, mlpack::RandomDimensionSelect, mlpack::BestBinaryNumericSplit, mlpack::AllCategoricalSplit, true,
                       mlpack::DefaultBootstrap>
      mModel;
  mlpack::data::Load(path.string(), "model", mModel, true, mlpack::data::format::json);

  // ============================================
  // Extract features based on model settings
  // ============================================
  const cv::Mat features = extractFeatures(image, modelSettings.featureExtractionPipelines, false);

  // ============================================
  // Convert OpenCV Mat -> Armadillo matrix
  // mlpack expects columns = samples, rows = features
  // ============================================
  arma::mat armaFeatures(static_cast<arma::uword>(features.cols), static_cast<arma::uword>(features.rows));
  for(int i = 0; i < features.rows; ++i) {
    for(int j = 0; j < features.cols; ++j) {
      armaFeatures(static_cast<arma::uword>(j), static_cast<arma::uword>(i)) = static_cast<double>(features.at<float>(i, j));
    }
  }

  // 5. Predict
  arma::Row<size_t> predictions;
  mModel.Classify(armaFeatures, predictions);

  // ============================================
  // Round predictions to nearest class
  // ============================================
  prediction = cv::Mat(static_cast<int>(predictions.n_elem), 1, CV_32S);
  for(size_t i = 0; i < predictions.n_elem; ++i) {
    prediction.at<int>(static_cast<int>(i), 0) = static_cast<int>(predictions[i]);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void RandomForestMlPack::train(const cv::Mat &trainSamples, const cv::Mat &trainLabels, int32_t nrOfClasses,
                               const std::filesystem::path &modelStoragePath, const MachineLearningSettings &settings)
{
  // ============================================
  // Convert OpenCV Mat to Armadillo
  // ============================================
  arma::mat data(static_cast<arma::uword>(trainSamples.cols), static_cast<arma::uword>(trainSamples.rows));

  for(int r = 0; r < trainSamples.rows; ++r) {
    for(int c = 0; c < trainSamples.cols; ++c) {    // Features
      data(c, r) = static_cast<double>(trainSamples.at<float>(r, c));
    }
  }

  // ============================================
  // Convert labels to arma::Row<size_t>
  // ============================================
  arma::Row<size_t> labels(static_cast<arma::uword>(trainLabels.rows));
  for(int r = 0; r < trainLabels.rows; ++r) {
    labels(static_cast<arma::uword>(r)) = static_cast<size_t>(trainLabels.at<int>(r, 0));
  }

  // ============================================
  // Train RandomForest
  // ============================================
  fireTrainingProgress("Start training with CPU ...");

  mlpack::RandomForest<mlpack::GiniGain, mlpack::RandomDimensionSelect, mlpack::BestBinaryNumericSplit, mlpack::AllCategoricalSplit, true,
                       mlpack::DefaultBootstrap>
      model;
  model.Train(data, labels, static_cast<size_t>(nrOfClasses), static_cast<size_t>(mSettings.maxNumberOfTrees),
              static_cast<size_t>(mSettings.minSampleCount), 0.0, static_cast<size_t>(mSettings.maxTreeDepth));

  auto m = GetRandomForestMetrics(model);
  {
    std::stringstream log;
    log << "Finished with following metrics:\n";
    log << "Number of trees     : " << m.numTrees << "\n";
    log << "Estimated features  : " << m.numFeatures << "\n";
    log << "Estimated classes   : " << m.estimatedNumClasses << "\n";
    log << "Average tree depth  : " << arma::mean(arma::conv_to<arma::vec>::from(m.treeDepths)) << "\n";
    log << "Average #nodes/tree : " << arma::mean(arma::conv_to<arma::vec>::from(m.treeNodeCounts)) << "\n";
    log << "Average #leaves/tree: " << arma::mean(arma::conv_to<arma::vec>::from(m.treeLeafCounts));
    log << std::endl;
    fireTrainingProgress(log.str());
  }
  // ============================================
  // Save model and meta
  // ============================================
  mlpack::data::Save(modelStoragePath.string(), "model", model, true, mlpack::data::format::json);

  MachineLearningSettings parsed;
  parsed.classLabels                = settings.classLabels;
  parsed.featureExtractionPipelines = settings.featureExtractionPipelines;
  parsed.meta                       = settings.meta;
  parsed.modelTyp                   = ModelType::RTrees;
  parsed.framework                  = Framework::MlPack;
  nlohmann::json json               = parsed;
  removeNullValues(json);
  std::string metaData = json.dump(2);
  if(!metaData.empty()) {
    metaData.erase(0, 1);    // remove 1 character at position 0 this is a '{'
  }

  // Append meta information

  // Open file in read/write mode
  std::fstream file(modelStoragePath.string(), std::ios::in | std::ios::out);
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
    file.seekp(pos);    // Move output pointer to that position
    file.put(',');      // Overwrite the '}' with ','
  }

  file << metaData;
  file.close();
}

void RandomForestMlPack::stopTraining()
{
}

}    // namespace joda::ml
