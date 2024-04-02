///
/// \file      ai_object_detection.h
/// \author    Joachim Danmayr
/// \date      2023-02-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "ai_object_detection.h"
#include <set>
#include <string>
#include "../detection.hpp"
#include <opencv2/core/cuda.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

namespace joda::func::ai {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  onnxNetPath Path to the ONNX net file
/// \param[in]  classNames  Array of class names e.g. {"nuclues","cell"}
///
ObjectDetector::ObjectDetector(const joda::settings::json::ChannelFiltering *filt,
                               const joda::onnx::OnnxParser::Data &model) :
    DetectionFunction(filt),
    mNet{cv::dnn::readNet(model.modelPath)}, mClassNames(model.classes)
{
}

///
/// \brief      Analyse the image
/// \author     Joachim Danmayr
/// \param[in]  inputImage      Image to analyze
/// \return     Result of the analysis
///
auto ObjectDetector::forward(const cv::Mat &inputImageOriginal, const cv::Mat &originalImage, int32_t channelIndex)
    -> DetectionResponse
{
  // Normalize the pixel values to [0, 255] float for detection
  cv::Mat grayImageFloat;
  inputImageOriginal.convertTo(grayImageFloat, CV_32F, 255.0 / 65535.0);
  cv::Mat inputImage;
  cv::cvtColor(grayImageFloat, inputImage, cv::COLOR_GRAY2BGR);

  // cv::cuda::setDevice(0);
  cv::Mat blob;
  cv::dnn::blobFromImage(inputImage, blob, 1. / 255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false,
                         CV_32F);
  mNet.setInput(blob);
  std::vector<cv::Mat> outputs;
  mNet.forward(outputs, mNet.getUnconnectedOutLayersNames());
  auto results = postProcessing(inputImageOriginal, originalImage, outputs, channelIndex);
  return {.result = results, .controlImage = inputImage};
}

///
/// \brief      Post process the prediction.
///             YOLO produces an ouput array with following format
///             +-+-+-+-+----------+-------------+-------------+--------
///             |x|y|w|h|confidence|class score 1|class score 2|.....
///             +-+-+-+-+----------+-------------+-------------+--------
///             The first two places are normalized center coordinates of the detected bounding box.
///             Then comes the normalized width and height. Index 4 has the confidence score that tells the
///             probability of the detection being an object. The following entries tell the class scores.
///
/// \author     Joachim Danmayr
/// \ref        https://learnopencv.com/object-detection-using-yolov5-and-opencv-dnn-in-c-and-python/
///
/// \param[in]  inputImage Image which has been used for detection
/// \param[out] outputs    predictionMatrix Matrix which holds the prediction result (see brief)
/// \return Retruens the prepared prediction result
///
auto ObjectDetector::postProcessing(const cv::Mat &inputImage, const cv::Mat &originalImage,
                                    const std::vector<cv::Mat> &predictionMatrix, int32_t channelIndex)
    -> DetectionResults

{
  // Initialize vectors to hold respective outputs while unwrapping     detections.
  std::vector<int> classIds;
  std::vector<float> confidences;
  std::vector<cv::Rect> boxes;
  // Resizing factor.
  float x_factor       = inputImage.cols / INPUT_WIDTH;
  float y_factor       = inputImage.rows / INPUT_HEIGHT;
  float *data          = (float *) predictionMatrix[0].data;
  const int dimensions = predictionMatrix[0].size[2];
  // 25200 for default size 640.
  const int rows = 25200;
  // Iterate through 25200 detections.
  for(int i = 0; i < rows; ++i) {
    /* std::cout << std::fixed << std::setw(4) << std::setprecision(2) << std::to_string(data[0]) << "\t"
               << std::to_string(data[1]) << "\t" << std::to_string(data[2]) << "\t" << std::to_string(data[3]) <<
       "\t"
               << std::to_string(data[4]) << std::endl;*/

    float confidence = data[4];
    // Discard bad detections and continue.
    if(confidence >= CONFIDENCE_THRESHOLD) {
      float *classes_scores = data + 5;
      // Create a 1x85 Mat and store class scores of 80 classes.
      cv::Mat scores(1, mClassNames.size(), CV_32FC1, classes_scores);
      // Perform minMaxLoc and acquire the index of best class  score.
      cv::Point class_id;
      double max_class_score;
      minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
      // Continue if the class score is above the threshold.
      if(max_class_score > SCORE_THRESHOLD) {
        // Store class ID and confidence in the pre-defined respective vectors.
        confidences.push_back(confidence);
        classIds.push_back(class_id.x);
        // Center.
        float cx = data[0];
        float cy = data[1];
        // Box dimension.
        float w = data[2];
        float h = data[3];
        // Bounding box coordinates.
        int left = int((cx - 0.5 * w) * x_factor);
        if(left < 0) {
          left = 0;
        }
        int top = int((cy - 0.5 * h) * y_factor);
        if(top < 0) {
          top = 0;
        }
        int width = int(w * x_factor);
        if(left + width > inputImage.cols) {
          width = inputImage.cols - left;
        }
        int height = int(h * y_factor);
        if(top + height > inputImage.rows) {
          height = inputImage.rows - top;
        }
        // Store good detections in the boxes vector.
        boxes.push_back(cv::Rect(left, top, width, height));
      }
    }
    // Jump to the next row.
    data += dimensions;
  }

  //
  // Non Maximum Suppression (NMS) is a technique used in numerous computer vision tasks.
  // It is a class of algorithms to select one entity (e.g., bounding boxes) out of many overlapping entities.
  // We can choose the selection criteria to arrive at the desired results. The criteria are most commonly some
  // form of probability number and some form of overlap measure (e.g. Intersection over Union).
  //
  /// \ref https://learnopencv.com/non-maximum-suppression-theory-and-implementation-in-pytorch/
  //
  std::vector<int> keptBoxIdx;    // Contains the indices of the boxes which are kept after the Non maximum suppression
  cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, keptBoxIdx);

  // Convert to set for faster search afterwards
  std::set<int> keptIndexesSet;
  for(const auto i : keptBoxIdx) {
    keptIndexesSet.emplace(i);
  }

  //
  // Remove all elements which where suppressed by the NMS algoeithm
  //
  DetectionResults result;
  uint32_t index = 0;
  for(int n = 0; n < confidences.size(); n++) {
    if(keptIndexesSet.count(n) == 1) {
      cv::Mat boxMask = cv::Mat::ones(inputImage.size(), CV_8UC1);
      boxMask         = boxMask(boxes[n]) >= 0;

      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(boxMask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
      if(contours.empty()) {
        contours.emplace_back();
      }
      ROI roi(index, confidences[n], classIds[n], boxes[n], boxMask, contours[0], originalImage, channelIndex,
              getFilterSettings());
      result.push_back(roi);
      index++;
    }
  }

  return result;
}

}    // namespace joda::func::ai
