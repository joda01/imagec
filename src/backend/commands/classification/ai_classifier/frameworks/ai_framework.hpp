///
/// \file      ai_framework.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#undef slots
#include <torch/torch.h>
#include <opencv2/core/mat.hpp>
#define slots Q_SLOTS

namespace joda::ai {

class AiFramework
{
public:
  struct InputParameters
  {
    enum class NetInputType
    {
      FLOAT32,
      UINT8,
      UINT16,
      UINT32
    };
    //
    // Axes input order (b=batch, c = channel, y = height, x = width)
    //
    std::string axesOrder = "bcyx";
    NetInputType dataType = NetInputType::FLOAT32;
    int32_t batchSize     = 1;
    int32_t nrOfChannels  = 3;
    int32_t inputWidth    = 640;
    int32_t inputHeight   = 640;

    [[nodiscard]] size_t getChannelIndex() const
    {
      return axesOrder.find('c');
    }
    [[nodiscard]] size_t getBatchIndex() const
    {
      return axesOrder.find('b');
    }
    [[nodiscard]] size_t getHeightIndex() const
    {
      return axesOrder.find('y');
    }
    [[nodiscard]] size_t getWidthIndex() const
    {
      return axesOrder.find('x');
    }
  };

  /////////////////////////////////////////////////////
  virtual at::IValue predict(const cv::Mat &inputImage) = 0;

protected:
  /////////////////////////////////////////////////////
  static auto prepareImage(const cv::Mat &inputImageOriginal, const InputParameters &settings) -> cv::Mat;
};

}    // namespace joda::ai
