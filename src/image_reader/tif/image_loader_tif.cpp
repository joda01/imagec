

#include "image_loader_tif.hpp"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#define USER_DEFINED_INT64
#include <libtiff/tiffio.h>
#include <libtiff/tiffiop.h>

// http://www.simplesystems.org/libtiff//functions/TIFFCreateDirectory.html

cv::Mat TiffLoader::loadImageTile(const std::string &filename, int idx, int offset)
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    unsigned int width, height, tilewidth, tileheight, tileOffset, tileByteCount;
    uint32 *raster;
    uint16_t cnt1 = 0;

    cnt1 = TIFFNumberOfDirectories(tif);
    TIFFSetDirectory(tif, idx);

    // get the size of the tiff
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileheight);
    TIFFGetField(tif, TIFFTAG_TILEOFFSETS, &tileOffset);
    TIFFGetField(tif, TIFFTAG_TILEBYTECOUNTS, &tileByteCount);

    std::cout << " WID " << std::to_string(width) << std::endl;
    std::cout << " HIGH " << std::to_string(height) << std::endl;

    std::cout << "TIL WID " << std::to_string(tilewidth) << std::endl;
    std::cout << "TIL HIGH " << std::to_string(tileheight) << std::endl;
    std::cout << "CNT " << std::to_string(cnt1) << std::endl;
    std::cout << "TIL " << std::to_string(tileOffset) << std::endl;
    std::cout << "BY " << std::to_string(tileByteCount) << std::endl;

    if(tileheight <= 0) {
      tileheight = tilewidth;
    }

    uint npixels = tilewidth * tileheight;    // get the total number of pixels

    int tilePart = offset * tilewidth;
    if(tilePart > width) {
      throw std::runtime_error("Offset error");
    }
    // for(tilePart = 0; tilePart < width; tilePart += tilewidth)

    raster =
        (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));    // allocate temp memory (must use the tiff library malloc)
    if(raster == NULL)                                       // check the raster's memory was allocaed
    {
      TIFFClose(tif);
      throw std::runtime_error("Could not allocate memory for raster of TIFF image.");
    }

    if(!TIFFReadRGBATile(tif, tilePart, tilePart, raster)) {
      TIFFClose(tif);
      throw std::runtime_error("Could not read raster of TIFF image.");
    }

    // Itterate through all the pixels of the tif
    cv::Mat image = cv::Mat(tilewidth, tileheight, CV_8UC3);

    for(uint x = 0; x < tilewidth; x++)
      for(uint y = 0; y < tileheight; y++) {
        uint32 &TiffPixel = raster[y * tilewidth + x];               // read the current pixel of the TIF
        cv::Vec3b &pixel  = image.at<cv::Vec3b>(cv::Point(y, x));    // read the current pixel of the matrix
        pixel[0]          = TIFFGetB(TiffPixel);                     // Set the pixel values as BGRA
        pixel[1]          = TIFFGetG(TiffPixel);
        pixel[2]          = TIFFGetR(TiffPixel);
        // pixel[3]          = TIFFGetA(TiffPixel);
      }
    _TIFFfree(raster);    // release temp memory
    // Rotate the image 90 degrees couter clockwise
    image = image.t();
    cv::flip(image, image, 0);

    cv::Mat imageOut = cv::Mat(tilewidth, tileheight, CV_32FC3);
    image.convertTo(imageOut, CV_32FC3);

    TIFFClose(tif);
    return imageOut;
  } else {
    throw std::runtime_error("Could not open image!");
  }
}

///
/// \brief      Load an entire tif image to the RAM and converts it
///             to an opencv Mat Matric object
/// \author     Joachim Danmayr
/// \param[in]  filename  Name of the file to load
/// \param[in]  directory If the image contains multiple imaged,
///                       this is the index of image to load
/// \return Loaded image
///
cv::Mat TiffLoader::loadEntireImage(const std::string &filename, int directory)
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    unsigned int width = 0, height = 0, directoryNr = 0;

    // Get the size of the tiff
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    // Access wanted directory
    directoryNr = TIFFNumberOfDirectories(tif);
    if(directory >= directoryNr) {
      throw std::runtime_error("Directory index too large! Image has only " + std::to_string(directoryNr) +
                               " directrories.");
    }
    TIFFSetDirectory(tif, directory);

    // Allocate temp memory (must use the tiff library malloc)
    uint npixels   = width * height;    // Total number of pixels
    uint32 *raster = (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));

    // Read image
    if(!TIFFReadRGBAImage(tif, width, height, raster, 0)) {
      TIFFClose(tif);
      throw std::runtime_error("Could not read raster of TIFF image");
    }

    // Create a new matrix of w x h with 8 bits per channel and 4 channels (RGBA)
    // and itterate through all the pixels of the tif and assign to the opencv matrix
    cv::Mat image = cv::Mat(width, height, CV_8UC4);

    for(uint x = 0; x < width; x++)
      for(uint y = 0; y < height; y++) {
        uint32 &TiffPixel = raster[y * width + x];                   // Read the current pixel of the TIF
        cv::Vec4b &pixel  = image.at<cv::Vec4b>(cv::Point(y, x));    // Read the current pixel of the matrix
        pixel[0]          = TIFFGetB(TiffPixel);                     // Set the pixel values as BGRA
        pixel[1]          = TIFFGetG(TiffPixel);
        pixel[2]          = TIFFGetR(TiffPixel);
        pixel[3]          = TIFFGetA(TiffPixel);
      }

    _TIFFfree(raster);    // Free temp memory

    // Rotate the image 90 degrees couter clockwise
    image = image.t();
    cv::flip(image, image, 0);
    return image;
  } else {
    throw std::runtime_error("Could not open image!");
  }
}
