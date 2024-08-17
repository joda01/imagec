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

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include "backend/enums/types.hpp"
#include <opencv2/core/types.hpp>

namespace joda::ome {

struct TileToLoad
{
  int32_t tileX      = 0;
  int32_t tileY      = 0;
  int32_t tileWidth  = 256;
  int32_t tileHeight = 256;
};

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
  struct ImagePlane
  {
    float exposureTime = 0.0;
    std::string exposureTimeUnit;
  };

  struct ChannelInfo
  {
    std::string channelId;
    std::string name;
    float emissionWaveLength = 0;
    std::string emissionWaveLengthUnit;
    std::string contrastMethod;
    std::map<enums::tStack_t, std::map<enums::zStack_t, ImagePlane>> planes;
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
      int32_t bits                   = 16;
      int64_t imageMemoryUsage       = 0;
      int32_t imageWidth             = 0;
      int32_t imageHeight            = 0;
      int64_t optimalTileMemoryUsage = 0;
      int32_t optimalTileWidth       = 0;
      int32_t optimalTileHeight      = 0;

      [[nodiscard]] int32_t getTileCount(int32_t tileWidth, int32_t tileHeight) const
      {
        auto [x, y] = getNrOfTiles(tileWidth, tileHeight);
        return x * y;
      }

      [[nodiscard]] int32_t getTileCount() const
      {
        auto [x, y] = getNrOfTiles(optimalTileWidth, optimalTileHeight);
        return x * y;
      }

      [[nodiscard]] auto getNrOfTiles(int32_t tileWidth, int32_t tileHeight) const -> std::tuple<int32_t, int32_t>
      {
        double tileNrWidth  = std::ceil(static_cast<double>(imageWidth) / static_cast<double>(tileWidth));
        double tileNrHeight = std::ceil(static_cast<double>(imageHeight) / static_cast<double>(tileHeight));
        return {tileNrWidth, tileNrHeight};
      }
      [[nodiscard]] auto toTileNr(const joda::ome::TileToLoad &tile) const
      {
        auto [tileNrX, tileNrY] = getNrOfTiles(tile.tileWidth, tile.tileHeight);
        return tile.tileX + tile.tileY * tileNrX;
      }
      [[nodiscard]] auto tileNrToTile(int32_t tileNr, int32_t tileWidth, int32_t tileHeight) const
          -> std::tuple<int32_t, int32_t>
      {
        auto [tileNrX, tileNrY] = getNrOfTiles(tileWidth, tileHeight);
        int32_t tileX           = tileNr % tileNrX;
        int32_t tileY           = tileNr / tileNrX;
        return {tileX, tileY};
      }
    };
    int32_t seriesIdx    = 0;
    int32_t nrOfChannels = 0;
    int32_t nrOfZStacks  = 0;
    int32_t nrOfTStacks  = 0;
    std::map<int32_t, Pyramid> resolutions;      ///< Array of resolutions in case of a pyamid image
    std::map<uint32_t, ChannelInfo> channels;    ///< Contains the channel information <channelIdx | channelinfo>
  };

  /////////////////////////////////////////////////////
  OmeInfo();

  void loadOmeInformationFromXMLString(const std::string &omeXML);

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
  [[nodiscard]] int getNrOfZStack(int32_t series = -1) const;
  [[nodiscard]] int getNrOfTStack(int32_t series = -1) const;

  [[nodiscard]] std::tuple<int64_t, int64_t> getSize(int32_t series = -1) const;
  [[nodiscard]] int32_t getBits(int32_t series = -1) const;
  [[nodiscard]] int32_t getSeriesWithHighestResolution() const;

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
