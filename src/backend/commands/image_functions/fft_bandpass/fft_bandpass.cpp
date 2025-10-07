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
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast.hpp"
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
void FFTBandpass::execute(processor::ProcessContext & /*context*/, cv::Mat &image, atom::ObjectList & /*result*/)
{
  filter(image);
}

/**
 * Puts imageprocessor (ROI) into a new imageprocessor of size width x height y
 * at position (x,y).
 * The image is mirrored around its edges to avoid wrap around effects of the
 * FFT.
 */
void insert(cv::Mat &dst, const cv::Mat &src, int xpos, int ypos)
{
  cv::Rect roi(xpos, ypos, src.cols, src.rows);
  cv::Rect dstRect(0, 0, dst.cols, dst.rows);
  cv::Rect validRoi = roi & dstRect;

  if(validRoi.width > 0 && validRoi.height > 0) {
    cv::Rect srcRoi(validRoi.x - roi.x, validRoi.y - roi.y, validRoi.width, validRoi.height);
    src(srcRoi).copyTo(dst(validRoi));
  }
}

cv::Mat tileMirror(const cv::Mat &ip, int width, int height, int x, int y)
{
  if(x < 0 || x > (width - 1) || y < 0 || y > (height - 1)) {
    joda::log::logError("Image to be tiled is out of bounds.");
    return {};
  }

  cv::Mat ipout = cv::Mat::zeros(cv::Size{width, height}, ip.type());

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
      insert(ipout, ip2, x - i * w2, y - j * h2);
    }
  }

  // Flip horizontal
  cv::flip(ip2, ip2, 1);
  for(int i = -i1 + 1; i < i2; i += 2) {
    for(int j = -j1; j < j2; j += 2) {
      // ipout.insert(ip2, x - i * w2, y - j * h2);
      insert(ipout, ip2, x - i * w2, y - j * h2);
    }
  }

  // Flip vertical
  cv::flip(ip2, ip2, 0);
  for(int i = -i1 + 1; i < i2; i += 2) {
    for(int j = -j1 + 1; j < j2; j += 2) {
      // ipout.insert(ip2, x - i * w2, y - j * h2);
      insert(ipout, ip2, x - i * w2, y - j * h2);
    }
  }

  // Flip horizontal
  cv::flip(ip2, ip2, 1);
  for(int i = -i1; i < i2; i += 2) {
    for(int j = -j1 + 1; j < j2; j += 2) {
      //   ipout.insert(ip2, x - i * w2, y - j * h2);
      insert(ipout, ip2, x - i * w2, y - j * h2);
    }
  }

  return ipout;
}

/*
 * filterLarge: down to which size are large structures suppressed?
 * filterSmall: up to which size are small structures suppressed?
 * filterLarge and filterSmall are given as fraction of the image size
 * in the original (untransformed) image.
 * stripesHorVert: filter out: 0) nothing more 1) horizontal 2) vertical stripes
 * (i.e. frequencies with x=0 / y=0)
 * scaleStripes: width of the stripe filter, same unit as filterLarge
 */
void filterLargeSmall(cv::Mat &ip, double filterLarge, double filterSmall, settings::FFTBandpassSettings::StripeMode stripesHorVert,
                      double scaleStripes)
{
  int maxN = ip.cols;

  //  float* fht   = ip.data;
  float *filter = new float[maxN * maxN];
  for(int i = 0; i < maxN * maxN; i++) {
    filter[i] = 1.0F;
  }

  int row;
  int backrow;
  float rowFactLarge;
  float rowFactSmall;

  int col;
  int backcol;
  float factor;
  float colFactLarge;
  float colFactSmall;

  float factStripes;

  // calculate factor in exponent of Gaussian from filterLarge / filterSmall

  double scaleLarge = filterLarge * filterLarge;
  double scaleSmall = filterSmall * filterSmall;
  scaleStripes      = scaleStripes * scaleStripes;
  // float FactStripes;

  // loop over rows
  for(int j = 1; j < maxN / 2; j++) {
    row          = j * maxN;
    backrow      = (maxN - j) * maxN;
    rowFactLarge = static_cast<float>(std::exp(-(j * j) * scaleLarge));
    rowFactSmall = static_cast<float>(std::exp(-(j * j) * scaleSmall));

    // loop over columns
    for(col = 1; col < maxN / 2; col++) {
      backcol      = maxN - col;
      colFactLarge = static_cast<float>(std::exp(-(col * col) * scaleLarge));
      colFactSmall = static_cast<float>(std::exp(-(col * col) * scaleSmall));
      factor       = (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall;
      switch(stripesHorVert) {
        case settings::FFTBandpassSettings::StripeMode::HORIZONTAL:
          factor *= (1 - static_cast<float>(std::exp(-(col * col) * scaleStripes)));
          break;    // hor stripes
        case settings::FFTBandpassSettings::StripeMode::VERTICAL:
          factor *= (1 - static_cast<float>(std::exp(-(j * j) * scaleStripes)));    // vert stripes
          break;
        default:
          break;
      }

      ip.at<float>(col + row) *= factor;
      ip.at<float>(col + backrow) *= factor;
      ip.at<float>(backcol + row) *= factor;
      ip.at<float>(backcol + backrow) *= factor;
      filter[col + row] *= factor;
      filter[col + backrow] *= factor;
      filter[backcol + row] *= factor;
      filter[backcol + backrow] *= factor;
    }
  }

  // process meeting points (maxN/2,0) , (0,maxN/2), and (maxN/2,maxN/2)
  int rowmid   = maxN * (maxN / 2);
  rowFactLarge = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleLarge));
  rowFactSmall = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleSmall));
  factStripes  = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleStripes));

  ip.at<float>(maxN / 2) *= (1 - rowFactLarge) * rowFactSmall;                                           // (maxN/2,0)
  ip.at<float>(rowmid) *= (1 - rowFactLarge) * rowFactSmall;                                             // (0,maxN/2)
  ip.at<float>(maxN / 2 + rowmid) *= (1 - rowFactLarge * rowFactLarge) * rowFactSmall * rowFactSmall;    // (maxN/2,maxN/2)
  filter[maxN / 2] *= (1 - rowFactLarge) * rowFactSmall;                                                 // (maxN/2,0)
  filter[rowmid] *= (1 - rowFactLarge) * rowFactSmall;                                                   // (0,maxN/2)
  filter[maxN / 2 + rowmid] *= (1 - rowFactLarge * rowFactLarge) * rowFactSmall * rowFactSmall;          // (maxN/2,maxN/2)

  switch(stripesHorVert) {
    case settings::FFTBandpassSettings::StripeMode::HORIZONTAL:
      ip.at<float>(maxN / 2) *= (1 - factStripes);
      ip.at<float>(rowmid) = 0;
      ip.at<float>(maxN / 2 + rowmid) *= (1 - factStripes);
      filter[maxN / 2] *= (1 - factStripes);
      filter[rowmid] = 0;
      filter[maxN / 2 + rowmid] *= (1 - factStripes);
      break;    // hor stripes
    case settings::FFTBandpassSettings::StripeMode::VERTICAL:
      ip.at<float>(maxN / 2) = 0;
      ip.at<float>(rowmid) *= (1 - factStripes);
      ip.at<float>(maxN / 2 + rowmid) *= (1 - factStripes);
      filter[maxN / 2] = 0;
      filter[rowmid] *= (1 - factStripes);
      filter[maxN / 2 + rowmid] *= (1 - factStripes);
      break;    // vert stripes
  }

  // loop along row 0 and maxN/2
  rowFactLarge = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleLarge));
  rowFactSmall = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleSmall));
  for(col = 1; col < maxN / 2; col++) {
    backcol      = maxN - col;
    colFactLarge = static_cast<float>(std::exp(-(col * col) * scaleLarge));
    colFactSmall = static_cast<float>(std::exp(-(col * col) * scaleSmall));

    switch(stripesHorVert) {
      case settings::FFTBandpassSettings::StripeMode::NOTHING:
        ip.at<float>(col) *= (1 - colFactLarge) * colFactSmall;
        ip.at<float>(backcol) *= (1 - colFactLarge) * colFactSmall;
        ip.at<float>(col + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall;
        ip.at<float>(backcol + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall;
        filter[col] *= (1 - colFactLarge) * colFactSmall;
        filter[backcol] *= (1 - colFactLarge) * colFactSmall;
        filter[col + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall;
        filter[backcol + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall;
        break;
      case settings::FFTBandpassSettings::StripeMode::HORIZONTAL:
        factStripes = static_cast<float>(std::exp(-(col * col) * scaleStripes));
        ip.at<float>(col) *= (1 - colFactLarge) * colFactSmall * (1 - factStripes);
        ip.at<float>(backcol) *= (1 - colFactLarge) * colFactSmall * (1 - factStripes);
        ip.at<float>(col + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        ip.at<float>(backcol + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        filter[col] *= (1 - colFactLarge) * colFactSmall * (1 - factStripes);
        filter[backcol] *= (1 - colFactLarge) * colFactSmall * (1 - factStripes);
        filter[col + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        filter[backcol + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        break;
      case settings::FFTBandpassSettings::StripeMode::VERTICAL:
        factStripes           = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleStripes));
        ip.at<float>(col)     = 0;
        ip.at<float>(backcol) = 0;
        ip.at<float>(col + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        ip.at<float>(backcol + rowmid) *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        filter[col]     = 0;
        filter[backcol] = 0;
        filter[col + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
        filter[backcol + rowmid] *= (1 - colFactLarge * rowFactLarge) * colFactSmall * rowFactSmall * (1 - factStripes);
    }
  }

  // loop along column 0 and maxN/2
  colFactLarge = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleLarge));
  colFactSmall = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleSmall));
  for(int j = 1; j < maxN / 2; j++) {
    row          = j * maxN;
    backrow      = (maxN - j) * maxN;
    rowFactLarge = static_cast<float>(std::exp(-(j * j) * scaleLarge));
    rowFactSmall = static_cast<float>(std::exp(-(j * j) * scaleSmall));

    switch(stripesHorVert) {
      case settings::FFTBandpassSettings::StripeMode::NOTHING:
        ip.at<float>(row) *= (1 - rowFactLarge) * rowFactSmall;
        ip.at<float>(backrow) *= (1 - rowFactLarge) * rowFactSmall;
        ip.at<float>(row + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall;
        ip.at<float>(backrow + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall;
        filter[row] *= (1 - rowFactLarge) * rowFactSmall;
        filter[backrow] *= (1 - rowFactLarge) * rowFactSmall;
        filter[row + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall;
        filter[backrow + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall;
        break;
      case settings::FFTBandpassSettings::StripeMode::HORIZONTAL:
        factStripes           = static_cast<float>(std::exp(-(maxN / 2) * (maxN / 2) * scaleStripes));
        ip.at<float>(row)     = 0;
        ip.at<float>(backrow) = 0;
        ip.at<float>(row + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        ip.at<float>(backrow + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        filter[row]     = 0;
        filter[backrow] = 0;
        filter[row + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        filter[backrow + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        break;
      case settings::FFTBandpassSettings::StripeMode::VERTICAL:
        factStripes = static_cast<float>(std::exp(-(j * j) * scaleStripes));
        ip.at<float>(row) *= (1 - rowFactLarge) * rowFactSmall * (1 - factStripes);
        ip.at<float>(backrow) *= (1 - rowFactLarge) * rowFactSmall * (1 - factStripes);
        ip.at<float>(row + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        ip.at<float>(backrow + maxN / 2) *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        filter[row] *= (1 - rowFactLarge) * rowFactSmall * (1 - factStripes);
        filter[backrow] *= (1 - rowFactLarge) * rowFactSmall * (1 - factStripes);
        filter[row + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
        filter[backrow + maxN / 2] *= (1 - rowFactLarge * colFactLarge) * rowFactSmall * colFactSmall * (1 - factStripes);
    }
  }

  delete[] filter;
}

// Forward Hartley Transform
cv::Mat hartleyTransform(const cv::Mat &src)
{
  CV_Assert(src.type() == CV_32F);    // must be float

  cv::Mat planes[] = {src.clone(), cv::Mat::zeros(src.size(), CV_32F)};
  cv::Mat complexImg;
  cv::merge(planes, 2, complexImg);

  cv::dft(complexImg, complexImg);

  cv::split(complexImg, planes);

  // Hartley transform: H = Re - Im
  cv::Mat H = planes[0] - planes[1];
  return H;
}

// Inverse Hartley Transform
cv::Mat inverseHartleyTransform(const cv::Mat &H)
{
  CV_Assert(H.type() == CV_32F);

  // Forward Hartley again, scaled by 1/N
  cv::Mat invH = hartleyTransform(H);
  invH /= static_cast<double>(H.total());
  return invH;
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
  double filterLarge = 2.0 * static_cast<double>(mSettings.filterLargeStructure) / static_cast<double>(i);
  double filterSmall = 2.0 * static_cast<double>(mSettings.filterLargeStructure) / static_cast<double>(i);

  // fit image into power of 2 size
  cv::Rect fitRect;
  fitRect.x      = static_cast<int>(std::round((i - ip.cols) / 2.0));
  fitRect.y      = static_cast<int>(std::round((i - ip.rows) / 2.0));
  fitRect.width  = ip.cols;
  fitRect.height = ip.rows;

  // put image (ROI) into power 2 size image
  // mirroring to avoid wrap around effects
  // showStatus("Pad to " + i + "x" + i);

  ip = tileMirror(ip, i, i, fitRect.x, fitRect.y);

  ip.convertTo(ip, CV_32F);
  cv::Mat fht = hartleyTransform(ip);

  // filter out large and small structures
  filterLargeSmall(fht, filterLarge, filterSmall, mSettings.stripesHorVert, sharpness);
  // new ImagePlus("filter",ip2.crop()).show();

  // transform backward
  fht = inverseHartleyTransform(fht);

  // crop to original size and do scaling if selected
  cv::Mat ip2 = fht(fitRect).clone();    // clone() = make independent copy

  // Optional: clip negatives / overshoot
  cv::threshold(ip2, ip2, 0, 0, cv::THRESH_TOZERO);           // clamp <0 to 0
  cv::threshold(ip2, ip2, 65535, 65535, cv::THRESH_TRUNC);    // clamp >65535

  ip2.convertTo(ip, CV_16UC1);    // back to 16-bit

  cv::imwrite("/workspaces/imagec/tmp/ip2.png", ip2 / 20);
  cv::imwrite("/workspaces/imagec/tmp/ip.png", ip * 10);

  if(doScaling) {
    int histSize           = UINT16_MAX + 1;
    float range[]          = {0, UINT16_MAX + 1};
    const float *histRange = {range};
    cv::Mat hist;
    cv::calcHist(&ip, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);    //, uniform, accumulate);
    joda::cmd::EnhanceContrast::stretchHistogram(ip, saturate ? 1.0 : 0.0, hist, true);
  }
}

}    // namespace joda::cmd
