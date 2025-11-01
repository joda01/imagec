#include <torch/torch.h>
#include <fstream>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>    // you can use this popular header-only library

using json = nlohmann::json;

struct MLPModelImpl : torch::nn::Module
{
  torch::nn::Sequential layers{nullptr};
  std::vector<int> hiddenLayerSizes;
  int inputSize{0};
  int outputSize{0};

  MLPModelImpl() = default;    // required for loading
  MLPModelImpl(int inputSize_, const std::vector<int> &hidden, int outputSize_) :
      hiddenLayerSizes(hidden), inputSize(inputSize_), outputSize(outputSize_)
  {
    build_layers();
  }

  void build_layers()
  {
    layers   = torch::nn::Sequential();
    int prev = inputSize;
    for(int n : hiddenLayerSizes) {
      if(n > 0) {
        layers->push_back(torch::nn::Linear(prev, n));
        layers->push_back(torch::nn::ReLU());
        prev = n;
      }
    }
    layers->push_back(torch::nn::Linear(prev, outputSize));
    register_module("layers", layers);
  }

  torch::Tensor forward(torch::Tensor x)
  {
    return layers->forward(x);
  }

  // --- custom serialization hooks ---
  void save(torch::serialize::OutputArchive &archive) const override
  {
    std::cout << "SAVE" << std::endl;
    // 1. Save the layers’ parameters
    torch::serialize::OutputArchive layers_archive;
    layers->save(layers_archive);
    archive.write("layers", layers_archive);

    // 2. Save metadata as tensors (so it goes inside the same .pt file)
    torch::Tensor input_t  = torch::tensor({inputSize}, torch::kInt32);
    torch::Tensor output_t = torch::tensor({outputSize}, torch::kInt32);
    torch::Tensor hidden_t = torch::tensor(hiddenLayerSizes, torch::kInt32);

    archive.write("input_size", input_t);
    archive.write("output_size", output_t);
    archive.write("hidden_layers", hidden_t);
  }

  void load(torch::serialize::InputArchive &archive) override
  {
    std::cout << "LOAD" << std::endl;

    // 1. Load metadata first
    torch::Tensor input_t;
    torch::Tensor output_t;
    torch::Tensor hidden_t;
    archive.read("input_size", input_t);
    archive.read("output_size", output_t);
    archive.read("hidden_layers", hidden_t);

    inputSize  = input_t.item<int>();
    outputSize = output_t.item<int>();
    hiddenLayerSizes.clear();
    hiddenLayerSizes.reserve(hidden_t.size(0));
    for(int i = 0; i < hidden_t.size(0); ++i) {
      hiddenLayerSizes.push_back(hidden_t[i].item<int>());
    }

    std::cout << "In" << std::to_string(inputSize) << std::endl;
    std::cout << "In" << std::to_string(outputSize) << std::endl;

    // 2. Rebuild layers with correct structure
    build_layers();

    // 3. Load layer parameters
    torch::serialize::InputArchive layers_archive;
    archive.read("layers", layers_archive);
    layers->load(layers_archive);
  }
};

// This macro defines the smart-pointer alias: "MLPModel"
TORCH_MODULE(MLPModel);
