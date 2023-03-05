

#include "image_loader_tif.hpp"

#include <iostream>
#include <ostream>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#define USER_DEFINED_INT64
#include <libtiff/tiffio.h>
#include <libtiff/tiffiop.h>

// http://www.simplesystems.org/libtiff//functions/TIFFCreateDirectory.html

void TiffLoader::openTiff(const std::string &filename)
{
  TIFF *tif = TIFFOpen(filename.c_str(), "r");
  if(tif) {
    unsigned int width, height, tilewidth, tileheight;
    uint32 *raster;
    uint16_t cnt1 = 0;

    // get the size of the tiff
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileheight);

    cnt1 = TIFFNumberOfDirectories(tif);
    TIFFSetDirectory(tif, 3);

    std::cout << "WID " << std::to_string(tilewidth) << std::endl;
    std::cout << "HIGH " << std::to_string(tileheight) << std::endl;
    std::cout << "CNT " << std::to_string(cnt1) << std::endl;

    uint npixels = tilewidth * tileheight;    // get the total number of pixels

    // Check the tif read to the raster correctly
    // if(!TIFFReadRGBAImage(tif, width, height, raster, 0)) {
    //  TIFFClose(tif);
    //  std::cerr << "Could not read raster of TIFF image" << std::endl;
    //  return;
    //}
    int tilePart = 0;

    for(tilePart = 0; tilePart < width; tilePart += tilewidth) {
      raster = (uint32 *) _TIFFmalloc(npixels *
                                      sizeof(uint32));    // allocate temp memory (must use the tiff library malloc)
      if(raster == NULL)                                  // check the raster's memory was allocaed
      {
        TIFFClose(tif);
        std::cerr << "Could not allocate memory for raster of TIFF image" << std::endl;
        return;
      }

      if(!TIFFReadRGBATile(tif, tilePart, tilePart, raster)) {
        TIFFClose(tif);
        std::cerr << "Could not read raster of TIFF image" << std::endl;
        return;
      }

      cv::Mat image = cv::Mat(tilewidth, tileheight,
                              CV_8UC4);    // create a new matrix of w x h with 8 bits per channel and 4 channels (RGBA)

      // itterate through all the pixels of the tif
      for(uint x = 0; x < tilewidth; x++)
        for(uint y = 0; y < tileheight; y++) {
          uint32 &TiffPixel = raster[y * tilewidth + x];               // read the current pixel of the TIF
          cv::Vec4b &pixel  = image.at<cv::Vec4b>(cv::Point(y, x));    // read the current pixel of the matrix
          pixel[0]          = TIFFGetB(TiffPixel);                     // Set the pixel values as BGRA
          pixel[1]          = TIFFGetG(TiffPixel);
          pixel[2]          = TIFFGetR(TiffPixel);
          pixel[3]          = TIFFGetA(TiffPixel);
        }

      _TIFFfree(raster);    // release temp memory
      // Rotate the image 90 degrees couter clockwise
      image = image.t();
      cv::flip(image, image, 0);
      cv::imwrite("bigtiff" + std::to_string(tilePart) + ".jpg", image);
    }

    TIFFClose(tif);
  }
}
