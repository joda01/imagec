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

#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <opencv2/core/types.hpp>

namespace joda::ome {

static constexpr int64_t TILES_TO_LOAD_PER_RUN = 36;

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
    struct Pyramid
    {
      int64_t imageMemoryUsage = 0;
      int64_t imageNrOfPixels  = 0;
      int64_t imageWidth       = 0;
      int64_t imageHeight      = 0;
      int64_t tileNr           = 0;
    };
    int seriesIdx          = 0;
    int nrOfChannels       = 0;
    int8_t bits            = 16;
    int64_t tileNrOfPixels = 0;
    int64_t tileWidth      = 0;
    int64_t tileHeight     = 0;
    uint16_t nrOfDocuments = 0;
    std::map<int32_t, Pyramid> resolutions;      // Array of resolutions in case of a pyamid image
    std::map<uint32_t, ChannelInfo> channels;    ///< Contains the channel information <channelIdx | channelinfo>
  };

  /////////////////////////////////////////////////////
  OmeInfo();

  void loadOmeInformationFromXMLString(const std::string &omeXML);
  void emulateOmeInformationFromTiff(const ImageInfo &);

  [[nodiscard]] size_t getNrOfSeries() const
  {
    return mImageInfo.size();
  }
  [[nodiscard]] auto getResolutionCount(int32_t series = -1) const -> const std::map<int32_t, ImageInfo::Pyramid> &
  {
    if(series < 0) {
      series = getSeriesWithHighestResolution();
    }
    return mImageInfo.at(series).resolutions;
  }
  [[nodiscard]] int getNrOfChannels(int32_t series = -1) const;
  [[nodiscard]] uint64_t getImageSize(int32_t series = -1) const;
  [[nodiscard]] std::tuple<int64_t, int64_t> getSize(int32_t series = -1) const;
  [[nodiscard]] int32_t getBits(int32_t series = -1) const;
  [[nodiscard]] int32_t getSeriesWithHighestResolution() const;
  [[nodiscard]] auto getDirectoryForChannel(uint32_t channel, uint32_t timeFrame, int32_t series = -1) const
      -> std::set<uint32_t>;
  [[nodiscard]] const ImageInfo &getImageInfo(int32_t series = -1) const
  {
    if(series < 0) {
      series = getSeriesWithHighestResolution();
    }
    return mImageInfo.at(series);
  }
  [[nodiscard]] const ObjectiveInfo &getObjectiveInfo() const
  {
    return mObjectiveInfo;
  }

  [[nodiscard]] const std::map<uint32_t, ChannelInfo> &getChannelInfos(int32_t series = -1) const
  {
    if(series < 0) {
      series = getSeriesWithHighestResolution();
    }
    return mImageInfo.at(series).channels;
  }

private:
  /////////////////////////////////////////////////////
  std::map<int32_t, ImageInfo> mImageInfo;
  ObjectiveInfo mObjectiveInfo;
};
}    // namespace joda::ome
