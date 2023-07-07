

#pragma once

#include <string>
#include "../image_reader.hpp"
#include "helper/ome_parser/ome_info.hpp"
#include <opencv2/core/mat.hpp>

#define USER_DEFINED_INT64
#include <libtiff/tiffio.h>

class TiffLoader
{
public:
  static void initLibTif();
  static cv::Mat loadImageTile(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead);
  static auto getOmeInformation(const std::string &filename) -> joda::ome::OmeInfo;
  static auto getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties;
  static cv::Mat loadEntireImage(const std::string &filename, int directory);

private:
  template <typename VEC>
  static void convertFromLibTiffToOpenCvMat(cv::Mat &image, unsigned int width, unsigned int height, uint32 *raster)
  {
    for(uint x = 0; x < width; x++) {
      for(uint y = 0; y < height; y++) {
        uint32_t &TiffPixel = raster[y * width + x];             // Read the current pixel of the TIF
        VEC &pixel          = image.at<VEC>(cv::Point(y, x));    // Read the current pixel of the matrix
        pixel[0]            = TIFFGetB(TiffPixel);               // Set the pixel values as BGRA
        pixel[1]            = TIFFGetG(TiffPixel);
        pixel[2]            = TIFFGetR(TiffPixel);
        // pixel[3]          = TIFFGetA(TiffPixel);    // No alpha channel
      }
    }
  }

  void TIFFRead16bitRGB();
};
