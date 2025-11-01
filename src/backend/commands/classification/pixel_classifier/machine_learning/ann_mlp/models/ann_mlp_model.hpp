#include <torch/torch.h>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include "backend/commands/classification/pixel_classifier/machine_learning/machine_learning_settings.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/json.hpp>    // you can use this popular header-only library
#include <nlohmann/json_fwd.hpp>

namespace joda::ml {

class MLPModelImpl : public torch::nn::Module
{
public:
  MLPModelImpl() = default;
  MLPModelImpl(int inputSize, std::vector<int> hidden, int outputSize, MachineLearningSettings modelMeta) :
      mHiddenLayerSizes(std::move(hidden)), mInputSize(inputSize), mOutputSize(outputSize), mModelMeta(std::move(modelMeta))
  {
    build_layers();
  }

  void build_layers()
  {
    mLayers  = torch::nn::Sequential();
    int prev = mInputSize;
    for(int n : mHiddenLayerSizes) {
      if(n > 0) {
        mLayers->push_back(torch::nn::Linear(prev, n));
        mLayers->push_back(torch::nn::ReLU());
        prev = n;
      }
    }
    mLayers->push_back(torch::nn::Linear(prev, mOutputSize));
    register_module("layers", mLayers);
  }

  torch::Tensor forward(torch::Tensor x)
  {
    return mLayers->forward(x);
  }

  // --- custom serialization hooks ---
  void save(torch::serialize::OutputArchive &archive) const override
  {
    // 1. Save the layers’ parameters
    torch::serialize::OutputArchive layers_archive;
    mLayers->save(layers_archive);
    archive.write("layers", layers_archive);

    // 2. Save metadata as tensors (so it goes inside the same .pt file)
    torch::Tensor input  = torch::tensor({mInputSize}, torch::kInt32);
    torch::Tensor output = torch::tensor({mOutputSize}, torch::kInt32);
    torch::Tensor hidden = torch::tensor(mHiddenLayerSizes, torch::kInt32);

    archive.write("input_size", input);
    archive.write("output_size", output);
    archive.write("hidden_layers", hidden);

    std::string metaStr             = nlohmann::json(mModelMeta).dump();
    torch::Tensor model_meta_tensor = torch::tensor(std::vector<int64_t>(metaStr.begin(), metaStr.end()), torch::kInt64);
    archive.write("model_meta", model_meta_tensor);
  }

  void load(torch::serialize::InputArchive &archive) override
  {
    // 1. Load metadata first
    torch::Tensor input;
    torch::Tensor output;
    torch::Tensor hidden;
    archive.read("input_size", input);
    archive.read("output_size", output);
    archive.read("hidden_layers", hidden);

    torch::Tensor model_meta_tensor;
    archive.read("model_meta", model_meta_tensor);
    std::string metaData(model_meta_tensor.data_ptr<int64_t>(), model_meta_tensor.data_ptr<int64_t>() + model_meta_tensor.numel());
    try {
      mModelMeta = nlohmann::json::parse(metaData);
    } catch(const std::exception &ex) {
      joda::log::logWarning("Could not parse meta. what: " + std::string(ex.what()));
    }

    mInputSize  = input.item<int>();
    mOutputSize = output.item<int>();
    mHiddenLayerSizes.clear();
    mHiddenLayerSizes.reserve(hidden.size(0));
    for(int i = 0; i < hidden.size(0); ++i) {
      mHiddenLayerSizes.push_back(hidden[i].item<int>());
    }

    // 2. Rebuild layers with correct structure
    build_layers();

    // 3. Load layer parameters
    torch::serialize::InputArchive layers_archive;
    archive.read("layers", layers_archive);
    mLayers->load(layers_archive);
  }

  const MachineLearningSettings &getMeta() const
  {
    return mModelMeta;
  }

private:
  /////////////////////////////////////////////////////
  torch::nn::Sequential mLayers{nullptr};
  std::vector<int> mHiddenLayerSizes;
  int mInputSize{0};
  int mOutputSize{0};
  MachineLearningSettings mModelMeta;
};

// This macro defines the smart-pointer alias: "MLPModel"
TORCH_MODULE(MLPModel);
}    // namespace joda::ml
