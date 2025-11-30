///
/// \file      cuda_flow_field.cu
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#ifdef WITH_CUDA

#include <cuda_runtime.h>                // Core CUDA runtime API (for cudaMalloc, cudaMemcpy, etc.)
#include <device_launch_parameters.h>    // For thread/block indexing macros
#include <math_constants.h>              // Optional, for constants like CUDART_INF_F, if needed
#include <cmath>                         // For std::fabs, std::floor if you use host code (but inside __device__ use CUDA math functions)
#include <cstdio>

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
__device__ float bilinearInterpolate(const float *data, int width, int height, float x, float y)
{
  int x0 = floorf(x);
  int x1 = min(x0 + 1, width - 1);
  int y0 = floorf(y);
  int y1 = min(y0 + 1, height - 1);

  float dx = x - x0;
  float dy = y - y0;

  float val00 = data[y0 * width + x0];
  float val10 = data[y0 * width + x1];
  float val01 = data[y1 * width + x0];
  float val11 = data[y1 * width + x1];

  float val0 = val00 * (1 - dx) + val10 * dx;
  float val1 = val01 * (1 - dx) + val11 * dx;

  return val0 * (1 - dy) + val1 * dy;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
__global__ void followFlow(const float *flowX, const float *flowY, const float *mask, int width, int height, float stepSize, int numSteps,
                           float epsilon, float *outputX, float *outputY, float maskThreshold)
{
  int px = blockIdx.x * blockDim.x + threadIdx.x;
  int py = blockIdx.y * blockDim.y + threadIdx.y;

  if(mask[py * width + px] < maskThreshold) {
    outputX[py * width + px] = -1;
    outputY[py * width + px] = -1;
    return;
  }

  if(px >= width || py >= height) {
    return;
  }

  float x = static_cast<float>(px);
  float y = static_cast<float>(py);

  for(int i = 0; i < numSteps; ++i) {
    float fx = bilinearInterpolate(flowX, width, height, x, y);
    float fy = bilinearInterpolate(flowY, width, height, x, y);

    if(fabsf(fx) < epsilon && fabsf(fy) < epsilon) {
      break;
    }

    x += stepSize * fx;
    y += stepSize * fy;

    x = fminf(fmaxf(x, 0.0f), static_cast<float>(width - 1));
    y = fminf(fmaxf(y, 0.0f), static_cast<float>(height - 1));
  }

  outputX[py * width + px] = x;
  outputY[py * width + px] = y;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
extern "C" void cudaFlowIterationKernel(const float *flowX, const float *flowY, const float *mask, int width, int height, float stepSize,
                                        int numSteps, float epsilon, float *outputX, float *outputY, float maskThreshold)
{
  dim3 block(2, 2);
  dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

  followFlow<<<grid, block>>>(flowX, flowY, mask, width, height, stepSize, numSteps, epsilon, outputX, outputY, maskThreshold);

  cudaDeviceSynchronize();    // optional but good for debugging
}
#endif
