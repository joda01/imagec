///
/// \file      ome_parser.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Parser which can strings regarding the OME XML
///            specification.
///
/// \ref       Goldberg, I.G., Allan, C., Burel, JM. et al.
///            The Open Microscopy Environment (OME) Data Model and XML file:
///            open tools for informatics and quantitative analysis in biological imaging.
///            Genome Biol 6, R47 (2005). https://doi.org/10.1186/gb-2005-6-5-r47
///
/// \link      https://docs.openmicroscopy.org/ome-model/5.6.3/ome-tiff/specification.html

#pragma once

#include <map>
#include <set>
#include <string>
#include "backend/image_processing/reader/image_reader.hpp"
#include <pugixml.hpp>

namespace joda::ome {

///
/// \class      OmeInfo
/// \author     Joachim Danmayr
/// \brief      Parses OME XML files
///
class OmeInfo
{
public:
  /////////////////////////////////////////////////////
  using TimeFrame = std::set<uint32_t>;

  /////////////////////////////////////////////////////
  struct ChannelInfo
  {
    std::string channelId;
    std::string name;
    float emissionWaveLength = 0;
    std::string emissionWaveLengthUnit;
    std::string contrastMethos;
    float exposuerTime = 0.0;
    std::string exposuerTimeUnit;
    std::map<uint32_t, TimeFrame> zStackForTimeFrame;    ///< TimeFrame <Time-Index, Frames in the time>
  };

  struct ObjectiveInfo
  {
    std::string manufacturer;
    std::string model;
    std::string medium;
    int32_t magnification = 0;
  };

  struct ImageInfo
  {
    int nrOfChannels       = 0;
    int64_t imageSize      = 0;
    uint64_t imageWidth    = 0;
    uint64_t imageHeight   = 0;
    int8_t bits            = 16;
    int64_t tileSize       = 0;
    int64_t nrOfTiles      = 0;
    uint16_t nrOfDocuments = 0;
    uint64_t tileWidth     = 0;
    uint64_t tileHeight    = 0;
  };

  /////////////////////////////////////////////////////
  OmeInfo();

  joda::image::ImageProperties loadOmeInformationFromXMLString(const std::string &omeXML);
  void emulateOmeInformationFromTiff(const joda::image::ImageProperties &);

  [[nodiscard]] int getNrOfChannels() const;
  [[nodiscard]] uint64_t getImageSize() const;
  [[nodiscard]] std::tuple<int64_t, int64_t> getSize() const;
  [[nodiscard]] int32_t getBits() const;
  [[nodiscard]] auto getDirectoryForChannel(uint32_t channel, uint32_t timeFrame) const -> std::set<uint32_t>;
  [[nodiscard]] const ImageInfo &getImageInfo() const
  {
    return mImageInfo;
  }
  [[nodiscard]] const ObjectiveInfo &getObjectiveInfo() const
  {
    return mObjectiveInfo;
  }

  [[nodiscard]] const std::map<uint32_t, ChannelInfo> &getChannelInfos() const
  {
    return mChannels;
  }

private:
  /////////////////////////////////////////////////////
  ImageInfo mImageInfo;
  ObjectiveInfo mObjectiveInfo;
  std::map<uint32_t, ChannelInfo> mChannels;    ///< Contains the channel information <channelIdx | channelinfo>
};
}    // namespace joda::ome
