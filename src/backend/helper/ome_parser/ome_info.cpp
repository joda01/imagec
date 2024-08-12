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

    pugi::xml_node pixels = image.child((keyPrefix + "Pixels").data());

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

      actImageInfo.channels.emplace(channelNr, ChannelInfo{.channelId              = channelId,
                                                           .name                   = channelName,
                                                           .emissionWaveLength     = emissionWaveLength,
                                                           .emissionWaveLengthUnit = emissionWaveLengthUnit,
                                                           .contrastMethos         = contrastMethod,
                                                           .exposuerTime           = -1});

      idx++;
    }

    actImageInfo.nrOfZStacks = sizeZ;
    actImageInfo.nrOfTStacks = sizeT;

    //
    // Load planes
    //
    for(pugi::xml_node plane = pixels.child((keyPrefix + "Plane").data()); plane != nullptr;
        plane                = plane.next_sibling((keyPrefix + "Plane").data())) {
      auto theZ               = plane.attribute("TheZ").as_int();
      auto theT               = plane.attribute("TheT").as_int();
      auto theC               = plane.attribute("TheC").as_int();
      ChannelInfo &chInfo     = actImageInfo.channels.at(theC);
      chInfo.exposuerTime     = plane.attribute("ExposureTime").as_float();
      chInfo.exposuerTimeUnit = std::string(plane.attribute("ExposureTimeUnit").as_string());
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
/// \brief      Returns the number of channels
/// \author     Joachim Danmayr
///
int OmeInfo::getNrOfZStack(int32_t series) const
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  return mImageInfo.at(series).nrOfZStacks;
}

///
/// \brief      Returns the number of channels
/// \author     Joachim Danmayr
///
int OmeInfo::getNrOfTStack(int32_t series) const
{
  if(series < 0) {
    series = getSeriesWithHighestResolution();
  }
  return mImageInfo.at(series).nrOfTStacks;
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
/// \brief     Return the series with the highest resolution
/// \author     Joachim Danmayr
/// \todo Support more image series
///
int32_t OmeInfo::getSeriesWithHighestResolution() const
{
  return 0;
}

}    // namespace joda::ome
