///
/// \file      ai_model.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#undef slots

#include <torch/torch.h>
#include "backend/commands/classification/ai_classifier/ai_classifier_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#define slots Q_SLOTS

namespace joda::ai {

class AiModel
{
public:
  struct Result
  {
    cv::Rect boundingBox;
    cv::Mat mask;
    std::vector<cv::Point> contour;
    int32_t classId;
    float probability;
  };

  virtual auto processPrediction(const at::Device &device, const cv::Mat &inputImage, const at::IValue &prediction) -> std::vector<Result> = 0;

  void printTensorProperties(const torch::Tensor &tensor)
  {
    // Print the number of dimensions
    std::cout << "Tensor dimensions: " << tensor.dim() << std::endl;

    // Print the tensor sizes (shape)
    std::cout << "Tensor sizes: " << tensor.sizes() << std::endl;

    // Print data type and device information
    std::cout << "Tensor type: " << tensor.dtype() << std::endl;
    std::cout << "Tensor device: " << tensor.device() << std::endl;

    // Optionally, iterate through each dimension and print its size individually
    std::cout << "Tensor shape: [";
    for(int i = 0; i < tensor.dim(); i++) {
      std::cout << tensor.size(i);
      if(i < tensor.dim() - 1)
        std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // Compute the minimum, maximum, and mean values.
    // Note: These operations return a tensor containing a single value.
    torch::Tensor min_tensor  = tensor.min();
    torch::Tensor max_tensor  = tensor.max();
    torch::Tensor mean_tensor = tensor.mean();

    // Convert the single-element tensors to scalar values.
    // Change <float> to another type if your tensor has a different data type.
    float min_val  = min_tensor.item<float>();
    float max_val  = max_tensor.item<float>();
    float mean_val = mean_tensor.item<float>();

    // Print the tensor and its statistics.
    std::cout << "Minimum value: " << min_val << "\n";
    std::cout << "Maximum value: " << max_val << "\n";
    std::cout << "Mean value: " << mean_val << "\n";
  }
};

}    // namespace joda::ai
