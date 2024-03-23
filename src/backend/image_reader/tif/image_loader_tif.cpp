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
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include "../../helper/ome_parser/ome_info.hpp"
#include "../../logger/console_logger.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pugixml.hpp>

#define USER_DEFINED_INT64
#include <tiffio.h>
// #include <tiffiop.h>

void DummyHandler(const char *module, const char *fmt, va_list ap)
{
  // ignore errors and warnings (or handle them your own way)
}

void DummyHandlerExt(thandle_t, const char *, const char *, va_list)
{
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
  TIFFSetWarningHandlerExt(DummyHandlerExt);
}

///
/// \brief      Reads the metadata of the image
/// \author     Joachim Danmayr
/// \param[in]  filename
/// \return     Read meta data
///
auto TiffLoader::getOmeInformation(const std::string &filename) -> joda::ome::OmeInfo
{
  TIFFSetWarningHandler(DummyHandler);

  joda::ome::OmeInfo omeInfo;

  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, 0);
    char *omeXML = nullptr;
    if(1 == TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, &omeXML)) {
      try {
        omeInfo.loadOmeInformationFromString(std::string(omeXML));
      } catch(const std::exception &ex) {
        // No OME information found, emulate it by just using the TIFF meta data
        joda::log::logInfo("No OME information found. Use TIFF meta data instead!");
        omeInfo.emulateOmeInformationFromTiff(getImageProperties(filename, 0));
      }
      // _TIFFfree(omeXML);    // Free allocated memory
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
  TIFFSetWarningHandler(DummyHandler);

  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    auto nrOfDirectories = TIFFNumberOfDirectories(tif);
    if(directory >= nrOfDirectories) {
      throw std::runtime_error("Nr. of directories exceeded! Maximum is: " + std::to_string(nrOfDirectories) + ".");
    }
    // Set the directory to load the image from this directory
    TIFFSetDirectory(tif, directory);

    unsigned int width      = 0;    //= tif->tif_dir.td_imagewidth;
    unsigned int height     = 0;    //= tif->tif_dir.td_imagelength;
    unsigned int tileWidth  = 0;    //= tif->tif_dir.td_tilewidth;
    unsigned int tileHeight = 0;    //= tif->tif_dir.td_tilelength;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileHeight);

    int64_t tileSize  = static_cast<int64_t>(tileWidth) * tileHeight;
    int64_t imageSize = static_cast<int64_t>(width) * height;
    int64_t nrOfTiles = 0;
    if(tileSize > 0) {
      nrOfTiles = imageSize / tileSize;
    }
    TIFFClose(tif);

    return ImageProperties{.imageSize     = imageSize,
                           .tileSize      = tileSize,
                           .nrOfTiles     = nrOfTiles,
                           .nrOfDocuments = (uint16_t) nrOfDirectories,
                           .width         = width,
                           .height        = height,
                           .tileWidth     = tileWidth,
                           .tileHeight    = tileHeight};
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
    unsigned int width;         //= tif->tif_dir.td_imagewidth;
    unsigned int height;        //= tif->tif_dir.td_imagelength;
    unsigned int tilewidth;     //= tif->tif_dir.td_tilewidth;
    unsigned int tileheight;    //= tif->tif_dir.td_tilelength;
    uint16_t bitDepth;          //= tif->tif_dir.td_bitspersample;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileheight);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitDepth);

    //
    // Messy piece of code. But I realized that there are TIFFs where the TIFFTAG_TILELENGTH meta is wrong.
    // In such a case we assume square tiles using the correct value read from TIFFTAG_TILEWIDTH
    //
    if(tileheight <= 0) {
      tileheight = tilewidth;
    }

    auto [offsetX, offsetY] = calculateTileXYoffset(nrOfTilesToRead, offset, width, height, tilewidth, tileheight);

    //
    // We read squares, therefore calculate the square root of the number of
    // tiles to read and divide them evenly in x and y direction.
    // The result is the size of the newly created composite image we get at the end.
    //
    uint64_t tilesPerLine   = std::sqrt(nrOfTilesToRead);
    uint64_t newImageWidth  = tilewidth * tilesPerLine;
    uint64_t newImageHeight = tileheight * tilesPerLine;
    cv::Mat image           = cv::Mat(newImageHeight, newImageWidth, CV_16UC1, 0.0);

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
          /// \todo Check this there are sometimes artifacts
          for(uint y = 0; y < tileheight; y++) {
            for(uint x = 0; x < tilewidth; x++) {
              int xImg              = (x + tilePartX);
              int yImg              = (y + tilePartY);
              unsigned short &pixel = image.at<unsigned short>(
                  cv::Point(xImg, yImg));    // Get the reference of the pixel to write of the opencv matrix
              pixel = 0;
            }
          }
          break;
        }
        switch(bitDepth) {
          case 8:
            ReadGrayScaleImageTile<TIFF_8BIT_GRAYSCALE>(tif, tileToReadX, tileToReadY, tilewidth, tileheight, image,
                                                        tilePartX, tilePartY, tileNrY, tileOffsetY);
            break;
          case 16:
            ReadGrayScaleImageTile<TIFF_16BIT_GRAYSCALE>(tif, tileToReadX, tileToReadY, tilewidth, tileheight, image,
                                                         tilePartX, tilePartY, tileNrY, tileOffsetY);
            break;
          default:
            throw std::runtime_error("Only 8bit and 16bit images are supported!");
        }
      }
    }

    // Free the tiff image
    TIFFClose(tif);
    return image;
  }
  throw std::runtime_error("Could not open image!");
}

///
/// \brief      Calculates the tile x and y offset based on the image and tile size.
///             This is used because we load more than one tile at once.
///             For padding reasons at the edges of the image the calculation of these
///             offsets is a little bit more complicated
/// \author     Joachim Danmayr
/// \param[in]  nrOfTilesToRead  Nr. of tiles to read at once
/// \param[in]  offset   Tile part index to read
/// \param[in]  width    Image width
/// \param[in]  height   Image height
/// \param[in]  tilewidth    Tile width
/// \param[in]  tileheight   Tile height
/// \return Tile x,y offset
///
std::tuple<int64_t, int64_t> TiffLoader::calculateTileXYoffset(int32_t nrOfTilesToRead, int32_t offset, int64_t width,
                                                               int64_t height, int64_t tilewidth, int64_t tileheight)
{
  // Calculate the total number of tiles in x, y direction and the total number of tiles of the whole image
  if(tilewidth <= 0) {
    tilewidth = width;
  }
  if(tileheight <= 0) {
    tileheight = height;
  }
  uint64_t nrOfXTiles = width / tilewidth;
  uint64_t nrOfYTiles = height / tileheight;

  //
  // We read squares, therefore calculate the square root of the number of
  // tiles to read and divide them evenly in x and y direction.
  // The result is the size of the newly created composite image we get at the end.
  //
  uint64_t tilesPerLine = std::sqrt(nrOfTilesToRead);

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
  uint64_t offsetY = ((uint64_t) (offset * tilesPerLine) / nrOfYTilesPadded) * tilesPerLine;

  return {offsetX, offsetY};
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
  TIFFSetWarningHandler(DummyHandler);

  // return  cv::imread(filename, CV_32FC3);

  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    // Access wanted directory
    auto directoryNr = TIFFNumberOfDirectories(tif);
    if(directory >= directoryNr) {
      throw std::runtime_error("Directory index too large! Image has only " + std::to_string(directoryNr) +
                               " directrories.");
    }
    TIFFSetDirectory(tif, directory);

    // Get the size of the tiff
    unsigned int width;     //= tif->tif_dir.td_imagewidth;
    unsigned int height;    //= tif->tif_dir.td_imagelength;
    uint16_t bitDepth;      //= tif->tif_dir.td_bitspersample;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitDepth);

    switch(bitDepth) {
      case 8: {
        auto img = ReadGrayScaleImage<TIFF_8BIT_GRAYSCALE>(tif, width, height);
        TIFFClose(tif);
        return img;
      }
      case 16: {
        auto img = ReadGrayScaleImage<TIFF_16BIT_GRAYSCALE>(tif, width, height);
        TIFFClose(tif);
        return img;
      }
      default:
        throw std::runtime_error("Only 8bit and 16bit images are supported!");
    }
  } else {
    throw std::runtime_error("Could not open image!");
  }
}
