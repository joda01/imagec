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
#include "../../image_reader/image_reader.hpp"
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
  OmeInfo();

  void loadOmeInformationFromString(const std::string &omeXML);
  ImageProperties loadOmeInformationFromJsonString(const std::string &omeJson);
  void emulateOmeInformationFromTiff(const ImageProperties &);

  [[nodiscard]] int getNrOfChannels() const;
  [[nodiscard]] uint64_t getImageSize() const;
  [[nodiscard]] std::tuple<int64_t, int64_t> getSize() const;
  [[nodiscard]] auto getDirectoryForChannel(uint32_t channel, uint32_t timeFrame) const -> std::set<uint32_t>;

private:
  /////////////////////////////////////////////////////
  using TimeFrame = std::set<uint32_t>;

  struct ChannelInfo
  {
    std::string name;
    uint32_t color;
    std::map<uint32_t, TimeFrame> zStackForTimeFrame;    ///< TimeFrame <Time-Index, Frames in the time>
  };

  /////////////////////////////////////////////////////
  int mNrOfChannels    = 0;
  int64_t mImageSize   = 0;
  int64_t mImageWidth  = 0;
  int64_t mImageHeight = 0;

  std::map<uint32_t, ChannelInfo> mChannels;    ///< Contains the channel information <channelIdx | channelinfo>
};
}    // namespace joda::ome
