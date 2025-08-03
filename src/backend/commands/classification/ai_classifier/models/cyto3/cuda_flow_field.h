///
/// \file      cuda_flow_field.h
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
#ifdef WITH_CUDA
#pragma once

extern "C" void cudaFlowIterationKernel(const float *flowX, const float *flowY, const float *mask, int width, int height, float stepSize,
                                        int numSteps, float epsilon, float *outputX, float *outputY, float maskThreshold);
#endif
