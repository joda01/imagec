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
/// \brief     A short description what happens here.
/// \link      https://github.com/UNeedCryDear/yolov5-seg-opencv-onnxruntime-cpp

#include "ai_object_segmentation.hpp"

namespace joda::func::ai {

using namespace std;
using namespace cv;
using namespace cv::dnn;

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  onnxNetPath Path to the ONNX net file
/// \param[in]  classNames  Array of class names e.g. {"nuclues","cell"}
///
ObjectSegmentation::ObjectSegmentation(const std::string &onnxNetPath, const std::vector<std::string> &classNames) :
    mClassNames(classNames)
{
  mNet        = cv::dnn::readNet(onnxNetPath);
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
///             |x|y|w|h|confidence|class score 1|class score 2|.....  |class score n | masking ...
///             +-+-+-+-+----------+-------------+-------------+-------+--------------+---------------
///             The first two places are normalized center coordinates of the detected bounding box.
///             Then comes the normalized width and height. Index 4 has the confidence score that tells the
///             probability of the detection being an object. The following entries tell the class scores.
///
/// \author     Joachim Danmayr
/// \ref        https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
///
/// \param[in]  inputImage Image which has been used for detection
/// \return     Result of the analysis
///
auto ObjectSegmentation::forward(Mat &inputImage) -> DetectionResults
{
  DetectionResults output;
  Mat blob;
  output.clear();
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
  int net_width = mClassNames.size() + 5 + SEG_CHANNELS;
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
          ;    // Get the probability that an object is contained in the box of each row
          if(box_score >= BOX_THRESHOLD) {
            cv::Mat scores(1, mClassNames.size(), CV_32FC1, pdata + 5);
            Point classIdPoint;
            double maxClassScores;
            minMaxLoc(scores, nullptr, &maxClassScores, nullptr, &classIdPoint);
            maxClassScores = static_cast<float>(maxClassScores);
            if(maxClassScores >= CLASS_THRESHOLD) {
              vector<float> temp_proto(pdata + 5 + mClassNames.size(), pdata + net_width);
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

  // Perform non-maximum suppression to remove redundant overlapping boxes with lower confidence
  vector<int> nms_result;
  NMSBoxes(boxes, confidences, NMS_SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
  std::vector<vector<float>> temp_mask_proposals;
  Rect holeImgRect(0, 0, inputImage.cols, inputImage.rows);
  for(int i = 0; i < nms_result.size(); ++i) {
    int idx = nms_result[i];
    Detection result;
    result.index      = i;
    result.classId    = classIds[idx];
    result.confidence = confidences[idx];
    result.box        = boxes[idx] & holeImgRect;
    temp_mask_proposals.push_back(pickedProposals[idx]);
    output.push_back(result);
  }
  Mat mask_proposals;
  for(auto &temp_mask_proposal : temp_mask_proposals) {
    mask_proposals.push_back(Mat(temp_mask_proposal).t());
  }

  getMask(mask_proposals, netOutputImg[1], params, inputImage.size(), output);

  return output;
}

///
/// \brief      Extracts the mask from the prediction and stores the mask
///             to the output >output[i].boxMask<
/// \author     Joachim Danmayr
/// \param[in]  maskProposals   Mask proposal
/// \param[in]  maskProtos      Mask proto
/// \param[in]  params          Image scaling parameters
/// \param[in]  inputImageShape Image shape
/// \param[out] output          Stores the mask to the output
///
void ObjectSegmentation::getMask(const Mat &maskProposals, const Mat &maskProtos, const cv::Vec4d &params,
                                 const cv::Size &inputImageShape, DetectionResults &output)
{
  Mat protos    = maskProtos.reshape(0, {SEG_CHANNELS, SEG_WIDTH * SEG_HEIGHT});
  Mat matmulRes = (maskProposals * protos).t();
  Mat masks     = matmulRes.reshape(output.size(), {SEG_WIDTH, SEG_HEIGHT});
  vector<Mat> maskChannels;
  split(masks, maskChannels);

  for(int i = 0; i < output.size(); ++i) {
    Mat dest;
    Mat mask;
    // sigmoid
    cv::exp(-maskChannels[i], dest);
    dest = 1.0 / (1.0 + dest);

    Rect roi(static_cast<int>(params[2] / NET_WIDTH * SEG_WIDTH), static_cast<int>(params[3] / NET_HEIGHT * SEG_HEIGHT),
             static_cast<int>(SEG_WIDTH - params[2] / 2), static_cast<int>(SEG_HEIGHT - params[3] / 2));
    dest = dest(roi);
    resize(dest, mask, inputImageShape, INTER_NEAREST);

    // crop
    Rect temp_rect    = output[i].box;
    mask              = mask(temp_rect) > MASK_THRESHOLD;
    output[i].boxMask = mask;
  }
}

///
/// \brief      Image preparation
/// \author     Joachim Danmayr
///
void ObjectSegmentation::letterBox(const cv::Mat &image, cv::Mat &outImage, cv::Vec4d &params, const cv::Size &newShape,
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

///
/// \brief      Paints the masks and bounding boxes around the found elements
/// \author     Joachim Danmayr
/// \param[in]  img    Image where the mask should be painted on
/// \param[in]  result Prediction result of the forward
///
void ObjectSegmentation::paintBoundingBox(cv::Mat &img, const DetectionResults &result)
{
  Mat mask = img.clone();
  for(int i = 0; i < result.size(); i++) {
    int left      = result[i].box.x;
    int top       = result[i].box.y;
    int color_num = i;
    rectangle(img, result[i].box, mColors[result[i].classId % mColors.size()], 2, 8);
    mask(result[i].box).setTo(mColors[result[i].classId % mColors.size()], result[i].boxMask);
    string label   = mClassNames[result[i].classId] + ":" + to_string(result[i].confidence);
    int baseLine   = 0;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top            = max(top, labelSize.height);
    // rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top +
    // baseLine), Scalar(0, 255, 0), FILLED);
    putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 1, mColors[result[i].classId % mColors.size()], 2);
  }
  addWeighted(img, 0.5, mask, 0.5, 0, img);
  // imwrite("test/out.jpg", img);
}
}    // namespace joda::func::ai