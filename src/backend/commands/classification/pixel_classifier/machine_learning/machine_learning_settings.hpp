///
/// \file      machine_learning_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include <map>
#include <set>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enums_classes.hpp"

namespace joda::ml {

enum class TrainingFeatures
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

enum class ModelType
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

struct MachineLearningSettings
{
  //
  //
  //
  ModelType modelType = ModelType::RTrees;

  //
  // Mapping from a object class id to a trainings class id.
  // The training class id is continuous starting with 0 for the background.
  //
  std::map<enums::ClassId, int32_t> trainingClasses;

  //
  // Which category of classes to train
  //
  joda::atom::ROI::Category categoryToTrain = joda::atom::ROI::Category::MANUAL_SEGMENTATION;

  //
  // Features to use for training and prediction
  //
  std::set<TrainingFeatures> features;

  //
  // Output path where the trained model should be stored
  //
  std::filesystem::path outPath;
};

NLOHMANN_JSON_SERIALIZE_ENUM(ModelType, {
                                            {ModelType::Unknown, "Unknown"},
                                            {ModelType::RTrees, "RTrees"},
                                            {ModelType::DTrees, "DTrees"},
                                            {ModelType::Boost, "Boost"},
                                            {ModelType::SVM, "SVM"},
                                            {ModelType::SVMSGD, "SVMSGD"},
                                            {ModelType::ANN_MLP, "ANN_MLP"},
                                            {ModelType::KNearest, "KNearest"},
                                            {ModelType::NormalBayes, "NormalBayes"},
                                            {ModelType::EM, "EM"},
                                            {ModelType::LogisticRegression, "LogisticRegression"},
                                        });

NLOHMANN_JSON_SERIALIZE_ENUM(TrainingFeatures, {{TrainingFeatures::Intensity, "Intensity"},
                                                {TrainingFeatures::Gaussian, "Gaussian"},
                                                {TrainingFeatures::LaplacianOfGaussian, "LaplacianOfGaussian"},
                                                {TrainingFeatures::WeightedDeviation, "WeightedDeviation"},
                                                {TrainingFeatures::GradientMagnitude, "GradientMagnitude"},
                                                {TrainingFeatures::StructureTensorEigenvalues, "StructureTensorEigenvalues"},
                                                {TrainingFeatures::StructureTensorCoherence, "StructureTensorCoherence"},
                                                {TrainingFeatures::HessianDeterminant, "HessianDeterminant"},
                                                {TrainingFeatures::HessianEigenvalues, "HessianEigenvalues"}});

}    // namespace joda::ml
