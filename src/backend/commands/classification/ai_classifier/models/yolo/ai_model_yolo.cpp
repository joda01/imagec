///
/// \file      ai_model_bioimage.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-07
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "ai_model_yolo.hpp"
#include <ATen/core/TensorBody.h>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::ai {

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
}

cv::Mat sigmoid(const cv::Mat &x)
{
  cv::Mat result;
  cv::exp(-x, result);
  result = 1.0 / (1.0 + result);
  return result;
}

static constexpr inline int SEG_WIDTH       = 160;
static constexpr inline int SEG_HEIGHT      = 160;
static constexpr inline int NET_WIDTH       = 640;
static constexpr inline int NET_HEIGHT      = 640;
static constexpr inline float BOX_THRESHOLD = 0.25;    // (default = 0.25)
static constexpr inline float NET_STRIDE[4] = {8, 16, 32, 64};
static constexpr inline int SEG_CHANNELS    = 32;
static constexpr inline int STRIDE_SIZE     = 3;
static constexpr inline float NMS_THRESHOLD = 0.45;    // To prevent double bounding boxes (default = 0.45)

const float mClassThreshold = 0.5;
const float mNmsScoreThreshold(BOX_THRESHOLD *mClassThreshold);
const float mMaskThreshold = 0.8;

///
/// \brief      Extracts the mask from the prediction and stores the mask
///             to the output >output[i].boxMask<
/// \author     Joachim Danmayr
/// \param[in]  maskChannel     Mask proposal
/// \param[in]  params          Image scaling parameters
/// \param[in]  inputImageShape Image shape
/// \param[out] output          Stores the mask to the output
///
auto getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) -> cv::Mat
{
  if(maskChannel.empty()) {
    return {};
  }
  static const cv::Rect roi(0, 0, static_cast<int>(SEG_WIDTH), static_cast<int>(SEG_HEIGHT));
  cv::Mat dest;
  cv::Mat mask;
  cv::exp(-maskChannel, dest);    // sigmoid
  dest = (1.0 / (1.0 + dest))(roi);
  resize(dest, mask, inputImageShape, cv::INTER_NEAREST);
  mask = mask(box).clone();
  mask = mask > mMaskThreshold;

  return mask;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto AiModelYolo::processPrediction(const cv::Mat &inputImage, const at::IValue &tensor) -> std::vector<Result>
{
  const int input_width  = 640;
  const int input_height = 640;

  // We assume the model returns a tuple: (detections, seg_masks)
  if(!tensor.isTuple()) {
    std::cerr << "Expected model output to be a tuple (detections, seg_masks)." << std::endl;
    return {};
  }
  auto output_tuple = tensor.toTuple();
  if(output_tuple->elements().size() != 2) {
    std::cerr << "Expected tuple with 2 elements." << std::endl;
    return {};
  }
  auto detections = output_tuple->elements()[0].toTensor();    // Shape: [N, 6]
  auto seg_masks  = output_tuple->elements()[1].toTensor();    // Shape: [N, mask_height, mask_width]

  float ratio[2] = {};
  {
    cv::Size shape = inputImage.size();
    ratio[0]       = static_cast<float>(NET_WIDTH) / static_cast<float>(shape.width);
    ratio[1]       = static_cast<float>(NET_HEIGHT) / static_cast<float>(shape.height);
  }

  // vector<string> outputLayerName{"output0", "output1"};
  // mNet.forward(netOutputImg, outputLayerName);

  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  std::vector<std::vector<float>> pickedProposals;    // output0[:,:, 5 + mClassNames.size():net_width]

  int32_t mNumberOfClasses = 2;

  int net_width = mNumberOfClasses + 5 + SEG_CHANNELS;
  float *pdata  = static_cast<float *>(detections.data_ptr());
  for(int stride = 0; stride < STRIDE_SIZE; stride++) {    // stride
    int grid_x = static_cast<int>(NET_WIDTH / NET_STRIDE[stride]);
    int grid_y = static_cast<int>(NET_HEIGHT / NET_STRIDE[stride]);
    for(int anchor = 0; anchor < 3; anchor++) {    // anchors
      for(int i = 0; i < grid_y; ++i) {
        for(int j = 0; j < grid_x; ++j) {
          if(pdata == nullptr || *pdata == 0) {
            continue;
          }
          float box_score = pdata[4];

          // Get the probability that an object is contained in the box of
          // each row
          if(box_score >= BOX_THRESHOLD) {
            cv::Mat scores(1, mNumberOfClasses, CV_32FC1, pdata + 5);

            // Add a handicap for the probability to rank some classes up or down
            for(int32_t modelClassIdx = 0; modelClassIdx < scores.cols; modelClassIdx++) {
              scores.at<float>(modelClassIdx) = scores.at<float>(modelClassIdx);
            }

            cv::Point classIdPointMax;
            cv::Point classIdPointMin;

            double maxClassScores;
            double minClassScores;
            minMaxLoc(scores, &minClassScores, &maxClassScores, &classIdPointMin, &classIdPointMax);
            maxClassScores = static_cast<float>(maxClassScores);
            if(maxClassScores >= mClassThreshold) {
              std::vector<float> temp_proto(pdata + 5 + mNumberOfClasses, pdata + net_width);
              pickedProposals.push_back(temp_proto);
              // rect [x,y,w,h]
              float x  = pdata[0] / ratio[0];    // x
              float y  = pdata[1] / ratio[1];    // y
              float w  = pdata[2] / ratio[0];    // w
              float h  = pdata[3] / ratio[1];    // h
              int left = MAX((x - 0.5 * w), 0);
              int top  = MAX((y - 0.5 * h), 0);

              classIds.push_back(classIdPointMax.x);
              confidences.push_back(maxClassScores * box_score);
              boxes.push_back(cv::Rect(left, top, static_cast<int>(w), static_cast<int>(h)));
            }
          }
          pdata += net_width;
        }
      }
    }
  }

  // Perform non-maximum suppression to remove redundant overlapping boxes with
  // lower confidence
  std::vector<int> nms_result;
  cv::dnn::NMSBoxes(boxes, confidences, mNmsScoreThreshold, NMS_THRESHOLD, nms_result);

  cv::Mat mask_proposals;
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx = nms_result[i];
    mask_proposals.push_back(cv::Mat(pickedProposals[idx]).t());
  }

  //
  // Prepare masks
  //
  cv::Mat protosTmp(SEG_CHANNELS, SEG_WIDTH * SEG_HEIGHT, CV_32F, seg_masks.data_ptr<float>());
  cv::Mat protos    = protosTmp.reshape(0, {SEG_CHANNELS, SEG_WIDTH * SEG_HEIGHT});
  cv::Mat matmulRes = (mask_proposals * protos).t();
  cv::Mat masks     = matmulRes.reshape(nms_result.size(), {SEG_WIDTH, SEG_HEIGHT});
  std::vector<cv::Mat> maskChannels;
  split(masks, maskChannels);

  cv::Rect holeImgRect(0, 0, inputImage.cols, inputImage.rows);
  std::vector<Result> results;
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx              = nms_result[i];
    cv::Rect boundingBox = boxes[idx] & holeImgRect;
    auto mask            = getMask(maskChannels[i], inputImage.size(), boundingBox);

    // Find contours in the binary image
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

    if(mask.empty() || mask.cols == 0 || mask.rows == 0 || contours.empty()) {
      continue;
    }

    // Look for the biggest contour area
    int idxMax = 0;
    for(int i = 1; i < contours.size(); i++) {
      if(hierarchy[i][3] == -1) {
        if(contours[i - 1].size() < contours[i].size()) {
          idxMax = i;
        }
      }
    }
    auto &contour = contours[idxMax];

    //
    // Remove pixels outside the contour
    //
    cv::Mat maskMaskTmp = cv::Mat::zeros(mask.size(), CV_8UC1);
    cv::fillPoly(maskMaskTmp, contour, cv::Scalar(255));
    cv::bitwise_and(mask, maskMaskTmp, maskMaskTmp);

    //
    // Fit the bounding box and mask to the new size
    //
    auto contourTmp = contour;
    for(auto &point : contourTmp) {
      point.x = point.x + boundingBox.x;
      point.y = point.y + boundingBox.y;
    }
    cv::Rect fittedBoundingBox = cv::boundingRect(contourTmp);
    cv::Mat shiftedMask        = cv::Mat::zeros(fittedBoundingBox.size(), CV_8UC1);
    int32_t xOffset            = fittedBoundingBox.x - boundingBox.x;
    int32_t yOffset            = fittedBoundingBox.y - boundingBox.y;
    // Transforms the old big mask to the new smaller one
    cv::Mat translationMatrix = (cv::Mat_<double>(2, 3) << 1, 0, -xOffset, 0, 1, -yOffset);
    cv::warpAffine(maskMaskTmp, shiftedMask, translationMatrix, shiftedMask.size());
    // Move by the offset of the old bounding box
    for(auto &point : contour) {
      point.x = point.x - xOffset;
      if(point.x < 0) {
        point.x = 0;
      }
      point.y = point.y - yOffset;
      if(point.y < 0) {
        point.y = 0;
      }
    }

    //
    // Apply the filter based on the object class
    //
    int32_t modelClassId = classIds[idx];

    results.push_back(
        Result{.boundingBox = fittedBoundingBox, .mask = shiftedMask, .contour = contour, .classId = modelClassId, .probability = confidences[idx]});
  }
  std::cout << "Finished" << std::endl;
  return results;
}
}    // namespace joda::ai
