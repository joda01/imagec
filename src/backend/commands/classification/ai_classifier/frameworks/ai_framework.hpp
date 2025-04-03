///
/// \file      ai_framework.hpp
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
#include <opencv2/core/mat.hpp>
#define slots Q_SLOTS

namespace joda::ai {

class AiFramework
{
public:
  struct InputParameters
  {
    enum class NetInputDataType
    {
      FLOAT32,
      UINT8,
      UINT16,
      UINT32
    };
    //
    // Axes input order (b=batch, c = channel, y = height, x = width)
    //
    std::string axesOrder     = "bcyx";
    NetInputDataType dataType = NetInputDataType::FLOAT32;
    int32_t batchSize         = 1;
    int32_t nrOfChannels      = 3;
    int32_t inputWidth        = 640;
    int32_t inputHeight       = 640;

    [[nodiscard]] long getChannelIndex() const
    {
      return axesOrder.find('c');
    }
    [[nodiscard]] long getBatchIndex() const
    {
      return axesOrder.find('b');
    }
    [[nodiscard]] long getHeightIndex() const
    {
      return axesOrder.find('y');
    }
    [[nodiscard]] long getWidthIndex() const
    {
      return axesOrder.find('x');
    }
  };

  /////////////////////////////////////////////////////
  virtual at::IValue predict(const cv::Mat &inputImage) = 0;

protected:
  /////////////////////////////////////////////////////
  static auto prepareImage(const cv::Mat &inputImageOriginal, const InputParameters &settings, int colorOrder) -> cv::Mat;
};

}    // namespace joda::ai
