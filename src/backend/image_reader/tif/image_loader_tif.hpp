

#pragma once

#include <climits>
#include <iostream>
#include <string>
#include "../../helper/ome_parser/ome_info.hpp"
#include "../image_reader.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>

#define USER_DEFINED_INT64
#include <tiffio.h>

using TIFF_8BIT_GRAYSCALE  = uint8_t;
using TIFF_16BIT_GRAYSCALE = uint16_t;
using TIFF_FLOAT_GRAYSCALE = float;

template <typename T>
concept tiffformat = std::is_same<T, TIFF_8BIT_GRAYSCALE>::value || std::is_same<T, TIFF_16BIT_GRAYSCALE>::value ||
                     /*std::is_same<T, TIFF_32BIT>::value ||*/ std::is_same<T, TIFF_FLOAT_GRAYSCALE>::value;

class TiffLoader
{
public:
  static void initLibTif();
  static cv::Mat loadImageTile(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead);
  static cv::Mat loadEntireImage(const std::string &filename, int directory);
  static auto getOmeInformation(const std::string &filename) -> joda::ome::OmeInfo;
  static auto getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties;

private:
  /////////////////////////////////////////////////////

  ///
  /// \brief      Reads a grayscale TIFF image and scales it to a 16bit
  ///             grayscale cv::Mat object
  /// \author     Joachim Danmayr
  /// \param[in]  tiff   Tiff image to read from
  /// \param[in]  width  Width of the image
  /// \param[in]  height Height of the image
  /// \return     cv::Mat image in Format CV_16UC1
  ///
  template <tiffformat T>
  static cv::Mat ReadGrayScaleImage(TIFF *tiff, unsigned int width, unsigned int height)
  {
    cv::Mat image      = cv::Mat(width, height, CV_16UC1, 0.0);
    tsize_t scanlength = TIFFScanlineSize(tiff);

    // We scale everything to 16 bit
    float scalingFactor = (USHRT_MAX / (std::pow(2, 8 * sizeof(T)) - 1));

    // Read image data allocating space for each line as we get it
    for(unsigned int y = 0; y < height; y++) {
      T *raster = (T *) malloc(scanlength);
      TIFFReadScanline(tiff, raster, y);
      for(unsigned int x = 0; x < scanlength / sizeof(T); x++) {
        unsigned short &pixel = image.at<unsigned short>(cv::Point(x, y));    // Read the current pixel of the matrix
        pixel                 = static_cast<unsigned short>((static_cast<float>(raster[x]) * scalingFactor));
      }
      _TIFFfree(raster);    // Free temp memory
    }
    return image;
  }

  ///
  /// \brief      Reads a grayscale TIFF image tile and scales ot to a 16bit
  ///             grayscale cv::Mat object
  /// \author     Joachim Danmayr
  /// \param[in]  tiff   Tiff image to read from
  /// \param[in]  width  Width of the image
  /// \param[in]  height Height of the image
  /// \return     cv::Mat image in Format CV_16UC1
  ///
  template <tiffformat T>
  static void ReadGrayScaleImageTile(TIFF *tiff, unsigned int tileX, unsigned int tileY, unsigned int tileWidth,
                                     unsigned int tileHeight, cv::Mat &image, unsigned int tilePartX,
                                     unsigned int tilePartY, unsigned int tileNrY, unsigned int tileOffsetY)
  {
    // We scale everything to 16 bit
    float scalingFactor = (USHRT_MAX / (std::pow(2, 8 * sizeof(T)) - 1));

    auto tileSize = TIFFTileSize(tiff);
    // Read image data allocating space for each line as we get it
    T *raster = (T *) malloc(tileSize);
    TIFFReadTile(tiff, raster, tileX, tileY, 0, 0);

    //
    // Now it gets tricky: We have to correctly place the read pixel values from the original image
    // in the openCV image matrix. This is tricky since an openCV image Matrix has its origin (0,0) at the
    // button left, a TIFF image has it's origin at the top left. Next tricky thing is that images are
    // rotated counterclockwise and flipped.
    // We therefore have to write the tiles from the right to left, but the pixels of a tile must written from left
    // to right.
    //
    for(uint y = 0; y < tileHeight; y++) {
      for(uint x = 0; x < tileWidth; x++) {
        int xImg              = (x + tilePartX);
        int yImg              = (y + tilePartY);
        unsigned short &pixel = image.at<unsigned short>(
            cv::Point(xImg, yImg));    // Get the reference of the pixel to write of the opencv matrix
        // Read the current pixel of the TIF (see:
        // http://www.simplesystems.org/libtiff//functions/TIFFReadRGBATile.html)) and scale it
        pixel = static_cast<unsigned short>(static_cast<float>(raster[y * tileWidth + x] * scalingFactor));
      }
    }

    _TIFFfree(raster);    // Free temp memory
  }
};
