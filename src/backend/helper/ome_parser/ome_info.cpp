///
/// \file      ome_parser.cpp
/// \author    Joachim Danmayr
/// \date      2023-06-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "ome_info.hpp"
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <pugixml.hpp>

namespace joda::ome {

OmeInfo::OmeInfo()
{
}

///
/// \brief      Reads OME image information regarding the OME TIFF specification
/// \link       https://docs.openmicroscopy.org/ome-model/6.1.0/ome-tiff/specification.html
/// \author     Joachim Danmayr
/// \param[in]  omeXML  Read OME XML data as string
/// \return     Parsed OME information
///
void OmeInfo::loadOmeInformationFromXMLString(const std::string &omeXML)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(omeXML.c_str());
  if(!result) {
    throw std::invalid_argument("Error parsing OME information from file!");
  }
  std::string keyPrefix;    // OME:

TRY_AGAIN:
  std::string imageName =
      std::string(doc.child("OME").child(std::string(keyPrefix + "Image").data()).attribute("Name").as_string());
  if(imageName.empty() && keyPrefix.empty()) {
    keyPrefix = "OME:";
    goto TRY_AGAIN;
  }

  auto objectivManufacturer = std::string(doc.child("OME")
                                              .child(std::string(keyPrefix + "Instrument").data())
                                              .child(std::string(keyPrefix + "Objective").data())
                                              .attribute("Manufacturer")
                                              .as_string("Unknown"));
  auto objectivModel        = std::string(doc.child("OME")
                                              .child(std::string(keyPrefix + "Instrument").data())
                                              .child(std::string(keyPrefix + "Objective").data())
                                              .attribute("Model")
                                              .as_string("Unknown"));
  auto magnification        = doc.child("OME")
                           .child(std::string(keyPrefix + "Instrument").data())
                           .child(std::string(keyPrefix + "Objective").data())
                           .attribute("NominalMagnification")
                           .as_int();
  auto medium = std::string(doc.child("OME")
                                .child(std::string(keyPrefix + "Image").data())
                                .child(std::string(keyPrefix + "ObjectiveSettings").data())
                                .attribute("Medium")
                                .as_string("Unknown"));

  mObjectiveInfo = ObjectiveInfo{
      .manufacturer = objectivManufacturer, .model = objectivModel, .medium = medium, .magnification = magnification};

  int series = 0;

  for(pugi::xml_node image = doc.child("OME").child(std::string(keyPrefix + "Image").data()); image != nullptr;
      image                = image.next_sibling(std::string(keyPrefix + "Image").data())) {
    mImageInfo.emplace(series, ImageInfo{});
    auto &actImageInfo     = mImageInfo.at(series);
    actImageInfo.seriesIdx = series;
    series++;

    std::string imageName = std::string(image.attribute("Name").as_string());

    //
    // Plane numbers
    //
    auto sizeC = image.child(std::string(keyPrefix + "Pixels").data()).attribute("SizeC").as_int();
    auto sizeZ = image.child(std::string(keyPrefix + "Pixels").data()).attribute("SizeZ").as_int();
    auto sizeT = image.child(std::string(keyPrefix + "Pixels").data()).attribute("SizeT").as_int();
    auto dimOrder =
        std::string(image.child(std::string(keyPrefix + "Pixels").data()).attribute("DimensionOrder").as_string());

    // auto sizeX               = image.child(std::string(keyPrefix + "Pixels").data()).attribute("SizeX").as_ullong();
    // auto sizeY               = image.child(std::string(keyPrefix + "Pixels").data()).attribute("SizeY").as_ullong();
    // actImageInfo.imageSize   = sizeX * sizeY;
    // actImageInfo.imageWidth  = sizeX;
    // actImageInfo.imageHeight = sizeY;

    // auto type = std::string(image.child(std::string(keyPrefix + "Pixels").data()).attribute("Type").as_string());
    // if(type == "uint8") {
    //   actImageInfo.bits = 8;
    // } else if(type == "uint16") {
    //   actImageInfo.bits = 16;
    // } else {
    //   actImageInfo.bits = 16;
    // }

    //
    // TIFF Data
    // This is the implementation of the specification section >The TiffDataElement<
    // https://docs.openmicroscopy.org/ome-model/6.1.0/ome-tiff/specification.html
    // It is used to determine which plane is associated to which IFD (tiff directory) in the tiff
    //
    actImageInfo.nrOfChannels = sizeC;
    union Order
    {
      uint32_t order = 0xFFFFFFFF;
      struct _order_t
      {
        uint8_t C;
        uint8_t Z;
        uint8_t T;
        uint8_t unused;
      } _order;
    };

    int nrOfIFDS = sizeC * sizeZ * sizeT;
    std::map<uint32_t, Order> planeIFDorder;
    uint32_t startIfd = 0;

    pugi::xml_node pixels = image.child((keyPrefix + "Pixels").data());
    for(pugi::xml_node tiffData = pixels.child((keyPrefix + "TiffData").data()); tiffData != nullptr;
        tiffData                = tiffData.next_sibling((keyPrefix + "TiffData").data())) {
      startIfd                         = tiffData.attribute("IFD").as_int(0);
      planeIFDorder[startIfd]._order.C = tiffData.attribute("FirstC").as_int(0);
      planeIFDorder[startIfd]._order.Z = tiffData.attribute("FirstZ").as_int(0);
      planeIFDorder[startIfd]._order.T = tiffData.attribute("FirstT").as_int(0);
      nrOfIFDS                         = tiffData.attribute("PlaneCount").as_int(nrOfIFDS);
    }

    int cCnt = 0;
    int zCnt = 0;
    int tCnt = 0;

    int *pCnt  = &cCnt;
    int *qCnt  = &zCnt;
    int *rCnt  = &tCnt;
    int *pSize = &sizeC;
    int *qSize = &sizeZ;
    int *rSize = &sizeT;

    if(dimOrder == "XYZTC") {
      pCnt  = &zCnt;
      qCnt  = &tCnt;
      rCnt  = &cCnt;
      pSize = &sizeZ;
      qSize = &sizeT;
      rSize = &sizeZ;
    }
    if(dimOrder == "XYZCT") {
      pCnt  = &zCnt;
      qCnt  = &cCnt;
      rCnt  = &tCnt;
      pSize = &sizeZ;
      qSize = &sizeC;
      rSize = &sizeT;
    }
    if(dimOrder == "XYTCZ") {
      pCnt  = &tCnt;
      qCnt  = &cCnt;
      rCnt  = &zCnt;
      pSize = &sizeT;
      qSize = &sizeC;
      rSize = &sizeZ;
    }
    if(dimOrder == "XYTZC") {
      pCnt  = &tCnt;
      qCnt  = &zCnt;
      rCnt  = &cCnt;
      pSize = &sizeT;
      qSize = &sizeZ;
      rSize = &sizeC;
    }
    if(dimOrder == "XYCTZ") {
      pCnt  = &cCnt;
      qCnt  = &tCnt;
      rCnt  = &zCnt;
      pSize = &sizeC;
      qSize = &sizeT;
      rSize = &sizeZ;
    }
    if(dimOrder == "XYCZT") {
      pCnt  = &cCnt;
      qCnt  = &zCnt;
      rCnt  = &tCnt;
      pSize = &sizeC;
      qSize = &sizeZ;
      rSize = &sizeT;
    }

    for(int idf = startIfd; idf < nrOfIFDS; idf++) {
      if(0xFF == planeIFDorder[idf]._order.C) {
        planeIFDorder[idf]._order.C = cCnt;
      }
      if(0xFF == planeIFDorder[idf]._order.Z) {
        planeIFDorder[idf]._order.Z = zCnt;
      }
      if(0xFF == planeIFDorder[idf]._order.T) {
        planeIFDorder[idf]._order.T = tCnt;
      }

      // Counter for the matrix
      *pCnt = (*pCnt + 1) % *pSize;
      if(*pCnt == 0) {
        *qCnt = (*qCnt + 1) % *qSize;
      }
      if(*qCnt == 0 && *pCnt == 0) {
        *rCnt = (*rCnt + 1) % *rSize;
      }
    }

    //
    // Load channels
    //
    int idx = 0;
    for(pugi::xml_node channelNode = pixels.child((keyPrefix + "Channel").data()); channelNode != nullptr;
        channelNode                = channelNode.next_sibling((keyPrefix + "Channel").data())) {
      auto channelId = std::string(channelNode.attribute("ID").as_string());
      const std::string &channelNrString{channelId};
      int channelNr = -1;

      size_t last_colon = channelNrString.rfind(':');
      if(last_colon != std::string::npos) {
        std::string number_str = channelNrString.substr(last_colon + 1);
        try {
          channelNr = std::stoi(number_str);
        } catch(const std::invalid_argument &e) {
          joda::log::logWarning("Cannot parse channel nr for. Got >" + channelId + "<");
        }
      } else {
        joda::log::logWarning("Wrong Channel ID format in OME XML.");
      }

      auto channelName            = std::string(channelNode.attribute("Name").as_string("Unknown"));
      auto samplesPerPixel        = channelNode.attribute("SamplesPerPixel").as_float();
      auto contrastMethod         = std::string(channelNode.attribute("ContrastMethod").as_string("Unknown"));
      auto emissionWaveLength     = channelNode.attribute("EmissionWavelength").as_float();
      auto emissionWaveLengthUnit = std::string(channelNode.attribute("EmissionWavelengthUnit").as_string());
      auto detectorSettings       = channelNode.child("DetectorSettings");
      auto detectorId             = std::string(detectorSettings.attribute("ID").as_string());
      auto detectorBinning        = std::string(detectorSettings.attribute("Binning").as_string());

      actImageInfo.nrOfChannels++;

      std::map<uint32_t, TimeFrame> zStackForTimeFrame;
      for(const auto &[idf, order] : planeIFDorder) {
        if(order._order.C == channelNr) {
          // This is our channel -> Add the IDF of the timeframe
          zStackForTimeFrame[order._order.T].emplace(idf);
        }
      }

      actImageInfo.channels.emplace(channelNr, ChannelInfo{.channelId              = channelId,
                                                           .name                   = channelName,
                                                           .emissionWaveLength     = emissionWaveLength,
                                                           .emissionWaveLengthUnit = emissionWaveLengthUnit,
                                                           .contrastMethos         = contrastMethod,
                                                           .exposuerTime           = -1,
                                                           .zStackForTimeFrame     = zStackForTimeFrame});

      idx++;
    }

    //
    // Load planes
    //
    uint16_t nrOfPlanes = 0;
    for(pugi::xml_node plane = pixels.child((keyPrefix + "Plane").data()); plane != nullptr;
        plane                = plane.next_sibling((keyPrefix + "Plane").data())) {
      auto theZ               = plane.attribute("TheZ").as_int();
      auto theT               = plane.attribute("TheT").as_int();
      auto theC               = plane.attribute("TheC").as_int();
      ChannelInfo &chInfo     = actImageInfo.channels.at(theC);
      chInfo.exposuerTime     = plane.attribute("ExposureTime").as_float();
      chInfo.exposuerTimeUnit = std::string(plane.attribute("ExposureTimeUnit").as_string());
      nrOfPlanes++;
    }

    int64_t resolutionCount = doc.child("JODA").attribute("ResolutionCount").as_int();

    for(pugi::xml_node pyramid = doc.child("JODA").child(std::string("PyramidResolution").data()); pyramid != nullptr;
        pyramid                = pyramid.next_sibling(std::string("PyramidResolution").data())) {
      int32_t idx        = pyramid.attribute("idx").as_int();
      int32_t width      = pyramid.attribute("width").as_int();
      int32_t height     = pyramid.attribute("height").as_int();
      int32_t tileWidth  = pyramid.attribute("TileWidth").as_int();
      int32_t tileHeight = pyramid.attribute("TileHeight").as_int();
      int32_t bits       = pyramid.attribute("BitsPerPixel").as_int();

      actImageInfo.resolutions.emplace(
          idx, ImageInfo::Pyramid{.bits                   = bits,
                                  .imageMemoryUsage       = static_cast<int64_t>(width) * height * (bits / 8),
                                  .imageWidth             = width,
                                  .imageHeight            = height,
                                  .optimalTileMemoryUsage = static_cast<int64_t>(tileWidth) * tileHeight * (bits / 8),
                                  .optimalTileWidth       = tileWidth,
                                  .optimalTileHeight      = tileHeight});
    }
  }
}

///
/// \brief      If no OME information was found, emulate the information from image properties
/// \author     Joachim Danmayr
///
void OmeInfo::emulateOmeInformationFromTiff(const ImageInfo &prop)
{
  mImageInfo.emplace(0, prop);
  auto &actImageInfo = mImageInfo.at(0);

  for(uint32_t idx = 0; idx < actImageInfo.nrOfChannels; idx++) {
    std::map<uint32_t, TimeFrame> zStackForTimeFrame;
    zStackForTimeFrame.emplace(0, TimeFrame{idx});
    actImageInfo.channels.emplace(idx, ChannelInfo{.channelId              = "Channel:" + std::to_string(idx),
                                                   .name                   = "",
                                                   .emissionWaveLength     = 0,
                                                   .emissionWaveLengthUnit = "",
                                                   .contrastMethos         = "",
                                                   .zStackForTimeFrame     = zStackForTimeFrame});
  }
}

///
/// \brief      Returns the number of channels
/// \author     Joachim Danmayr
///
int OmeInfo::getNrOfChannels(int32_t series) const
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  return mImageInfo.at(series).nrOfChannels;
}

///
/// \brief      Returns the image bits
/// \author     Joachim Danmayr
///
[[nodiscard]] int32_t OmeInfo::getBits(int32_t series) const
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  return mImageInfo.at(series).resolutions.at(0).bits;
}

///
/// \brief      Returns the width/height size of the image
/// \author     Joachim Danmayr
///
[[nodiscard]] std::tuple<int64_t, int64_t> OmeInfo::getSize(int32_t series) const
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  return {mImageInfo.at(series).resolutions.at(0).imageWidth, mImageInfo.at(series).resolutions.at(0).imageHeight};
}

///
/// \brief      Returns the TIFF directories for one channel and timeframe
/// \author     Joachim Danmayr
///
auto OmeInfo::getDirectoryForChannel(uint32_t channel, uint32_t timeFrame, int32_t series) const -> std::set<uint32_t>
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  if(mImageInfo.at(series).channels.contains(channel)) {
    if(mImageInfo.at(series).channels.at(channel).zStackForTimeFrame.contains(timeFrame)) {
      return mImageInfo.at(series).channels.at(channel).zStackForTimeFrame.at(timeFrame);
    }
  }
  return std::set<uint32_t>{};
}

///
/// \brief     Return the series with the highest resolution
/// \author     Joachim Danmayr
/// \todo Support more image series
///
int32_t OmeInfo::getSeriesWithHighestResolution() const
{
  return 0;
}

}    // namespace joda::ome
