///
/// \file      ai_model_bioimage.cpp
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

#include <string>
#include "backend/helper/logger/console_logger.hpp"
#undef slots
#include <ATen/core/TensorBody.h>
#include <stdexcept>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "ai_model_yolo.hpp"
#define slots Q_SLOTS

namespace joda::ai {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
AiModelYolo::AiModelYolo(const ProbabilitySettings &settings) :
    mClassThreshold(settings.classThreshold), mMaskThreshold(settings.maskThreshold), mNmsScoreThreshold(BOX_THRESHOLD * settings.classThreshold)
{
}

///
/// \brief      Post process the prediction.
///             YOLO produces an ouput array with following format
///             +-+-+-+-+----------+-------------+-------------+-------+--------------+---------------
///             |x|y|w|h|confidence|class score 1|class score 2|.....  |class
///             score n | masking ...
///             +-+-+-+-+----------+-------------+-------------+-------+--------------+---------------
///             The first two places are normalized center coordinates of the
///             detected bounding box. Then comes the normalized width and
///             height. Index 4 has the confidence score that tells the
///             probability of the detection being an object. The following
///             entries tell the class scores.
///
/// \author     Joachim Danmayr
/// \ref
/// https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
///
/// \param[in]  inputImage Image which has been used for detection
/// \return     Result of the analysis
///
auto AiModelYolo::processPrediction(const at::Device &device, const cv::Mat &inputImage, const at::IValue &tensor) -> std::vector<Result>
{
  // We assume the model returns a tuple: (detections, seg_masks)
  if(!tensor.isTuple()) {
    joda::log::logError("Expected model output to be a tuple (detections, seg_masks).");
    return {};
  }
  auto output_tuple = tensor.toTuple();
  if(output_tuple->elements().size() != 2) {
    joda::log::logError("Expected tuple with 2 elements.");
    return {};
  }
  auto detections = output_tuple->elements()[0].toTensor();    // Shape: [N, 6]

  torch::Tensor seg_masks;
  bool yolo11Detected = false;
  if(output_tuple->elements()[1].isTensor()) {
    seg_masks = output_tuple->elements()[1].toTensor();    // Shape: [N, mask_height, mask_width]
  } else if(output_tuple->elements()[1].isTuple()) {
    yolo11Detected = true;
    auto tuples    = output_tuple->elements()[1].toTuple()->elements();
    for(const auto &tuple : tuples) {
      if(tuple.isTensor()) {
        auto tensorTmp = tuple.toTensor();
        if(tensorTmp.size(0) == 1 && tensorTmp.size(1) == 32 && tensorTmp.size(2) == 160) {
          seg_masks = tensorTmp;
          break;
        }
      }
    }
  }

  // We want an ouput order of  [B, N, 4+1+C]
  // - YoloV5  has [B, N, 4+1+C]
  // - YoloV11 has [B 4+1+C, N]
  if(yolo11Detected) {
    detections = detections.permute({0, 2, 1});
  }

  int64_t lastSimSize     = detections.size(detections.dim() - 1);
  int64_t numberOfClasses = lastSimSize - 5 - SEG_CHANNELS;

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

  // int32_t mNumberOfClasses = 2;

  // [batch_size, num_predictions, (5 + num_classes)]
  /*
  batch_size is the number of images processed at once.
        num_predictions is the total number of candidate detections.
                        This number comes from flattening the predictions from all detection layers (YOLOv5
                        normally predicts at three different scales, each with its own grid size and set of anchor boxes).
        (5 + num_classes) represents the data for each prediction: The first 4 numbers correspond to the bounding box
                          parameters: x, y: The center coordinates of the box (typically relative to a grid cell
                          or normalized). w, h: The width and height of the box (often predicted as offsets/scales
                          relative to predefined anchor boxes). The 5th number is the objectness score (i.e., the confidence
                          that an object exists in that box). The remaining num_classes numbers are the class scores (one per possible
                          class). For example, if you’re using a model trained on the COCO dataset, there would be 80 class scores.
  */

  int net_width = numberOfClasses + 5 + SEG_CHANNELS;
  auto *pdata   = static_cast<float *>(detections.data_ptr());
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
            cv::Mat scores(1, numberOfClasses, CV_32FC1, pdata + 5);

            cv::Point classIdPointMax;
            cv::Point classIdPointMin;

            double maxClassScores;
            double minClassScores;
            minMaxLoc(scores, &minClassScores, &maxClassScores, &classIdPointMin, &classIdPointMax);
            maxClassScores = static_cast<float>(maxClassScores);
            if(maxClassScores >= mClassThreshold) {
              std::vector<float> temp_proto(pdata + 5 + numberOfClasses, pdata + 5 + numberOfClasses + SEG_CHANNELS);
              pickedProposals.push_back(temp_proto);
              //  rect [x,y,w,h]
              float x  = pdata[0] / ratio[0];    // x
              float y  = pdata[1] / ratio[1];    // y
              float w  = pdata[2] / ratio[0];    // w
              float h  = pdata[3] / ratio[1];    // h
              int left = MAX((x - 0.5 * w), 0);
              int top  = MAX((y - 0.5 * h), 0);

              classIds.push_back(classIdPointMax.x);
              confidences.push_back(maxClassScores * box_score);
              boxes.emplace_back(left, top, static_cast<int>(w), static_cast<int>(h));
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
    if(maskChannels[i].empty()) {
      continue;
    }

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
  return results;
}

///
/// \brief      Extracts the mask from the prediction and stores the mask
///             to the output >output[i].boxMask<
/// \author     Joachim Danmayr
/// \param[in]  maskChannel     Mask proposal
/// \param[in]  params          Image scaling parameters
/// \param[in]  inputImageShape Image shape
/// \param[out] output          Stores the mask to the output
///
auto AiModelYolo::getMask(const cv::Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) const -> cv::Mat
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
  if(!mask.empty()) {
    mask = mask > mMaskThreshold;
  }

  return mask;
}

}    // namespace joda::ai
