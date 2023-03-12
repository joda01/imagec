#///
/// \file      image_loader_tif.cpp
/// \author    Joachim Danmayr
/// \date      2023-03-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Allows to open TIFFs and Big Tiffs
///

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

///
/// \brief      Used to load (very) large TIFF images that cannot be loaded into RAM as a whole.
///             The prerequisite is that the TIF is stored as a Tiled TIFF.
///             With this method tile per tile can be loaded and it it possible to load more
///             than one tile and form a composite image of this tiles.
///
///             Loads >nrOfTilesToRead< tiles of a tiled TIFF image starting at >offset<
///             The tiles are read in squares and put together to form a picture.
///             Offset is the number of composite tile which should be read.
///
///             +-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |
///             |     |     |     |     |     |     |     |
///             +-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |
///             |     |     |     |     |     |     |     |
///             +-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |
///             |     |     |     |     |     |     |     |
///             +-----+-----+-----+-----+-----+-----+-----+
///             |3    |3    |3    |4    |4    |4    |5    |
///             |     |     |     |     |     |     |     |
///             +-----+-----+-----+-----+-----+-----+-----+
///
/// \author     Joachim Danmayr
/// \ref        http://www.simplesystems.org/libtiff//functions.html
///
/// \param[in]  filename  Name of the file to load
/// \param[in]  document  If it is multi document tiff, the index of
///                       the document which should be loaded
/// \param[in]  offset    Composite tile number to load
/// \param[in]  nrOfTilesToRead Nr of tiles which should form one composite image
/// \return Loaded composite image
///
cv::Mat TiffLoader::loadImageTile(const std::string &filename, int document, int offset, int nrOfTilesToRead)
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    auto nrOfDirectories = TIFFNumberOfDirectories(tif);
    if(document >= nrOfDirectories) {
      throw std::runtime_error("Nr. of directories exceeded!");
    }
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, document);

    //
    // Load TIF meta data
    //
    unsigned int width, height, tilewidth, tileheight, tileOffset, tileByteCount;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileheight);
    TIFFGetField(tif, TIFFTAG_TILEOFFSETS, &tileOffset);
    TIFFGetField(tif, TIFFTAG_TILEBYTECOUNTS, &tileByteCount);

    //
    // Messy piece of code. But I realized that there are TIFFs where the TIFFTAG_TILELENGTH meta is wrong.
    // In such a case we assume square tiles using the correct value read from TIFFTAG_TILEWIDTH
    //
    if(tileheight <= 0) {
      tileheight = tilewidth;
    }
    uint64_t npixels = tilewidth * tileheight;    // get the total number of pixels per tile

    // Calculate the total number of tiles in x, y direction and the total number of tiles of the whole image
    uint64_t nrOfXTiles = width / tilewidth;
    uint64_t nrOfYTiles = height / tileheight;
    uint64_t nrOfTiles  = nrOfXTiles * nrOfYTiles;

    //
    // We read squares, therefore calculate the square root of the number of
    // tiles to read and divide them evenly in x and y direction.
    // The result is the size of the newly created composite image we get at the end.
    //
    uint64_t newImageWidth  = tilewidth * std::sqrt(nrOfTilesToRead);
    uint64_t newImageHeight = tileheight * std::sqrt(nrOfTilesToRead);
    cv::Mat image           = cv::Mat(newImageWidth, newImageHeight, CV_8UC3);

    //
    // Now calculate the number of tiles in x and y direction based on the new image size.
    // We do this to compensate for rounding errors.
    //
    uint64_t tileNrX = newImageWidth / tilewidth;
    uint64_t tileNrY = newImageHeight / tileheight;

    //
    // Iterate through all tiles in X and Y directions, load the
    // Pixel values and write them to an openCV Matrix (Mat).
    //
    for(int tileOffsetY = 0; tileOffsetY < tileNrY; tileOffsetY++) {
      for(int tileOffsetX = 0; tileOffsetX < tileNrX; tileOffsetX++) {
        //
        // Calculate the x/y part of the tile to read
        //
        int tilePartX = tileOffsetX * tilewidth;
        int tilePartY = tileOffsetY * tileheight;

        //
        // Boundary check
        //
        uint64_t tileToReadX = tilePartX + (offset * tilewidth);
        uint64_t tileToReadY = tilePartY + (offset * tileheight);

        if(tileToReadX > width || tileToReadY > height) {
          throw std::runtime_error("Tries to read a tile which is not part of the image (array index out of bound)!");
        }

        // Allocate temp memory (must use the tiff library malloc)
        auto raster = (uint32 *) _TIFFmalloc(npixels * sizeof(uint32));
        if(raster == NULL) {
          TIFFClose(tif);
          throw std::runtime_error("Could not allocate memory for raster of TIFF image.");
        }

        // Read the R(ed), G(reen), B(blue) and A(alpha) channel if one tile of the original image
        if(!TIFFReadRGBATile(tif, tileToReadX, tileToReadY, raster)) {
          TIFFClose(tif);
          throw std::runtime_error("Could not read raster of TIFF image.");
        }

        //
        // Now it gets tricky: We have to correctly place the read pixel values from the original image
        // in the openCV image matrix. This is tricky since an openCV image Matrix has its origin (0,0) at the
        // button left, a TIFF image has it's origin at the top left. Next tricky thing is that images are
        // rotated counterclockwise and flipped.
        // We therefore have to write the tiles from the right to left, but the pixels of a tile must written from left
        // to right. This is done with: >(y + (tilewidth * (tileNrY - tileOffsetY - 1)))<. For the x direction it is
        // much more easier we can just write them as they are.
        //
        // In openCV language: Rotate the image 90 degrees couter clockwise
        // image = image.t();
        // cv::flip(image, image, 0);
        //
        for(uint x = 0; x < tilewidth; x++) {
          for(uint y = 0; y < tileheight; y++) {
            uint32 &TiffPixel =
                raster[y * tilewidth + x];    // Read the current pixel of the TIF (see:
                                              // http://www.simplesystems.org/libtiff//functions/TIFFReadRGBATile.html))
            int xImg = (x + tilePartX);
            int yImg = (y + (tilewidth * (tileNrY - tileOffsetY - 1)));

            cv::Vec3b &pixel = image.at<cv::Vec3b>(
                cv::Point(yImg, xImg));    // Get the reference of the pixel to write of the opencv matrix

            pixel[0] = TIFFGetB(TiffPixel);    // Set the pixel values as BGRA
            pixel[1] = TIFFGetG(TiffPixel);
            pixel[2] = TIFFGetR(TiffPixel);
            // pixel[3] = TIFFGetA(TiffPixel);   // We ignore the alpha channel since we are using CV_8UC3
          }
        }
        _TIFFfree(raster);    // Free the allocated memory before next round starts
      }
    }

    // Free the tiff image
    TIFFClose(tif);

    // Convert to a three channel float image, because we need this format for AI
    cv::Mat imageOut = cv::Mat(newImageWidth, newImageHeight, CV_32FC3);
    image.convertTo(imageOut, CV_32FC3);
    return imageOut;
  } else {
    throw std::runtime_error("Could not open image!");
  }
}

///
/// \brief      Load an entire tif image to the RAM and converts it
///             to an opencv Mat Matrix object
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
