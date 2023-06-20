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
#include <iostream>
#include <stdexcept>

namespace joda::ome {

OmeInfo::OmeInfo()
{
}

void OmeInfo::loadOmeInformationFromString(const std::string &omeXML)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(omeXML.c_str());
  if(!result) {
    throw std::invalid_argument("Error parsing OME information from file!");
  }

  //
  // Load channels
  //
  mNrOfChannels = 0;
  for(pugi::xml_node channel : doc.child("OME").child("OME:Image").child("OME:Pixels").children("OME:Channel")) {
    auto channelId              = std::string(channel.attribute("ID").as_string());
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
  }

  //
  // Load planes
  //
  for(pugi::xml_node tool : doc.child("OME").child("Image").child("Pixels").children("Plane")) {
    int timeout   = tool.attribute("Timeout").as_int();
    mNrOfChannels = 0;
    if(timeout > 0) {
      std::cout << "Tool " << tool.attribute("Filename").value() << " has timeout " << timeout << "\n";
    }
  }
}
}    // namespace joda::ome
