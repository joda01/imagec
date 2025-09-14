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

struct AnnMlpTrainingSettings
{
  int32_t nrNeuronsLayer01  = 64;
  int32_t nrNeuronsLayer02  = 64;
  int32_t nrNeuronsLayer03  = 64;
  int32_t nrNeuronsLayer04  = 64;
  int32_t nrNeuronsLayer05  = 64;
  double terminationEpsilon = 0;
  int32_t maxIterations     = 1000;
};

struct PixelClassifierTrainingSettings
{
  //
  // ID of the class to train
  //
  std::set<int32_t> trainingClasses;

  //
  // Machine learning method to use
  //
  PixelClassifierMethod method = PixelClassifierMethod::Unknown;

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
  std::optional<RandomForestTrainingSettings> randomForest;
  std::optional<AnnMlpTrainingSettings> annMlp;
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
