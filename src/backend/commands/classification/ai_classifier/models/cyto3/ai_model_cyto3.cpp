///
/// \file      ai_model_unet.cpp
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

#include <random>
#include <string>
#include "backend/helper/duration_count/duration_count.h"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/types.hpp>

#undef slots
#include <ATen/core/TensorBody.h>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "ai_model_cyto3.hpp"
#define slots Q_SLOTS

// Cuda
#ifdef WITH_CUDA
#include <cuda_runtime.h>
#include "cuda_flow_field.h"
#endif

// Flow field
#include "cpu_flow_field.h"

namespace joda::ai {

std::pair<cv::Mat, std::set<int>> followFlowFieldCpu(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask, float maskThreshold);

#ifdef WITH_CUDA
std::pair<cv::Mat, std::set<int>> followFlowFieldCuda(const at::Device &device, const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask,
                                                      float maskThreshold);
#endif
std::vector<AiModel::Result> extractObjectMasksAndBoundingBoxes(const cv::Mat &labelImage, const std::set<int> &labels);

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiModelCyto3::AiModelCyto3(const ProbabilitySettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelCyto3::processPrediction(const at::Device &device, const cv::Mat &inputImage, const at::IValue &tensorIn) -> std::vector<Result>
{
  static const int CHANNEL_GRADIENTS_X = 0;
  static const int CHANNEL_GRADIENTS_Y = 1;
  static const int CHANNEL_MASKS       = 2;

  const int32_t originalHeight = inputImage.rows;
  const int32_t originalWith   = inputImage.cols;

  if(!tensorIn.isTuple()) {
    joda::log::logError("Expected model output to be a tuple (detections, seg_masks).");
    return {};
  }

  auto outputTuple = tensorIn.toTuple();
  auto maskTensor  = outputTuple->elements()[0].toTensor();    // Shape: [N, 6]

  // Move tensor to CPU and convert to float32
  maskTensor = maskTensor.detach().to(torch::kCPU).to(torch::kFloat32);

  // ===============================
  // 0. Rescale to original image
  // ===============================
  at::Tensor tensorSmall     = maskTensor.to(at::kCPU).to(at::kFloat);
  torch::Tensor tensorCpu    = torch::upsample_bilinear2d(tensorSmall, {originalHeight, originalWith}, false);
  at::Tensor maskTensorImage = tensorCpu[0][CHANNEL_MASKS].clone().contiguous();          // [height, width]
  at::Tensor flowX           = tensorCpu[0][CHANNEL_GRADIENTS_X].clone().contiguous();    // [height, width]
  at::Tensor flowY           = tensorCpu[0][CHANNEL_GRADIENTS_Y].clone().contiguous();    // [height, width]

  // ===============================
  // 1. Convert mask to cv::mat
  // ===============================
  cv::Mat maskImage(originalHeight, originalWith, CV_32F, maskTensorImage.data_ptr<float>());
  cv::Mat flowXImage(originalHeight, originalWith, CV_32F, flowX.data_ptr<float>());
  cv::Mat flowYImage(originalHeight, originalWith, CV_32F, flowY.data_ptr<float>());

  // ===============================
  // 2. Follow the flow field, returns a object segmented mask
  // ===============================
  std::pair<cv::Mat, std::set<int>> result;

  if(device.is_cpu()) {
    auto idx = DurationCount::start("Follow field CPU");
    result   = followFlowFieldCpu(flowXImage, flowYImage, maskImage, mSettings.maskThreshold);
    DurationCount::stop(idx);
  }
#ifdef WITH_CUDA
  else if(device.is_cuda()) {
    auto idx = DurationCount::start("Follow field CUDA");
    result   = followFlowFieldCuda(device, flowXImage, flowYImage, maskImage, mSettings.maskThreshold);
    DurationCount::stop(idx);
  }
#endif
  else {
    throw std::runtime_error("unsupported device");
  }

  return extractObjectMasksAndBoundingBoxes(result.first, result.second);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
std::vector<AiModel::Result> extractObjectMasksAndBoundingBoxes(const cv::Mat &labelImage, const std::set<int> &labels)
{
  CV_Assert(labelImage.type() == CV_32S || labelImage.type() == CV_8U);
  auto idx = DurationCount::start("Extract objects");

  const int rows = labelImage.rows;
  const int cols = labelImage.cols;

  std::vector<AiModel::Result> result;
  for(int label : labels) {
    cv::Mat fullMask = cv::Mat::zeros(rows, cols, CV_8U);
    // Generate full-size binary mask
    for(int y = 0; y < rows; ++y) {
      for(int x = 0; x < cols; ++x) {
        int val = (labelImage.type() == CV_32S) ? labelImage.at<int>(y, x) : labelImage.at<uchar>(y, x);
        if(val == label) {
          fullMask.at<uchar>(y, x) = 255;
        }
      }
    }

    // Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(fullMask.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if(contours.empty()) {
      continue;
    }

    // Use contour to get bounding box
    cv::Rect bbox = cv::boundingRect(contours[0]);
    for(size_t i = 1; i < contours.size(); ++i) {
      bbox |= cv::boundingRect(contours[i]);
    }

    // Crop mask and adjust contours
    cv::Mat croppedMask = fullMask(bbox).clone();

    // Shift contours relative to top-left of bbox
    std::vector<std::vector<cv::Point>> adjustedContours;
    size_t maxContourIdx = 0;
    size_t conoutrSize   = 0;
    size_t idx           = 0;
    for(const auto &contour : contours) {
      std::vector<cv::Point> shifted;
      if(contour.size() > conoutrSize) {
        maxContourIdx = idx;
        conoutrSize   = contour.size();
      }
      for(const auto &pt : contour) {
        shifted.emplace_back(pt.x - bbox.x, pt.y - bbox.y);
      }
      adjustedContours.push_back(shifted);
      idx++;
    }

    result.push_back(AiModel::Result{.boundingBox = bbox, .mask = croppedMask, .contour = adjustedContours.at(maxContourIdx), .classId = 0});
  }
  DurationCount::stop(idx);
  return result;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
#ifdef WITH_CUDA
std::pair<cv::Mat, std::set<int>> followFlowFieldCuda(const at::Device &device, const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask,
                                                      float maskThreshold)
{
  int32_t width    = flowX.cols;
  int32_t height   = flowX.rows;
  int32_t stepSize = 2;
  int32_t numSteps = 500;

  cv::transpose(flowX, flowX);
  cv::transpose(flowY, flowY);
  cv::transpose(mask, mask);

  auto flowX_tensor = torch::from_blob(flowX.data, {flowX.rows, flowX.cols}, torch::kFloat32).to(device);
  auto flowY_tensor = torch::from_blob(flowY.data, {flowY.rows, flowY.cols}, torch::kFloat32).to(device);
  auto mask_tensor  = torch::from_blob(mask.data, {mask.rows, mask.cols}, torch::kFloat32).to(device);

  if(!flowX_tensor.is_contiguous()) {
    flowX_tensor = flowX_tensor.contiguous();
  }
  if(!flowY_tensor.is_contiguous()) {
    flowY_tensor = flowY_tensor.contiguous();
  }

  auto *d_flowX = flowX_tensor.data_ptr<float>();
  auto *d_flowY = flowY_tensor.data_ptr<float>();
  auto *d_mask  = mask_tensor.data_ptr<float>();
  float *d_outX;
  float *d_outY;
  cudaMalloc(&d_outX, sizeof(float) * width * height);
  cudaMalloc(&d_outY, sizeof(float) * width * height);
  cudaFlowIterationKernel(d_flowX, d_flowY, d_mask, width, height, stepSize, numSteps, 0.0001f, d_outX, d_outY, maskThreshold);

  // Copy result back to CPU if needed
  std::vector<float> h_outX(width * height);
  std::vector<float> h_outY(width * height);
  cudaMemcpy(h_outX.data(), d_outX, sizeof(float) * width * height, cudaMemcpyDeviceToHost);
  cudaMemcpy(h_outY.data(), d_outY, sizeof(float) * width * height, cudaMemcpyDeviceToHost);

  // Cleanup GPU memory
  cudaFree(d_outX);
  cudaFree(d_outY);

  // Preprocessing
  cv::Mat labels = cv::Mat::zeros(flowX.size(), CV_32S);    // int labels per pixel
  std::map<std::pair<int, int>, int> landingLabelMap;
  std::set<int> labelsSet;
  int currentLabel = 1;
  for(int y = 0; y < flowY.rows; ++y) {
    for(int x = 0; x < flowX.cols; ++x) {
      // Get the landing pos
      int lx = cvRound(h_outX.at(y * flowX.cols + x));
      int ly = cvRound(h_outY.at(y * flowX.cols + x));
      // Key for map
      auto key = std::make_pair(lx, ly);
      int label;
      auto it = landingLabelMap.find(key);
      if(it == landingLabelMap.end()) {
        label                = currentLabel++;
        landingLabelMap[key] = label;
      } else {
        label = it->second;
      }
      labelsSet.emplace(label);
      labels.at<int>(y, x) = label;
    }
  }
  cv::transpose(labels, labels);

  return {labels, labelsSet};
}
#endif

///
/// \brief      Follow the flow field and returns a cv::Mat with each segmented object having a pixel value
/// \author     Joachim Danmayr
/// \return     Segmented mask, set of labels
///
std::pair<cv::Mat, std::set<int>> followFlowFieldCpu(const cv::Mat &flowX, const cv::Mat &flowY, const cv::Mat &mask, float maskThreshold)
{
  // We have to transform to get a correct flow field
  cv::transpose(flowX, flowX);
  cv::transpose(flowY, flowY);
  cv::transpose(mask, mask);

  // Follow the flow starting at pixel (x0, y0)
  cv::Mat labels = cv::Mat::zeros(flowX.size(), CV_32S);    // int labels per pixel
  std::map<std::pair<int, int>, int> landingLabelMap;
  std::set<int> labelsSet;
  int currentLabel = 1;
  for(int y = 0; y < flowX.rows; ++y) {
    for(int x = 0; x < flowX.cols; ++x) {
      float pixelMask = mask.at<float>(y, x);
      if(pixelMask > maskThreshold) {
        float px = static_cast<float>(x);
        float py = static_cast<float>(y);

        cv::Point2f landing_pos = cpuFlowIterationKernel(flowX, flowY, px, py, 500, 2);    // your method modifies px, py to landing pos

        int lx = cvRound(landing_pos.x);
        int ly = cvRound(landing_pos.y);

        // Key for map
        auto key = std::make_pair(lx, ly);

        int label;
        auto it = landingLabelMap.find(key);
        if(it == landingLabelMap.end()) {
          label                = currentLabel++;
          landingLabelMap[key] = label;
        } else {
          label = it->second;
        }
        labelsSet.emplace(label);
        labels.at<int>(y, x) = label;
      }
    }
  }

  // transpose back
  cv::transpose(labels, labels);

  //
  // Cell pose flow field
  //
  {
      //   cv::Mat colorImage(mask.size(), CV_8UC3, cv::Scalar(0, 0, 0));    // Black image
      //   for(int x = 0; x < mask.cols; x++) {
      //     for(int y = 0; y < mask.rows; y++) {
      //       float pixelMask = mask.at<float>(y, x);
      //       if(pixelMask > 0.5) {
      //         float pixelValFlowX = flowX.at<float>(y, x);
      //         float pixelValFlowY = flowY.at<float>(y, x);
      //
      //         colorImage.at<cv::Vec3b>(y, x) = flowToColor(pixelValFlowX, pixelValFlowY);
      //       } else {
      //         colorImage.at<cv::Vec3b>(y, x) = {0, 0, 0};
      //       }
      //     }
      //   }
      //   cv::imwrite("tmp/flowField.jpg", colorImage);
  }

  //
  // Debugging
  //
  {
    // cv::Mat labelDebugImage;
    // cv::Mat binaryDebugImage;
    // clusterLandingPoints(landingLabelMap, labelDebugImage, binaryDebugImage);
    // cv::imwrite("tmp/labelDebugImage.jpg", labelDebugImage);
    // cv::imwrite("tmp/binaryDebugImage.jpg", binaryDebugImage);
    // cv::transpose(labels, labels);
    // cv::imwrite("tmp/labels.jpg", paintLabels(labels));

    //
    // FLow field arrows
    //
    // cv::Mat arrowImage;
    // drawFlowArrows(flowX, flowY, arrowImage, 10, 5.0);    // stride=10, scale flow x5 for visibility
    // for(const auto &[coor, a] : landingLabelMap) {
    //  cv::Point center(cvRound(coor.first), cvRound(coor.second));     // Convert to integer pixel position
    //  cv::circle(arrowImage, center, 2, cv::Scalar(0, 0, 255), -1);    // BGR = Red
    //}
    // cv::imwrite("tmp/arrows.jpg", arrowImage);
  }

  return {labels, labelsSet};
}

}    // namespace joda::ai
