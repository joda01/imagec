///
/// \file      ai_lassifier_onnx.cpp
/// \author    Joachim Danmayr
/// \date      2025-01-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include <stdexcept>
#include "ai_lassifier_onnx.hpp"

namespace joda::ai {

using namespace std;
using namespace cv;
using namespace cv::dnn;

AiClassifierOnnx::AiClassifierOnnx(const settings::AiClassifierSettings &settings) :
    mSettings(settings), mClassThreshold(settings.classThreshold), mNmsScoreThreshold(BOX_THRESHOLD * settings.classThreshold),
    mMaskThreshold(settings.maskThreshold)
{
  mProbabilityHandicap.clear();
  for(const auto &modelClass : settings.modelClasses) {
    mProbabilityHandicap.emplace(modelClass.modelClassId, modelClass.probabilityHandicap);
  }
}

// weight of size [64, 1, 3, 3], expected input[1, 3, 256, 256] to have 1 channels, but got 3 channels instead
// Expected 4-dimensional input for 4-dimensional weight [64, 1, 3, 3], but got 5-dimensional input of size [1, 1, 256, 256, 3] instead

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
auto AiClassifierOnnx::execute(const cv::Mat &originalImage) -> std::vector<ai::AiSegmentation::Result>
{
  cv::dnn::Net mNet;

  // Load model
  try {
    mNet        = cv::dnn::readNet(mSettings.modelPath);
    bool isCuda = false;
    if(isCuda) {
      mNet.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
      mNet.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
    } else {
      mNet.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
      mNet.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
  } catch(const std::exception &ex) {
    throw std::runtime_error("Cannot load AI model! What: " + std::string(ex.what()));
  }
  const cv::Mat &inputImageOriginal = originalImage;

  // Normalize the pixel values to [0, 255] float for detection
  cv::Mat grayImageFloat;
  inputImageOriginal.convertTo(grayImageFloat, CV_32F, 255.0 / 65535.0);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  Mat blob;
  Mat netInputImg = inputImage.clone();
  blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(NET_WIDTH, NET_HEIGHT), cv::Scalar(0, 0, 0), true, false);
  mNet.setInput(blob);
  std::vector<cv::Mat> netOutputImg;
  mNet.forward(netOutputImg, mNet.getUnconnectedOutLayersNames());

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
  std::vector<vector<float>> pickedProposals;    // output0[:,:, 5 + mClassNames.size():net_width]

  int net_width = mSettings.numberOfModelClasses + 5 + SEG_CHANNELS;
  float *pdata  = (float *) netOutputImg[0].data;
  for(int stride = 0; stride < STRIDE_SIZE; stride++) {    // stride
    int grid_x = static_cast<int>(NET_WIDTH / NET_STRIDE[stride]);
    int grid_y = static_cast<int>(NET_HEIGHT / NET_STRIDE[stride]);
    for(int anchor = 0; anchor < 3; anchor++) {    // anchors
      const float anchor_w = NET_ANCHORS[stride][anchor * 2];
      const float anchor_h = NET_ANCHORS[stride][anchor * 2 + 1];
      for(int i = 0; i < grid_y; ++i) {
        for(int j = 0; j < grid_x; ++j) {
          if(pdata == nullptr || *pdata == 0) {
            continue;
          }
          float box_score = pdata[4];

          // Get the probability that an object is contained in the box of
          // each row
          if(box_score >= BOX_THRESHOLD) {
            cv::Mat scores(1, mSettings.numberOfModelClasses, CV_32FC1, pdata + 5);

            // Add a handicap for the probability to rank some classes up or down
            for(int32_t modelClassIdx = 0; modelClassIdx < scores.cols; modelClassIdx++) {
              scores.at<float>(modelClassIdx) = scores.at<float>(modelClassIdx) * mProbabilityHandicap[modelClassIdx];
            }

            Point classIdPointMax;
            Point classIdPointMin;

            double maxClassScores;
            double minClassScores;
            minMaxLoc(scores, &minClassScores, &maxClassScores, &classIdPointMin, &classIdPointMax);
            maxClassScores = static_cast<float>(maxClassScores);
            if(maxClassScores >= mClassThreshold) {
              vector<float> temp_proto(pdata + 5 + mSettings.numberOfModelClasses, pdata + net_width);
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
              boxes.push_back(Rect(left, top, static_cast<int>(w), static_cast<int>(h)));
            }
          }
          pdata += net_width;
        }
      }
    }
  }

  // Perform non-maximum suppression to remove redundant overlapping boxes with
  // lower confidence
  vector<int> nms_result;
  NMSBoxes(boxes, confidences, mNmsScoreThreshold, NMS_THRESHOLD, nms_result);

  Mat mask_proposals;
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx = nms_result[i];
    mask_proposals.push_back(Mat(pickedProposals[idx]).t());
  }

  //
  // Prepare masks
  //
  Mat protos    = netOutputImg[1].reshape(0, {SEG_CHANNELS, SEG_WIDTH * SEG_HEIGHT});
  Mat matmulRes = (mask_proposals * protos).t();
  Mat masks     = matmulRes.reshape(nms_result.size(), {SEG_WIDTH, SEG_HEIGHT});
  vector<Mat> maskChannels;
  split(masks, maskChannels);
  std::vector<ai::AiSegmentation::Result> result;

  Rect holeImgRect(0, 0, inputImage.cols, inputImage.rows);
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx              = nms_result[i];
    cv::Rect boundingBox = boxes[idx] & holeImgRect;
    auto mask            = getMask(maskChannels[i], inputImageOriginal.size(), boundingBox);

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

    result.push_back(AiSegmentation::Result{.boundingBox = fittedBoundingBox,
                                            .mask        = std::move(shiftedMask),
                                            .contour     = std::move(contour),
                                            .classId     = classIds[idx],
                                            .probability = confidences[idx]});
  }
  return result;
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
auto AiClassifierOnnx::getMask(const Mat &maskChannel, const cv::Size &inputImageShape, const cv::Rect &box) -> cv::Mat
{
  if(maskChannel.empty()) {
    return {};
  }
  static const Rect roi(0, 0, static_cast<int>(SEG_WIDTH), static_cast<int>(SEG_HEIGHT));
  Mat dest;
  Mat mask;
  cv::exp(-maskChannel, dest);    // sigmoid
  dest = (1.0 / (1.0 + dest))(roi);
  resize(dest, mask, inputImageShape, INTER_NEAREST);
  mask = mask(box).clone();
  mask = mask > mMaskThreshold;

  return mask;
}

}    // namespace joda::ai
