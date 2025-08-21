///
/// \file      threshold_adaptive.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "threshold_adaptive.hpp"
#include <string>
#include "backend/commands/image_functions/rank_filter/rank_filter_algo.hpp"

namespace joda::cmd {

///
/// \brief         Bernsen recommends WIN_SIZE = 31 and CONTRAST_THRESHOLD = 15.
///                 1) Bernsen J. (1986) "Dynamic Thresholding of Grey-Level Images"
///                   Proc. of the 8th Int. Conf. on Pattern Recognition, pp. 1251-1255
///                 2) Sezgin M. and Sankur B. (2004) "Survey over Image Thresholding
///                  Techniques and Quantitative Performance Evaluation" Journal of
///                  Electronic Imaging, 13(1): 146-165
///                  http://citeseer.ist.psu.edu/sezgin04survey.html
///                Ported to ImageJ plugin from E Celebi's fourier_0.8 routines
///                This version uses a circular local window, instead of a rectagular one
///
///
void Bernsen(cv::Mat &imp, int radius, double par1, double /*par2*/, int32_t /*c_value*/, bool doIwhite)
{
  int contrast_threshold = 15;
  int local_contrast     = 0;
  int mid_gray           = 0;
  uint16_t object        = 0;
  uint16_t backg         = 0;
  int temp               = 0;

  if(par1 != 0) {
    contrast_threshold = static_cast<int>(par1);
  }

  if(doIwhite) {
    object = 0xffff;
    backg  = 0;
  } else {
    object = 0;
    backg  = 0xffff;
  }

  //
  // Maximum
  //
  auto ipMax = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMax, radius, algo::RankFilter::MAX);
  }

  //
  // Minimum
  //
  auto ipMin = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMin, radius, algo::RankFilter::MIN);
  }

  cv::Mat outImg = imp.clone();
  for(int i = 0; i < static_cast<int>(imp.total()); i++) {
    local_contrast = static_cast<int32_t>(ipMax.at<uint16_t>(i) & 0xffff) - (ipMin.at<uint16_t>(i) & 0xffff);
    mid_gray       = (static_cast<int32_t>(ipMin.at<uint16_t>(i) & 0xffff) + (ipMax.at<uint16_t>(i) & 0xffff)) / 2;
    temp           = static_cast<int32_t>(imp.at<uint16_t>(i) & 0x0000ff);
    if(local_contrast < contrast_threshold) {
      outImg.at<uint16_t>(i) = (mid_gray >= 32768) ? object : backg;    // Low contrast region
    } else {
      outImg.at<uint16_t>(i) = (temp >= mid_gray) ? object : backg;
    }
  }
  imp = outImg;
}

///
/// G. Landini, 2013
/// Based on a simple contrast toggle. This procedure does not have user-provided parameters other than the kernel radius
/// Sets the pixel value to either white or black depending on whether its current value is closest to the local Max or Min respectively
/// The procedure is similar to Toggle Contrast Enhancement (see Soille, Morphological Image Analysis (2004), p. 259
///
void Contrast(cv::Mat &imp, int radius, double /*par1*/, double /*par2*/, int32_t /*c_value*/, bool doIwhite)
{
  uint16_t object = 0;
  uint16_t backg  = 0;

  if(doIwhite) {
    object = 0xffff;
    backg  = 0;
  } else {
    object = 0;
    backg  = 0xffff;
  }

  //
  // Maximum
  //
  auto ipMax = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMax, radius, algo::RankFilter::MAX);
  }

  //
  // Minimum
  //
  auto ipMin = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMin, radius, algo::RankFilter::MIN);
  }

  cv::Mat outImg = imp.clone();
  for(int i = 0; i < static_cast<int>(imp.total()); i++) {
    outImg.at<uint16_t>(i) = ((std::abs((ipMax.at<uint16_t>(i) & 0xffff) - (outImg.at<uint16_t>(i) & 0xffff)) <=
                               std::abs((outImg.at<uint16_t>(i) & 0xffff) - (ipMin.at<uint16_t>(i) & 0xffff))) &&
                              ((outImg.at<uint16_t>(i) & 0xffff) != 0))
                                 ? object
                                 : backg;
  }
  imp = outImg;
}

//
/// See: Image Processing Learning Resourches HIPR2
/// http://homepages.inf.ed.ac.uk/rbf/HIPR2/adpthrsh.htm
///
void Mean(cv::Mat &imp, int radius, double /*par1*/, double /*par2*/, int32_t c_value, bool doIwhite)
{
  uint16_t object = 0;
  uint16_t backg  = 0;

  if(doIwhite) {
    object = 0xffff;
    backg  = 0;
  } else {
    object = 0;
    backg  = 0xffff;
  }

  //
  // Mean
  //
  auto ipMean = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMean, radius, algo::RankFilter::MEAN);
  }

  cv::Mat outImg = imp.clone();
  for(int i = 0; i < static_cast<int>(imp.total()); i++) {
    outImg.at<uint16_t>(i) = ((outImg.at<uint16_t>(i) & 0xffff) > (ipMean.at<uint16_t>(i) - c_value)) ? object : backg;
  }
  imp = outImg;
}

void Median(cv::Mat &imp, int radius, double /*par1*/, double /*par2*/, int32_t c_value, bool doIwhite)
{
  uint16_t object = 0;
  uint16_t backg  = 0;

  if(doIwhite) {
    object = 0xffff;
    backg  = 0;
  } else {
    object = 0;
    backg  = 0xffff;
  }

  //
  // Mean
  //
  auto ipMean = imp.clone();
  {
    algo::RankFilter rf;
    rf.rank(ipMean, radius, algo::RankFilter::MEDIAN);
  }

  cv::Mat outImg = imp.clone();
  for(int i = 0; i < static_cast<int>(imp.total()); i++) {
    outImg.at<uint16_t>(i) = ((outImg.at<uint16_t>(i) & 0xffff) > (ipMean.at<uint16_t>(i) - c_value)) ? object : backg;
  }
  imp = outImg;
}

cv::Mat calcHistogram(const cv::Mat &charImg)
{
  // Calculate the histogram of the image
  int histSize           = UINT16_MAX + 1;    // Number of bins
  float range[]          = {0, 65536};        // Pixel value range
  const float *histRange = {range};
  cv::Mat histogram;
  cv::calcHist(&charImg, 1, nullptr, cv::Mat(), histogram, 1, &histSize, &histRange);
  return histogram;
}

void Otsu(cv::Mat &imp, int radius, double /*par1*/, double /*par2*/, int32_t /*c_value*/, bool doIwhite)
{
  // Otsu's threshold algorithm
  // M. Emre Celebi 6.15.2007, Fourier Library https://sourceforge.net/projects/fourier-ipal/
  // ported to ImageJ plugin by G.Landini. Same algorithm as in Auto_Threshold, this time for local circular regions

  cv::Mat data;
  int w = imp.cols;
  int h = imp.rows;
  int position;
  int radiusx2 = radius * 2;
  cv::Mat pixelsOut(imp.size(), CV_16UC1);
  uint16_t object;
  uint16_t backg;

  if(doIwhite) {
    object = 0xffff;
    backg  = 0;
  } else {
    object = 0;
    backg  = 0xffff;
  }

  int ih;
  int roiy;
  static const int L = 65536;    // L is for 8bit images.
  int threshold;
  int num_pixels;
  double total_mean;    // mean gray-level for the whole image
  double bcv;
  double term;            // between-class variance, scaling term
  double max_bcv;         // max BCV
  double cnh[65536];      // cumulative normalized histogram
  double mean[65536];     // mean gray-level
  double histo[65536];    // normalized histogram

  // ip.setRoi(roi);
  for(int y = 0; y < h; y++) {
    roiy = y - radius;
    for(int x = 0; x < w; x++) {
      // Todo replace by OVAL
      int roix = x - radius;
      if(roix < 0) {
        roix = 0;
      }
      if(roiy < 0) {
        roiy = 0;
      }
      int radiusx2X = radius * 2;
      int radiusx2Y = radius * 2;
      if(roix + radiusx2 > w) {
        radiusx2X = radiusx2X - (roix + radiusx2X - w);
      }
      if(roiy + radiusx2 > h) {
        radiusx2Y = radiusx2Y - (roiy + radiusx2Y - h);
      }

      auto roi     = cv::Rect(roix, roiy, radiusx2X, radiusx2Y);
      cv::Mat part = imp(roi);
      // ip.setRoi(new OvalRoi(x-radius, roiy, radiusx2, radiusx2));
      position = x + y * w;
      data     = calcHistogram(part);

      //----
      // Calculate total numbre of pixels
      num_pixels = 0;

      for(ih = 0; ih < L; ih++) {
        num_pixels = num_pixels + static_cast<int>(data.at<float>(ih));
      }

      term = 1.0 / static_cast<double>(num_pixels);

      // Calculate the normalized histogram
      for(ih = 0; ih < L; ih++) {
        histo[ih] = term * static_cast<int>(data.at<float>(ih));
      }

      // Calculate the cumulative normalized histogram
      cnh[0] = histo[0];
      for(ih = 1; ih < L; ih++) {
        cnh[ih] = cnh[ih - 1] + histo[ih];
      }

      mean[0] = 0.0;

      for(ih = 0 + 1; ih < L; ih++) {
        mean[ih] = mean[ih - 1] + ih * histo[ih];
      }

      total_mean = mean[L - 1];

      //	Calculate the BCV at each gray-level and find the threshold that maximizes it
      threshold = 0;    // Integer.MIN_VALUE;
      max_bcv   = 0.0;

      for(ih = 0; ih < L; ih++) {
        bcv = total_mean * cnh[ih] - mean[ih];
        bcv *= bcv / (cnh[ih] * (1.0 - cnh[ih]));

        if(max_bcv < bcv) {
          max_bcv   = bcv;
          threshold = ih;
        }
      }

      // std::cout << "TH: " << std::to_string(threshold) << std::endl;
      pixelsOut.at<uint16_t>(position) =
          ((imp.at<uint16_t>(position) & 0xffff) > threshold || (imp.at<uint16_t>(position) & 0xffff) == 65535) ? object : backg;
    }
  }
  for(position = 0; position < w * h; position++) {
    imp.at<uint16_t>(position) = pixelsOut.at<uint16_t>(position);    // update with thresholded pixels
  }
}

/*
void MidGrey(const cv::Mat &image, int radius, double par1, double par2, int32_t c_value, bool doIwhite)
{
  // See: Image Processing Learning Resourches HIPR2
  // http://homepages.inf.ed.ac.uk/rbf/HIPR2/adpthrsh.htm
  ImagePlus Maximp, Minimp;
  ImageProcessor ip = imp.getProcessor(), ipMax, ipMin;
  int c_value       = 0;
  int mid_gray;
  int8_t object;
  int8_t backg;

  if(par1 != 0) {
    if(IJ.debugMode)
      IJ.log("MidGrey: changed c_value from :" + c_value + "  to:" + par1);
    c_value = (int) par1;
  }

  if(doIwhite) {
    object = (int8_t) 0xff;
    backg  = (int8_t) 0;
  } else {
    object = (int8_t) 0;
    backg  = (int8_t) 0xff;
  }

  Maximp         = duplicateImage(ip);
  ipMax          = Maximp.getProcessor();
  RankFilters rf = new RankFilters();
  rf.rank(ipMax, radius, rf.MAX);    // Maximum
  // Maximp.show();
  Minimp = duplicateImage(ip);
  ipMin  = Minimp.getProcessor();
  rf.rank(ipMin, radius, rf.MIN);    // Minimum
  // Minimp.show();
  int8_t[] pixels = (int8_t[]) ip.getPixels();
  int8_t[] max    = (int8_t[]) ipMax.getPixels();
  int8_t[] min    = (int8_t[]) ipMin.getPixels();

  for(int i = 0; i < pixels.length; i++) {
    pixels[i] = ((int) (pixels[i] & 0xff) > ((((int) (max[i] & 0xff) + (int) (min[i] & 0xff)) / 2) - c_value)) ? object : backg;
  }
  // imp.updateAndDraw();
  return;
}

void Niblack(const cv::Mat &image, int radius, double par1, double par2, bool doIwhite)
{
  // Niblack recommends K_VALUE = -0.2 for images with black foreground
  // objects, and K_VALUE = +0.2 for images with white foreground objects.
  // Niblack W. (1986) "An introduction to Digital Image Processing" Prentice-Hall.
  // Ported to ImageJ plugin from E Celebi's fourier_0.8 routines
  // This version uses a circular local window, instead of a rectagular one

  ImagePlus Meanimp, Varimp;
  ImageProcessor ip = imp.getProcessor(), ipMean, ipVar;
  double k_value;
  int c_value = 0;

  int8_t object;
  int8_t backg;

  if(doIwhite) {
    k_value = 0.2;
    object  = (int8_t) 0xff;
    backg   = (int8_t) 0;
  } else {
    k_value = -0.2;
    object  = (int8_t) 0;
    backg   = (int8_t) 0xff;
  }

  if(par1 != 0) {
    if(IJ.debugMode)
      IJ.log("Niblack: changed k_value from :" + k_value + "  to:" + par1);
    k_value = par1;
  }

  if(par2 != 0) {
    if(IJ.debugMode)
      IJ.log("Niblack: changed c_value from :" + c_value + "  to:" + par2);    // requested feature, not in original
    c_value = (int) par2;
  }

  Meanimp           = duplicateImage(ip);
  ImageConverter ic = new ImageConverter(Meanimp);
  ic.convertToGray32();

  ipMean         = Meanimp.getProcessor();
  RankFilters rf = new RankFilters();
  rf.rank(ipMean, radius, rf.MEAN);    // Mean
  // Meanimp.show();
  Varimp = duplicateImage(ip);
  ic     = new ImageConverter(Varimp);
  ic.convertToGray32();
  ipVar = Varimp.getProcessor();
  rf.rank(ipVar, radius, rf.VARIANCE);    // Variance
  // Varimp.show();
  int8_t[] pixels = (int8_t[]) ip.getPixels();
  float[] mean    = (float[]) ipMean.getPixels();
  float[] var     = (float[]) ipVar.getPixels();

  for(int i = 0; i < pixels.length; i++)
    pixels[i] = ((int) (pixels[i] & 0xff) > (int) (mean[i] + k_value * Math.sqrt(var[i]) - c_value)) ? object : backg;
  // imp.updateAndDraw();
  return;
}

void Otsu(const cv::Mat &image, int radius, double par1, double par2, bool doIwhite)
{
  // Otsu's threshold algorithm
  // M. Emre Celebi 6.15.2007, Fourier Library https://sourceforge.net/projects/fourier-ipal/
  // ported to ImageJ plugin by G.Landini. Same algorithm as in Auto_Threshold, this time for local circular regions

  int[] data;
  int w = imp.getWidth();
  int h = imp.getHeight();
  int position;
  int radiusx2       = radius * 2;
  ImageProcessor ip  = imp.getProcessor();
  int8_t[] pixels    = (int8_t[]) ip.getPixels();
  int8_t[] pixelsOut = new int8_t[pixels.length];    // need this to avoid changing the image data (and further histograms)
  int8_t object;
  int8_t backg;

  if(doIwhite) {
    object = (int8_t) 0xff;
    backg  = (int8_t) 0;
  } else {
    object = (int8_t) 0;
    backg  = (int8_t) 0xff;
  }

  int ih, roiy, L = 256;    // L is for 8bit images.
  int threshold;
  int num_pixels;
  double total_mean;            // mean gray-level for the whole image
double bcv, term;               // between-class variance, scaling term
double max_bcv;                 // max BCV
double[] cnh   = new double[L]; // cumulative normalized histogram
double[] mean  = new double[L]; // mean gray-level
double[] histo = new double[L]; // normalized histogram

Roi roi = new OvalRoi(0, 0, radiusx2, radiusx2);
// ip.setRoi(roi);
for(int y = 0; y < h; y++) {
  IJ.showProgress((double) (y) / (h - 1));    // this method is slow, so let's show the progress bar
  roiy = y - radius;
  for(int x = 0; x < w; x++) {
    roi.setLocation(x - radius, roiy);
    ip.setRoi(roi);
    // ip.setRoi(new OvalRoi(x-radius, roiy, radiusx2, radiusx2));
    position = x + y * w;
    data     = ip.getHistogram();

    //----
    // Calculate total numbre of pixels
    num_pixels = 0;

    for(ih = 0; ih < L; ih++)
      num_pixels = num_pixels + data[ih];

    term = 1.0 / (double) num_pixels;

    // Calculate the normalized histogram
    for(ih = 0; ih < L; ih++) {
      histo[ih] = term * data[ih];
    }

    // Calculate the cumulative normalized histogram
    cnh[0] = histo[0];
    for(ih = 1; ih < L; ih++) {
      cnh[ih] = cnh[ih - 1] + histo[ih];
    }

    mean[0] = 0.0;

    for(ih = 0 + 1; ih < L; ih++) {
      mean[ih] = mean[ih - 1] + ih * histo[ih];
    }

    total_mean = mean[L - 1];

    //	Calculate the BCV at each gray-level and find the threshold that maximizes it
    threshold = 0;    // Integer.MIN_VALUE;
    max_bcv   = 0.0;

    for(ih = 0; ih < L; ih++) {
      bcv = total_mean * cnh[ih] - mean[ih];
      bcv *= bcv / (cnh[ih] * (1.0 - cnh[ih]));

      if(max_bcv < bcv) {
        max_bcv   = bcv;
        threshold = ih;
      }
    }
    pixelsOut[position] = ((int) (pixels[position] & 0xff) > threshold || (int) (pixels[position] & 0xff) == 255) ? object : backg;
  }
}
for(position = 0; position < w * h; position++)
  pixels[position] = pixelsOut[position];    // update with thresholded pixels
}

void Phansalkar(const cv::Mat &image, int radius, double par1, double par2, bool doIwhite)
{
  // This is a modification of Sauvola's thresholding method to deal with low contrast images.
  // Phansalskar N. et al. Adaptive local thresholding for detection of nuclei in diversity stained
  // cytology images.International Conference on Communications and Signal Processing (ICCSP), 2011,
  // 218 - 220.
  // In this method, the threshold t = mean*(1+p*exp(-q*mean)+k*((stdev/r)-1))
  // Phansalkar recommends k = 0.25, r = 0.5, p = 2 and q = 10. In this plugin, k and r are the
  // parameters 1 and 2 respectively, but the values of p and q are fixed.
  //
  // Implemented from Phansalkar's paper description by G. Landini
  // This version uses a circular local window, instead of a rectagular one

  ImagePlus Meanimp, Varimp, Orimp;
  ImageProcessor ip = imp.getProcessor(), ipMean, ipVar, ipOri;
  double k_value    = 0.25;
  double r_value    = 0.5;
  double p_value    = 2.0;
  double q_value    = 10.0;
  int8_t object;
  int8_t backg;

  if(par1 != 0) {
    if(IJ.debugMode)
      IJ.log("Phansalkar: changed k_value from :" + k_value + "  to:" + par1);
    k_value = par1;
  }

  if(par2 != 0) {
    if(IJ.debugMode)
      IJ.log("Phansalkar: changed r_value from :" + r_value + "  to:" + par2);
    r_value = par2;
  }

  if(doIwhite) {
    object = (int8_t) 0xff;
    backg  = (int8_t) 0;
  } else {
    object = (int8_t) 0;
    backg  = (int8_t) 0xff;
  }

  Meanimp             = duplicateImage(ip);
  ContrastEnhancer ce = new ContrastEnhancer();
  ce.setNormalize(true);    // Needs to be true for correct normalization
  ce.stretchHistogram(Meanimp, 0.0);
  ImageConverter ic = new ImageConverter(Meanimp);
  ic.convertToGray32();
  ipMean = Meanimp.getProcessor();
  ipMean.multiply(1.0 / 255);

  Orimp = duplicateImage(ip);
  ce.stretchHistogram(Orimp, 0.0);
  ic = new ImageConverter(Orimp);
  ic.convertToGray32();
  ipOri = Orimp.getProcessor();
  ipOri.multiply(1.0 / 255);    // original to compare
  // Orimp.show();

  RankFilters rf = new RankFilters();
  rf.rank(ipMean, radius, rf.MEAN);    // Mean

  // Meanimp.show();
  Varimp = duplicateImage(ip);
  ce.stretchHistogram(Varimp, 0.0);
  ic = new ImageConverter(Varimp);
  ic.convertToGray32();
  ipVar = Varimp.getProcessor();
  ipVar.multiply(1.0 / 255);

  rf.rank(ipVar, radius, rf.VARIANCE);    // Variance
  ipVar.sqrt();                           // SD

  // Varimp.show();
  int8_t[] pixels = (int8_t[]) ip.getPixels();
  float[] ori     = (float[]) ipOri.getPixels();
  float[] mean    = (float[]) ipMean.getPixels();
  float[] sd      = (float[]) ipVar.getPixels();

  for(int i = 0; i < pixels.length; i++)
    pixels[i] = ((ori[i]) > (mean[i] * (1.0 + p_value * Math.exp(-q_value * mean[i]) + k_value * ((sd[i] / r_value) - 1.0)))) ? object : backg;
  // imp.updateAndDraw();
  return;
}

void Sauvola(const cv::Mat &image, int radius, double par1, double par2, bool doIwhite)
{
  // Sauvola recommends K_VALUE = 0.5 and R_VALUE = 128.
  // This is a modification of Niblack's thresholding method.
  // Sauvola J. and Pietaksinen M. (2000) "Adaptive Document Image Binarization"
  // Pattern Recognition, 33(2): 225-236
  // http://www.ee.oulu.fi/mvg/publications/show_pdf.php?ID=24
  // Ported to ImageJ plugin from E Celebi's fourier_0.8 routines
  // This version uses a circular local window, instead of a rectagular one

  ImagePlus Meanimp, Varimp;
  ImageProcessor ip = imp.getProcessor(), ipMean, ipVar;
  double k_value    = 0.5;
  double r_value    = 128;
  int8_t object;
  int8_t backg;

  if(par1 != 0) {
    if(IJ.debugMode)
      IJ.log("Sauvola: changed k_value from :" + k_value + "  to:" + par1);
    k_value = par1;
  }

  if(par2 != 0) {
    if(IJ.debugMode)
      IJ.log("Sauvola: changed r_value from :" + r_value + "  to:" + par2);
    r_value = par2;
  }

  if(doIwhite) {
    object = (int8_t) 0xff;
    backg  = (int8_t) 0;
  } else {
    object = (int8_t) 0;
    backg  = (int8_t) 0xff;
  }

  Meanimp           = duplicateImage(ip);
  ImageConverter ic = new ImageConverter(Meanimp);
  ic.convertToGray32();

  ipMean         = Meanimp.getProcessor();
  RankFilters rf = new RankFilters();
  rf.rank(ipMean, radius, rf.MEAN);    // Mean
  // Meanimp.show();
  Varimp = duplicateImage(ip);
  ic     = new ImageConverter(Varimp);
  ic.convertToGray32();
  ipVar = Varimp.getProcessor();
  rf.rank(ipVar, radius, rf.VARIANCE);    // Variance
  // Varimp.show();
  int8_t[] pixels = (int8_t[]) ip.getPixels();
  float[] mean    = (float[]) ipMean.getPixels();
  float[] var     = (float[]) ipVar.getPixels();

  for(int i = 0; i < pixels.length; i++)
    pixels[i] = ((int) (pixels[i] & 0xff) > (int) (mean[i] * (1.0 + k_value * ((Math.sqrt(var[i]) / r_value) - 1.0)))) ? object : backg;
  // imp.updateAndDraw();
  return;
}
*/

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ThresholdAdaptive::autoThresholdAdaptive(const settings::ThresholdAdaptiveSettings::ThresholdAdaptive &settings, const cv::Mat &imp,
                                              cv::Mat &outImage)
{
  outImage = imp.clone();
  switch(settings.method) {
    case settings::ThresholdAdaptiveSettings::Methods::BERNSEN:
      Bernsen(outImage, settings.kernelSize, settings.contrastThreshold, 0, settings.thresholdOffset, true);
      break;
    case settings::ThresholdAdaptiveSettings::Methods::CONTRAST:
      Contrast(outImage, settings.kernelSize, settings.contrastThreshold, 0, settings.thresholdOffset, true);
      break;
    case settings::ThresholdAdaptiveSettings::Methods::MEAN:
      Mean(outImage, settings.kernelSize, settings.contrastThreshold, 0, settings.thresholdOffset, true);
      break;
    case settings::ThresholdAdaptiveSettings::Methods::MEDIAN:
      Median(outImage, settings.kernelSize, settings.contrastThreshold, 0, settings.thresholdOffset, true);
      break;
    case settings::ThresholdAdaptiveSettings::Methods::OTSU:
      Otsu(outImage, settings.kernelSize, settings.contrastThreshold, 0, settings.thresholdOffset, true);
      break;
    case settings::ThresholdAdaptiveSettings::Methods::MID_GRAY:
    case settings::ThresholdAdaptiveSettings::Methods::NIBLACK:
    case settings::ThresholdAdaptiveSettings::Methods::PHANSALKAR:
    case settings::ThresholdAdaptiveSettings::Methods::SAUVOLA:
      break;
  }
}

}    // namespace joda::cmd
