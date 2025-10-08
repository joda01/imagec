///
/// \file      random_forest_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/setting_base.hpp"

namespace joda::settings {

enum class PixelClassifierMethod
{
  Unknown,
  RTrees,
  DTrees,
  Boost,
  SVM,
  SVMSGD,
  ANN_MLP,
  KNearest,
  NormalBayes,
  LogisticRegression,
  EM
};

enum class PixelClassifierFeatures
{
  Intensity,
  Gaussian,
  LaplacianOfGaussian,
  WeightedDeviation,
  GradientMagnitude,
  StructureTensorEigenvalues,
  StructureTensorCoherence,
  HessianDeterminant,
  HessianEigenvalues
};

struct RandomForestTrainingSettings
{
  int32_t maxTreeDepth      = 50;    // 100
  int32_t minSampleCount    = 10;
  float regressionAccuracy  = 0;
  double terminationEpsilon = 0;
  int32_t maxNumberOfTrees  = 50;
};

struct KNearestTrainingSettings
{
  int32_t defaultK = 10;
};

struct AnnMlpTrainingSettings
{
  std::vector<int32_t> neuronsLayer = {64};    // Vector size is nr. of neuron layers. Entry is the number of neurons per layer.
  double terminationEpsilon         = 0.01;
  int32_t maxIterations             = 1000;
};

struct PixelClassifierTrainingSettings
{
  //
  // Mapping from a object class id to a trainings class id.
  // The training class id is continuous starting with 0 for the background.
  //
  std::map<enums::ClassId, int32_t> trainingClasses;

  //
  // Machine learning method to use
  //
  PixelClassifierMethod method = PixelClassifierMethod::Unknown;

  //
  // Which category of classes to train
  //
  joda::atom::ROI::Category categoryToTrain = joda::atom::ROI::Category::MANUAL_SEGMENTATION;

  //
  // Features to use for training and prediction
  //
  std::set<PixelClassifierFeatures> features;

  //
  // Output path where the trained model should be stored
  //
  std::filesystem::path outPath;

  //
  // Settings for different algorithms
  //
  RandomForestTrainingSettings randomForest;
  AnnMlpTrainingSettings annMlp;
  KNearestTrainingSettings kNearest;
};

NLOHMANN_JSON_SERIALIZE_ENUM(PixelClassifierMethod, {
                                                        {PixelClassifierMethod::Unknown, "Unknown"},
                                                        {PixelClassifierMethod::RTrees, "RTrees"},
                                                        {PixelClassifierMethod::DTrees, "DTrees"},
                                                        {PixelClassifierMethod::Boost, "Boost"},
                                                        {PixelClassifierMethod::SVM, "SVM"},
                                                        {PixelClassifierMethod::SVMSGD, "SVMSGD"},
                                                        {PixelClassifierMethod::ANN_MLP, "ANN_MLP"},
                                                        {PixelClassifierMethod::KNearest, "KNearest"},
                                                        {PixelClassifierMethod::NormalBayes, "NormalBayes"},
                                                        {PixelClassifierMethod::EM, "EM"},
                                                        {PixelClassifierMethod::LogisticRegression, "LogisticRegression"},
                                                    });

NLOHMANN_JSON_SERIALIZE_ENUM(PixelClassifierFeatures, {{PixelClassifierFeatures::Intensity, "Intensity"},
                                                       {PixelClassifierFeatures::Gaussian, "Gaussian"},
                                                       {PixelClassifierFeatures::LaplacianOfGaussian, "LaplacianOfGaussian"},
                                                       {PixelClassifierFeatures::WeightedDeviation, "WeightedDeviation"},
                                                       {PixelClassifierFeatures::GradientMagnitude, "GradientMagnitude"},
                                                       {PixelClassifierFeatures::StructureTensorEigenvalues, "StructureTensorEigenvalues"},
                                                       {PixelClassifierFeatures::StructureTensorCoherence, "StructureTensorCoherence"},
                                                       {PixelClassifierFeatures::HessianDeterminant, "HessianDeterminant"},
                                                       {PixelClassifierFeatures::HessianEigenvalues, "HessianEigenvalues"}});

}    // namespace joda::settings
