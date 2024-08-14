///
/// \file      ai_object_segmentation.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

/// \link      https://github.com/UNeedCryDear/yolov5-seg-opencv-onnxruntime-cpp

#include "ai_classifier.hpp"
#include <memory>
#include <string>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/enums/enum_objects.hpp"
#include "backend/helper/duration_count/duration_count.h"
#include <opencv2/core/matx.hpp>
#include <opencv2/core/persistence.hpp>
#include <opencv2/imgproc.hpp>

namespace joda::cmd {

using namespace std;
using namespace cv;
using namespace cv::dnn;

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  onnxNetPath Path to the ONNX net file
/// \param[in]  classNames  Array of class names e.g. {"nuclues","cell"}
///
AiClassifier::AiClassifier(const settings::AiClassifierSettings &settings) :
    mSettings(settings), mNumberOfClasses(settings.numberOfClasses), mClassThreshold(settings.classThreshold),
    mNmsScoreThreshold(settings.classThreshold * BOX_THRESHOLD)
{
  mNet        = cv::dnn::readNet(settings.modelPath);
  bool isCuda = false;
  if(isCuda) {
    mNet.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
    mNet.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
  } else {
    mNet.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
    mNet.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
  }
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
void AiClassifier::execute(processor::ProcessContext &context, cv::Mat &imageNotUse, atom::ObjectList &result)
{
  const cv::Mat &inputImageOriginal = imageNotUse;

  // Normalize the pixel values to [0, 255] float for detection
  cv::Mat grayImageFloat;
  inputImageOriginal.convertTo(grayImageFloat, CV_32F, 255.0 / 65535.0);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  Mat blob;
  int col    = inputImage.cols;
  int row    = inputImage.rows;
  int maxLen = MAX(col, row);
  Mat netInputImg;
  Vec4d params;
  letterBox(inputImage, netInputImg, params, cv::Size(NET_WIDTH, NET_HEIGHT));
  blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(NET_WIDTH, NET_HEIGHT), cv::Scalar(0, 0, 0), true, false);
  mNet.setInput(blob);
  std::vector<cv::Mat> netOutputImg;
  mNet.forward(netOutputImg, mNet.getUnconnectedOutLayersNames());

  // vector<string> outputLayerName{"output0", "output1"};
  // mNet.forward(netOutputImg, outputLayerName);

  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  std::vector<vector<float>> pickedProposals;    // output0[:,:, 5 + mClassNames.size():net_width]

  float ratio_h = static_cast<float>(netInputImg.rows) / NET_HEIGHT;
  float ratio_w = static_cast<float>(netInputImg.cols) / NET_WIDTH;
  int net_width = mNumberOfClasses + 5 + SEG_CHANNELS;
  float *pdata  = (float *) netOutputImg[0].data;
  for(int stride = 0; stride < STRIDE_SIZE; stride++) {    // stride
    int grid_x = static_cast<int>(NET_WIDTH / NET_STRIDE[stride]);
    int grid_y = static_cast<int>(NET_HEIGHT / NET_STRIDE[stride]);
    for(int anchor = 0; anchor < 3; anchor++) {    // anchors
      const float anchor_w = NET_ANCHORS[stride][anchor * 2];
      const float anchor_h = NET_ANCHORS[stride][anchor * 2 + 1];
      for(int i = 0; i < grid_y; ++i) {
        for(int j = 0; j < grid_x; ++j) {
          float box_score = pdata[4];
          // Get the probability that an object is contained in the box of
          // each row
          if(box_score >= BOX_THRESHOLD) {
            cv::Mat scores(1, mNumberOfClasses, CV_32FC1, pdata + 5);
            Point classIdPoint;
            double maxClassScores;
            minMaxLoc(scores, nullptr, &maxClassScores, nullptr, &classIdPoint);
            maxClassScores = static_cast<float>(maxClassScores);
            if(maxClassScores >= mClassThreshold) {
              vector<float> temp_proto(pdata + 5 + mNumberOfClasses, pdata + net_width);
              pickedProposals.push_back(temp_proto);
              // rect [x,y,w,h]
              float x  = (pdata[0] - params[2]) / params[0];    // x
              float y  = (pdata[1] - params[3]) / params[1];    // y
              float w  = pdata[2] / params[0];                  // w
              float h  = pdata[3] / params[1];                  // h
              int left = MAX((x - 0.5 * w) * ratio_w, 0);
              int top  = MAX((y - 0.5 * h) * ratio_h, 0);
              classIds.push_back(classIdPoint.x);
              confidences.push_back(maxClassScores * box_score);
              boxes.push_back(Rect(left, top, static_cast<int>(w * ratio_w), static_cast<int>(h * ratio_h)));
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

  Rect holeImgRect(0, 0, inputImage.cols, inputImage.rows);
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx      = nms_result[i];
    cv::Rect box = boxes[idx] & holeImgRect;
    auto mask    = getMask(maskChannels[i], params, inputImageOriginal.size(), box);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    if(contours.empty()) {
      contours.emplace_back();
    }
    // Look for the biggest contour area
    int idxMax = 0;
    for(int i = 1; i < contours.size(); i++) {
      if(contours[i - 1].size() < contours[i].size()) {
        idxMax = i;
      }
    }
    int32_t classId = classIds[idx];

    //
    // Apply the filter based on the object class
    //
    if(mSettings.objectClasses.size() > classId) {
      const auto &objectClass = mSettings.objectClasses.at(classId);

      joda::atom::ROI detectedRoi(
          atom::ROI::RoiObjectId{
              .objectId  = context.acquireNextObjectId(),
              .clusterId = context.getClusterId(objectClass.noMatchingClusterId),
              .classId   = context.getClassId(objectClass.noMatchingClassId),
              .iteration = context.getActIterator(),
          },
          context.pipelineContext.actImage.appliedMinThreshold, 0, box, mask, contours[idxMax], context.getImageSize());

      for(const auto &filter : objectClass.filters) {
        const auto &cachedImage = context.loadImageFromCache(filter.intensity->imageId);
        // If filter matches assign the new cluster and class to the ROI
        if(filter.doesFilterMatch(detectedRoi, *cachedImage)) {
          detectedRoi.setClusterAndClass(context.getClusterId(filter.clusterId), context.getClassId(filter.classId));
        }
      }
    }
  }
}

///
/// \brief      Extracts the mask from the prediction and stores the mask
///             to the output >output[i].boxMask<
/// \author     Joachim Danmayr
/// \param[in]  image           Original image
/// \param[in]  maskProposals   Mask proposal
/// \param[in]  maskProtos      Mask proto
/// \param[in]  params          Image scaling parameters
/// \param[in]  inputImageShape Image shape
/// \param[out] output          Stores the mask to the output
///
auto AiClassifier::getMask(const Mat &maskChannel, const cv::Vec4d &params, const cv::Size &inputImageShape,
                           const cv::Rect &box) -> cv::Mat
{
  static const Rect roi(static_cast<int>(params[2] / NET_WIDTH * SEG_WIDTH),
                        static_cast<int>(params[3] / NET_HEIGHT * SEG_HEIGHT),
                        static_cast<int>(SEG_WIDTH - params[2] / 2), static_cast<int>(SEG_HEIGHT - params[3] / 2));

  Mat dest;
  Mat mask;
  cv::exp(-maskChannel, dest);    // sigmoid
  dest = (1.0 / (1.0 + dest))(roi);
  resize(dest, mask, inputImageShape, INTER_NEAREST);
  mask = mask(box) > MASK_THRESHOLD;
  return mask;
}

///
/// \brief      Image preparation
/// \author     Joachim Danmayr
///
void AiClassifier::letterBox(const cv::Mat &image, cv::Mat &outImage, cv::Vec4d &params, const cv::Size &newShape,
                             bool autoShape, bool scaleFill, bool scaleUp, int stride, const cv::Scalar &color)
{
  // if(false) {
  //   int maxLen = MAX(image.rows, image.cols);
  //   outImage   = Mat::zeros(Size(maxLen, maxLen), CV_8UC3);
  //   image.copyTo(outImage(Rect(0, 0, image.cols, image.rows)));
  //   params[0] = 1;
  //   params[1] = 1;
  //   params[3] = 0;
  //   params[2] = 0;
  // }

  cv::Size shape = image.size();
  float r        = std::min(static_cast<float>(newShape.height) / static_cast<float>(shape.height),
                            static_cast<float>(newShape.width) / static_cast<float>(shape.width));
  if(!scaleUp) {
    r = std::min(r, 1.0F);
  }

  float ratio[2]{r, r};
  int newUnpad[2]{static_cast<int>(std::round(static_cast<float>(shape.width) * r)),
                  static_cast<int>(std::round(static_cast<float>(shape.height) * r))};

  auto dw = static_cast<float>(newShape.width - newUnpad[0]);
  auto dh = static_cast<float>(newShape.height - newUnpad[1]);

  if(autoShape) {
    dw = static_cast<float>(static_cast<int>(dw) % stride);
    dh = static_cast<float>(static_cast<int>(dh) % stride);
  } else if(scaleFill) {
    dw          = 0.0F;
    dh          = 0.0F;
    newUnpad[0] = newShape.width;
    newUnpad[1] = newShape.height;
    ratio[0]    = static_cast<float>(newShape.width) / static_cast<float>(shape.width);
    ratio[1]    = static_cast<float>(newShape.height) / static_cast<float>(shape.height);
  }

  dw /= 2.0F;
  dh /= 2.0F;

  if(shape.width != newUnpad[0] && shape.height != newUnpad[1]) {
    cv::resize(image, outImage, cv::Size(newUnpad[0], newUnpad[1]));
  } else {
    outImage = image.clone();
  }

  int top    = static_cast<int>(std::round(dh - 0.1F));
  int bottom = static_cast<int>(std::round(dh + 0.1F));
  int left   = static_cast<int>(std::round(dw - 0.1F));
  int right  = static_cast<int>(std::round(dw + 0.1F));
  params[0]  = ratio[0];
  params[1]  = ratio[1];
  params[2]  = left;
  params[3]  = top;
  cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
}

}    // namespace joda::cmd
