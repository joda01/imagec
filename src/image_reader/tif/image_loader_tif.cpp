///
/// \file      image_loader_tif.cpp
/// \author    Joachim Danmayr
/// \date      2023-03-12
///
/// \copyright Copyright 2023 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Allows to open TIFFs and Big Tiffs
///

#include "image_loader_tif.hpp"
#include <opencv2/core/hal/interface.h>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include "helper/ome_parser/ome_info.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pugixml.hpp>

#define USER_DEFINED_INT64
#include <libtiff/tiffio.h>
#include <libtiff/tiffiop.h>

void DummyHandler(const char *module, const char *fmt, va_list ap)
{
  // ignore errors and warnings (or handle them your own way)
}

///
/// \brief      Init the lib tif to suppress warnings
///             This method should be called once at top of main
/// \author     Joachim Danmayr
///
void TiffLoader::initLibTif()
{
  TIFFSetWarningHandler(DummyHandler);
  TIFFSetErrorHandler(DummyHandler);
}

///
/// \brief      Reads the metadata of the image
/// \author     Joachim Danmayr
/// \param[in]  filename
/// \return     Read meta data
///
auto TiffLoader::readOmeInformation(const std::string &filename) -> joda::ome::OmeInfo
{
  joda::ome::OmeInfo omeInfo;

  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, 0);
    char *omeXML = nullptr;
    TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &omeXML);
    try {
      omeInfo.loadOmeInformationFromString(std::string(omeXML));
    } catch(const std::exception &ex) {
      std::cout << ex.what() << std::endl;
    }
    TIFFClose(tif);
    return omeInfo;
  }
  return omeInfo;
}

///
/// \brief      Returns the number of tiles of an image
/// \author     Joachim Danmayr
/// \param[in]  filename
/// \param[in]  document
/// \return     Nr. of tiles
///
auto TiffLoader::getImageProperties(const std::string &filename, uint16_t directory) -> ImageProperties
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    auto nrOfDirectories = TIFFNumberOfDirectories(tif);
    if(directory >= nrOfDirectories) {
      throw std::runtime_error("Nr. of directories exceeded! Maximum is: " + std::to_string(nrOfDirectories) + ".");
    }
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, directory);

    unsigned int width      = tif->tif_dir.td_imagewidth;
    unsigned int height     = tif->tif_dir.td_imagelength;
    unsigned int tileWidth  = tif->tif_dir.td_tilewidth;
    unsigned int tileHeight = tif->tif_dir.td_tilelength;

    int64_t tileSize  = static_cast<int64_t>(tileWidth) * tileHeight;
    int64_t imageSize = static_cast<int64_t>(width) * height;
    int64_t nrOfTiles = imageSize / tileSize;
    TIFFClose(tif);

    return ImageProperties{
        .imageSize = imageSize, .tileSize = tileSize, .nrOfTiles = nrOfTiles, .nrOfDocuments = nrOfDirectories};
  }

  return ImageProperties{};
}

///
/// \brief      Used to load (very) large TIFF images that cannot be loaded as a whole into RAM.
///             Prerequisite is that the TIF is saved as a tiled TIFF.
///             With this method it is possible to load tile by tile, and it is possible to load
///             more than one tile and create a composite image from these tiles.
///
///             Loads >nrOfTilesToRead< tiles of a tiled TIFF image starting at >offset<
///             The tiles are read in squares and assembled into an image.
///             Offset is the number of composite tiles to read.
///
///             If sqrt(nrOfTilesToRead) is not a multiplier of nrOfTilesX / nrOfTilesY
///             the image is padded with black "virtual" tiles.
///             Example shows an image with size 4x7 tiles and nrOfTilesToRead = 9
///             Numbers in brackets are padded part of the image
///
///
///         (6) +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |(3)  |(3)  |(3)  |(4)  |(4)  |(4)  |(5)  |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///         (5) +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |(3)  |(3)  |(3)  |(4)  |(4)  |(4)  |(5)  |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///           4 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |3    |3    |3    |4    |4    |4    |5    |(5)  |(5)  |
///             |     |     |     |     |     |     |     |     |     |
///           3 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           2 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           1 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             |0    |0    |0    |1    |1    |1    |2    |(2)  |(2)  |
///             |     |     |     |     |     |     |     |     |     |
///           0 +-----+-----+-----+-----+-----+-----+-----+-----+-----+
///             0     1     2     3     4     5     6     (7)   (8)
///
/// \author     Joachim Danmayr
/// \ref        http://www.simplesystems.org/libtiff//functions.html
///
/// \param[in]  filename  Name of the file to load
/// \param[in]  directory  If it is multi document tiff, the index of
///                       the document which should be loaded
/// \param[in]  offset    Composite tile number to load
/// \param[in]  nrOfTilesToRead Nr of tiles which should form one composite image
/// \return Loaded composite image
///
cv::Mat TiffLoader::loadImageTile(const std::string &filename, uint16_t directory, int offset, int nrOfTilesToRead)
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    auto nrOfDirectories = TIFFNumberOfDirectories(tif);
    if(directory >= nrOfDirectories) {
      throw std::runtime_error("Nr. of directories exceeded!");
    }
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, directory);

    //
    // Load TIF meta data
    //
    unsigned int width      = tif->tif_dir.td_imagewidth;
    unsigned int height     = tif->tif_dir.td_imagelength;
    unsigned int tilewidth  = tif->tif_dir.td_tilewidth;
    unsigned int tileheight = tif->tif_dir.td_tilelength;

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
    uint64_t tilesPerLine   = std::sqrt(nrOfTilesToRead);
    uint64_t newImageWidth  = tilewidth * tilesPerLine;
    uint64_t newImageHeight = tileheight * tilesPerLine;
    cv::Mat image           = cv::Mat(newImageHeight, newImageWidth, CV_8UC3, 0.0);

    //
    // Now calculate the number of tiles in x and y direction based on the new image size.
    // We do this to compensate for rounding errors.
    //
    uint64_t tileNrX = newImageWidth / tilewidth;
    uint64_t tileNrY = newImageHeight / tileheight;

    //
    // Padding the image (make the number of tiles in x and y direction a multiplier of tilesPerLine)
    //
    uint64_t nrOfXTilesPadded = nrOfXTiles;
    if(nrOfXTiles % tilesPerLine > 0) {
      nrOfXTilesPadded += (tilesPerLine - (nrOfXTiles % tilesPerLine));
    }

    uint64_t nrOfYTilesPadded = nrOfYTiles;
    if(nrOfYTiles % tilesPerLine > 0) {
      nrOfYTilesPadded += (tilesPerLine - (nrOfYTiles % tilesPerLine));
    }

    //
    // Calculates the x and y tile offset based on the padded composite image offset
    //
    uint64_t offsetX = (offset * tilesPerLine) % nrOfXTilesPadded;
    uint64_t offsetY = (offset * tilesPerLine) % nrOfYTilesPadded;

    //
    // Iterate through all tiles in X and Y directions, load the
    // Pixel values and write them to an openCV Matrix (Mat).
    //
    for(int tileOffsetY = 0; tileOffsetY < tileNrX; tileOffsetY++) {
      for(int tileOffsetX = 0; tileOffsetX < tileNrX; tileOffsetX++) {
        //
        // Calculate the x/y part of the tile to read
        //
        int tilePartX = tileOffsetX * tilewidth;
        int tilePartY = tileOffsetY * tileheight;

        //
        // Boundary check
        //
        uint64_t tileToReadX = tilePartX + (offsetX * tilewidth);
        uint64_t tileToReadY = tilePartY + (offsetY * tileheight);

        if((tileToReadX + tilewidth) >= width || (tileToReadY + tileheight) >= height) {
          //  We reached the padding area -> Nothing to see here, just write a black tile
          for(uint y = 0; y < tileheight; y++) {
            for(uint x = 0; x < tilewidth; x++) {
              int xImg         = (x + tilePartX);
              int yImg         = (y + (tilewidth * (tileNrY - tileOffsetY - 1)));
              cv::Vec3b &pixel = image.at<cv::Vec3b>(
                  cv::Point(xImg, yImg));    // Get the reference of the pixel to write of the opencv matrix
              pixel[0] = 0;
              pixel[1] = 0;
              pixel[2] = 0;
              // pixel[3] = TIFFGetA(TiffPixel);   // We ignore the alpha channel since we are using CV_8UC3
            }
          }
          break;
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
        for(uint y = 0; y < tileheight; y++) {
          for(uint x = 0; x < tilewidth; x++) {
            uint32 &TiffPixel =
                raster[y * tilewidth + x];    // Read the current pixel of the TIF (see:
                                              // http://www.simplesystems.org/libtiff//functions/TIFFReadRGBATile.html))
            int xImg = (x + tilePartX);
            int yImg = (y + (tilewidth * (tileNrY - tileOffsetY - 1)));

            cv::Vec3b &pixel = image.at<cv::Vec3b>(
                cv::Point(xImg, yImg));    // Get the reference of the pixel to write of the opencv matrix

            pixel[0] = TIFFGetB(TiffPixel);    // Set the pixel values as BGRA
            pixel[1] = TIFFGetG(TiffPixel);
            pixel[2] = TIFFGetR(TiffPixel);
            //  pixel[3] = TIFFGetA(TiffPixel);   // We ignore the alpha channel since we are using CV_8UC3
          }
        }
        _TIFFfree(raster);    // Free the allocated memory before next round starts
      }
    }

    // Free the tiff image
    TIFFClose(tif);

    // Convert to a three channel float image, because we need this format for AI
    cv::Mat imageOut = cv::Mat(newImageHeight, newImageWidth, CV_32FC3);
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
    // Get the size of the tiff
    unsigned int width  = tif->tif_dir.td_imagewidth;
    unsigned int height = tif->tif_dir.td_imagelength;

    // Access wanted directory
    auto directoryNr = TIFFNumberOfDirectories(tif);
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
    cv::Mat image = cv::Mat(height, width, CV_8UC3, 0.0);

    for(uint x = 0; x < width; x++)
      for(uint y = 0; y < height; y++) {
        uint32 &TiffPixel = raster[y * width + x];                   // Read the current pixel of the TIF
        cv::Vec4b &pixel  = image.at<cv::Vec4b>(cv::Point(x, y));    // Read the current pixel of the matrix
        pixel[0]          = TIFFGetB(TiffPixel);                     // Set the pixel values as BGRA
        pixel[1]          = TIFFGetG(TiffPixel);
        pixel[2]          = TIFFGetR(TiffPixel);
        // pixel[3]          = TIFFGetA(TiffPixel);    // No alpha channel
      }

    _TIFFfree(raster);    // Free temp memory

    // Rotate the image 90 degrees couter clockwise
    image = image.t();
    cv::flip(image, image, 0);
    // Convert to a three channel float image, because we need this format for AI
    cv::Mat imageOut = cv::Mat(height, width, CV_32FC3);
    image.convertTo(imageOut, CV_32FC3);
    return imageOut;
  } else {
    throw std::runtime_error("Could not open image!");
  }
}
