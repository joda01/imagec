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
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/settings_meta.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ml {

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

enum class Framework
{
  Unknown,
  OpenCv,
  MlPack,
  PyTorch
};

struct ImageCommandPipeline
{
  //
  // The snap shotted pipeline steps
  //
  std::list<settings::PipelineStep> pipelineSteps = {};

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImageCommandPipeline, pipelineSteps);
};

struct MachineLearningSettings
{
  struct ClassLabels
  {
    enums::ClassId classId = enums::ClassId::NONE;
    int32_t pixelClassId   = 0;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ClassLabels, pixelClassId);
  };

  //
  // Mapping from a object class id to a trainings class id.
  // The training class id is continuous starting with 0 for the background.
  //
  std::list<ClassLabels> classLabels;

  //
  //
  //
  joda::settings::SettingsMeta meta;

  //
  //
  //
  Framework framework = Framework::OpenCv;

  //
  //
  //
  ModelType modelTyp;

  //
  //
  //
  std::list<ImageCommandPipeline> featureExtractionPipelines;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MachineLearningSettings, meta, classLabels, framework, modelTyp, featureExtractionPipelines);

  /// NOT SAVED /////////////////////////////////////

  //
  // Which category of classes to train
  //
  joda::atom::ROI::Category categoryToTrain = joda::atom::ROI::Category::MANUAL_SEGMENTATION;

  //
  // Output path where the trained model should be stored
  //
  std::filesystem::path outPath;

  //
  // Just a placeholder
  //
  nlohmann::json model;

  auto toTrainingsClassesMap() const -> std::map<enums::ClassId, int32_t>
  {
    std::map<enums::ClassId, int32_t> ret;
    for(const auto &entry : classLabels) {
      ret.emplace(entry.classId, entry.pixelClassId);
    }
    return ret;
  }

  void toClassesLabels(const std::map<enums::ClassId, int32_t> &in)
  {
    classLabels.clear();
    for(const auto &[classId, pixelId] : in) {
      classLabels.emplace_back(ClassLabels{classId, pixelId});
    }
  }
};

NLOHMANN_JSON_SERIALIZE_ENUM(Framework, {
                                            {Framework::Unknown, "Unknown"},
                                            {Framework::OpenCv, "OpenCv"},
                                            {Framework::MlPack, "MlPack"},
                                            {Framework::PyTorch, "PyTorch"},
                                        });

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

}    // namespace joda::ml
