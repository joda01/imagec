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
#include <cstdint>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include "helper/helper.hpp"
#include "logger/console_logger.hpp"

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
void OmeInfo::loadOmeInformationFromString(const std::string &omeXML)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(omeXML.c_str());
  if(!result) {
    throw std::invalid_argument("Error parsing OME information from file!");
  }

  auto imageName = std::string(doc.child("OME").child("OME:Image").attribute("Name").as_string());

  //
  // Plane numbers
  //
  auto sizeC = doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("SizeC").as_int();
  auto sizeZ = doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("SizeZ").as_int();
  auto sizeT = doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("SizeT").as_int();
  auto dimOrder =
      std::string(doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("DimensionOrder").as_string());

  auto sizeX = doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("SizeX").as_ullong();
  auto sizeY = doc.child("OME").child("OME:Image").child("OME:Pixels").attribute("SizeY").as_ullong();
  mImageSize = sizeX * sizeY;

  //
  // TIFF Data
  // This is the implementation of the specification section >The TiffDataElement<
  // https://docs.openmicroscopy.org/ome-model/6.1.0/ome-tiff/specification.html
  // It is used to determine which plane is associated to which IFD (tiff directory) in the tiff
  //
  mNrOfChannels = sizeC;
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
  for(pugi::xml_node tiffData : doc.child("OME").child("OME:Image").child("OME:Pixels").children("OME:TiffData")) {
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
  for(pugi::xml_node channel : doc.child("OME").child("OME:Image").child("OME:Pixels").children("OME:Channel")) {
    auto channelId = std::string(channel.attribute("ID").as_string());
    std::string channelNrString{channelId};
    joda::helper::stringReplace(channelNrString, "Channel:", "");
    int channelNr = -1;
    try {
      channelNr = std::stoi(channelNrString);
    } catch(const std::exception &) {
      joda::log::logWarning("Cannot parse channel nr for. Got >" + channelId + "<");
    }

    auto channelName            = std::string(channel.attribute("Name").as_string());
    auto samplesPerPixel        = channel.attribute("SamplesPerPixel").as_float();
    auto contrastMethod         = std::string(channel.attribute("ContrastMethod").as_string());
    auto emissionWaveLength     = channel.attribute("EmissionWavelength").as_float();
    auto emissionWaveLengthUnit = std::string(channel.attribute("EmissionWavelengthUnit").as_string());
    auto color                  = channel.attribute("EmissionWavelength").as_uint();
    // auto prefix                 = std::string(channel.attribute("__prefix").as_string());

    auto detectorSettings = channel.child("DetectorSettings");
    auto detectorId       = std::string(detectorSettings.attribute("ID").as_string());
    auto detectorBinning  = std::string(detectorSettings.attribute("Binning").as_string());

    mNrOfChannels++;

    std::map<uint32_t, TimeFrame> zStackForTimeFrame;
    for(const auto &[idf, order] : planeIFDorder) {
      if(order._order.C == channelNr) {
        // This is our channel -> Add the IDF of the timeframe
        zStackForTimeFrame[order._order.T].emplace(idf);
      }
    }

    mChannels.emplace(channelNr,
                      ChannelInfo{.name = channelName, .color = color, .zStackForTimeFrame = zStackForTimeFrame});
  }

  //
  // Load planes
  //
  for(pugi::xml_node plane : doc.child("OME").child("OME:Image").child("OME:Pixels").children("OME:Plane")) {
    auto theZ           = plane.attribute("TheZ").as_int();
    auto theT           = plane.attribute("TheT").as_int();
    auto theC           = plane.attribute("TheC").as_int();
    ChannelInfo &chInfo = mChannels.at(theC);
  }
}

///
/// \brief      Returns the number of channels
/// \author     Joachim Danmayr
///
int OmeInfo::getNrOfChannels() const
{
  return mNrOfChannels;
}

///
/// \brief      Returns the image size
/// \author     Joachim Danmayr
///
uint64_t OmeInfo::getImageSize() const
{
  return mImageSize;
}

///
/// \brief      Returns the TIFF directories for one channel and timeframe
/// \author     Joachim Danmayr
///
auto OmeInfo::getDirectoryForChannel(uint32_t channel, uint32_t timeFrame) const -> std::set<uint32_t>
{
  if(mChannels.contains(channel)) {
    if(mChannels.at(channel).zStackForTimeFrame.contains(timeFrame)) {
      return mChannels.at(channel).zStackForTimeFrame.at(timeFrame);
    }
  }
  return std::set<uint32_t>{};
}
}    // namespace joda::ome
