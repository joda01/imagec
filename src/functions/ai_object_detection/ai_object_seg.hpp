#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

#define YOLO_P6 false

struct OutputSeg
{
  int id;
  float confidence;
  cv::Rect box;
  cv::Mat boxMask;
};

class YoloSeg
{
public:
  YoloSeg()
  {
  }
  ~YoloSeg()
  {
  }

  bool ReadModel(cv::dnn::Net &net, const std::string &netPath, bool isCuda);
  bool Detect(cv::Mat &srcImg, cv::dnn::Net &net, std::vector<OutputSeg> &output);
  void DrawPred(cv::Mat &img, std::vector<OutputSeg> result, std::vector<cv::Scalar> color);
  void LetterBox(const cv::Mat &image, cv::Mat &outImage,
                 cv::Vec4d &params,    //[ratio_x,ratio_y,dw,dh]
                 const cv::Size &newShape = cv::Size(640, 640), bool autoShape = false, bool scaleFill = false,
                 bool scaleUp = true, int stride = 32, const cv::Scalar &color = cv::Scalar(114, 114, 114));

private:
  void GetMask(const cv::Mat &maskProposals, const cv::Mat &mask_protos, const cv::Vec4d &params,
               const cv::Size &srcImgShape, std::vector<OutputSeg> &output);

#if(defined YOLO_P6 && YOLO_P6 == true)
  const float _netAnchors[4][6] = {{19, 27, 44, 40, 38, 94},
                                   {96, 68, 86, 152, 180, 137},
                                   {140, 301, 303, 264, 238, 542},
                                   {436, 615, 739, 380, 925, 792}};

  const int _netWidth    = 1280;
  const int _netHeight   = 1280;
  const int _segWidth    = 320;
  const int _segHeight   = 320;
  const int _segChannels = 32;
  const int _strideSize  = 4;    // stride size
#else
  const float _netAnchors[3][6] = {{10, 13, 16, 30, 33, 23}, {30, 61, 62, 45, 59, 119}, {116, 90, 156, 198, 373, 326}};

  const int _netWidth    = 640;
  const int _netHeight   = 640;
  const int _segWidth    = 160;
  const int _segHeight   = 160;
  const int _segChannels = 32;
  const int _strideSize  = 3;    // stride size
#endif    // YOLO_P6

  const float _netStride[4] = {8, 16, 32, 64};
  float _boxThreshold       = 0.25;
  float _classThreshold     = 0.5;
  float _nmsThreshold       = 0.45;
  float _maskThreshold      = 0.5;
  float _nmsScoreThreshold  = _boxThreshold * _classThreshold;

  std::vector<std::string> _className = {"cells"};
};
