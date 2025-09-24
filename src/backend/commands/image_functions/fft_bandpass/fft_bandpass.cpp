///
/// \file      fft_bandpass.cpp
/// \author    Joachim Danmayr
/// \date      2025-09-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "fft_bandpass.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <opencv2/core/types.hpp>

namespace joda::cmd {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
FFTBandpass::FFTBandpass(const settings::FFTBandpassSettings &settings) : mSettings(settings)
{
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void FFTBandpass::execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result)
{
}

/**
 * Puts imageprocessor (ROI) into a new imageprocessor of size width x height y
 * at position (x,y).
 * The image is mirrored around its edges to avoid wrap around effects of the
 * FFT.
 */
cv::Mat tileMirror(cv::Mat &ip, int width, int height, int x, int y)
{
  if(x < 0 || x > (width - 1) || y < 0 || y > (height - 1)) {
    joda::log::logError("Image to be tiled is out of bounds.");
    return {};
  }

  cv::Mat ipout = cv::Mat::zeros(cv::Size{width, height}, CV_16UC1);

  auto ip2 = ip.clone();
  int w2   = ip2.cols;
  int h2   = ip2.rows;

  // how many times does ip2 fit into ipout?
  int i1 = static_cast<int>(std::ceil(x / static_cast<double>(w2)));
  int i2 = static_cast<int>(std::ceil((width - x) / static_cast<double>(w2)));
  int j1 = static_cast<int>(std::ceil(y / static_cast<double>(h2)));
  int j2 = static_cast<int>(std::ceil((height - y) / static_cast<double>(h2)));

  // tile
  if((i1 % 2) > 0.5) {
    // Flip horizontal
    cv::flip(ip2, ip2, 1);
  }
  if((j1 % 2) > 0.5) {
    // Flip vertical
    cv::flip(ip2, ip2, 0);
  }

  for(int i = -i1; i < i2; i += 2) {
    for(int j = -j1; j < j2; j += 2) {
      // ipout.insert(ip2, x - i * w2, y - j * h2);
      ip2.copyTo(ipout(cv::Rect(x - i * w2, y - j * h2, ip2.cols, ip2.rows)));
    }
  }

  // Flip horizontal
  cv::flip(ip2, ip2, 1);
  for(int i = -i1 + 1; i < i2; i += 2) {
    for(int j = -j1; j < j2; j += 2) {
      // ipout.insert(ip2, x - i * w2, y - j * h2);
      ip2.copyTo(ipout(cv::Rect(x - i * w2, y - j * h2, ip2.cols, ip2.rows)));
    }
  }

  // Flip vertical
  cv::flip(ip2, ip2, 0);
  for(int i = -i1 + 1; i < i2; i += 2) {
    for(int j = -j1 + 1; j < j2; j += 2) {
      // ipout.insert(ip2, x - i * w2, y - j * h2);
      ip2.copyTo(ipout(cv::Rect(x - i * w2, y - j * h2, ip2.cols, ip2.rows)));
    }
  }

  // Flip horizontal
  cv::flip(ip2, ip2, 1);
  for(int i = -i1; i < i2; i += 2) {
    for(int j = -j1 + 1; j < j2; j += 2) {
      //   ipout.insert(ip2, x - i * w2, y - j * h2);
      ip2.copyTo(ipout(cv::Rect(x - i * w2, y - j * h2, ip2.cols, ip2.rows)));
    }
  }

  return ipout;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void FFTBandpass::filter(cv::Mat &ip)
{
  int maxN         = std::max(ip.cols, ip.rows);
  double sharpness = (1.0 - static_cast<double>(mSettings.toleranceOfDirection));
  bool doScaling   = mSettings.doScaling;
  bool saturate    = mSettings.doSaturation;

  /*
   * tile mirrored image to power of 2 size
   * first determine smallest power 2 >= 1.5 * image width/height
   * factor of 1.5 to avoid wrap-around effects of Fourier Trafo
   */
  int i = 2;
  while(i < 1.5 * maxN) {
    i *= 2;
  }

  // Calculate the inverse of the 1/e frequencies for large and small structures.
  double filterLarge = 2.0 * static_cast<double>(mSettings.filterLargeStructure) / (double) i;
  double filterSmall = 2.0 * static_cast<double>(mSettings.filterLargeStructure) / (double) i;

  // fit image into power of 2 size
  cv::Rect fitRect;
  fitRect.x      = static_cast<int>(std::round((i - ip.cols) / 2.0));
  fitRect.y      = static_cast<int>(std::round((i - ip.rows) / 2.0));
  fitRect.width  = ip.cols;
  fitRect.height = ip.rows;

  // put image (ROI) into power 2 size image
  // mirroring to avoid wrap around effects
  // showStatus("Pad to " + i + "x" + i);

  tileMirror(ip, i, i, fitRect.x, fitRect.y);

  // transform forward
  // Do a fast harley transformation
  {
    // FHT fht = new FHT(ip2);
    // fht.transform();

    cv::Mat floatImg;
    ip.convertTo(floatImg, CV_32F);    // formal ip2

    // forward FFT
    cv::Mat planes[] = {floatImg.clone(), cv::Mat::zeros(floatImg.size(), CV_32F)};
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg);

    // split back into real + imag
    cv::split(complexImg, planes);
    cv::Mat fht = planes[0] - planes[1];    // Hartley transform
  }

  // filter out large and small structures
  filterLargeSmall(fht, filterLarge, filterSmall, choiceIndex, sharpness);
  // new ImagePlus("filter",ip2.crop()).show();

  // transform backward
  fht.inverseTransform();

  // crop to original size and do scaling if selected
  fht.setRoi(fitRect);
  ip2 = fht.crop();
  if(doScaling) {
    ImagePlus imp2 = new ImagePlus(imp.getTitle() + "-filtered", ip2);
    new ContrastEnhancer().stretchHistogram(imp2, saturate ? 1.0 : 0.0);
    ip2 = imp2.getProcessor();
  }

  // convert back to original data type
  int bitDepth = imp.getBitDepth();
  switch(bitDepth) {
    case 8:
      ip2 = ip2.convertToByte(doScaling);
      break;
    case 16:
      ip2 = ip2.convertToShort(doScaling);
      break;
    case 24:
      ip.snapshot();
      showStatus("Setting brightness");
      ((ColorProcessor) ip).setBrightness((FloatProcessor) ip2);
      break;
    case 32:
      break;
  }

  // copy filtered image back into original image
  if(bitDepth != 24) {
    ip.snapshot();
    ip.copyBits(ip2, roiRect.x, roiRect.y, Blitter.COPY);
  }
  ip.resetMinAndMax();
  System.gc();
  IJ.showProgress(20, 20);
}

}    // namespace joda::cmd
