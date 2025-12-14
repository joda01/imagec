///
/// \file      image_loader.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <filesystem>
#include <tuple>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/reader/image_reader.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include "backend/settings/project_settings/project_pipeline_setup.hpp"
#include <opencv2/core/types.hpp>
#include "pipeline_settings.hpp"

namespace joda::processor {

class PipelineInitializer
{
public:
  /////////////////////////////////////////////////////

  struct TileSize
  {
    int32_t width = 0;
    int32_t height;
  };

  /////////////////////////////////////////////////////
  PipelineInitializer(const settings::ProjectImageSetup &settings, const settings::ProjectPipelineSetup &pipelineSetup,
                      const std::filesystem::path &imagePath, const std::filesystem::path &imagesBasePath);

  [[nodiscard]] const std::tuple<int32_t, int32_t> &getNrOfTilesToProcess() const
  {
    return mNrOfTiles;
  }
  [[nodiscard]] uint32_t getNrOfTStacksToProcess() const
  {
    return mTstackToLoad;
  }
  [[nodiscard]] uint32_t getNrOfZStacksToProcess() const
  {
    return mZStackToLoad;
  }

  auto getCompositeTileSize() const -> TileSize const;

  enums::ImageId loadImageAndStoreToCache(enums::MemoryScope scope, const enums::PlaneId &planeToLoad, enums::ZProjection zProjection,
                                          const enums::tile_t &tile, joda::processor::ProcessContext &processContext) const;

  void initPipeline(const joda::settings::PipelineSettings &settings, const enums::tile_t &tile, const joda::enums::PlaneId &imagePartToLoad,
                    ProcessContext &processStepOu, int32_t pipelineIndex) const;

  auto &getImagePath() const
  {
    return mImageRead.getImagePath();
  }

  auto getImageId() const
  {
    return mImageId;
  }

  auto getSeries() const
  {
    return mSelectedSeries;
  }

  auto getImageHeight() const
  {
    return mImageMeta.getImageHeight(mSelectedSeries, 0);
  }

  auto getImageWidth() const
  {
    return mImageMeta.getImageWidth(mSelectedSeries, 0);
  }

  [[nodiscard]] cv::Size getTileSize() const
  {
    return tileSize;
  }

  ome::PhyiscalSize getPhysicalPixelSIzeOfImage() const
  {
    return mImageMeta.getPhyiscalSize(mSelectedSeries);
  }

  auto getImageSize() const
  {
    return mImageMeta.getSize(mSelectedSeries);
  }

  auto getNrOfChannels() const
  {
    return mImageMeta.getNrOfChannels(mSelectedSeries);
  }

  auto getNrOfZStack() const
  {
    return mImageMeta.getNrOfZStack(mSelectedSeries);
  }

  auto getNrOfTStack() const
  {
    return mImageMeta.getNrOfTStack(mSelectedSeries);
  }

  auto getPhyiscalSize() const
  {
    return mImageMeta.getPhyiscalSize(mSelectedSeries);
  }

  auto getChannelInfos() const
  {
    return mImageMeta.getChannelInfos(mSelectedSeries);
  }

  enums::Units getPixelSizeUnit() const
  {
    return mSettings.imagePixelSizeSettings.pixelSizeUnit;
  }

private:
  /////////////////////////////////////////////////////
  static int32_t limitChannel(int32_t wantedIndex, int32_t maxIndex);

  /////////////////////////////////////////////////////
  std::tuple<int32_t, int32_t> mNrOfTiles = {1, 1};
  uint32_t mTstackToLoad                  = 0;
  uint32_t mZStackToLoad                  = 0;
  int32_t mTotalNrOfChannels              = 0;
  int32_t mTotalNrOfZChannels             = 0;
  int32_t mTotalNrOfTChannels             = 0;
  int32_t mSelectedSeries                 = 0;

  /////////////////////////////////////////////////////
  const settings::ProjectImageSetup &mSettings;
  const settings::ProjectPipelineSetup &mSettingsPipeline;
  image::reader::ImageReader mImageRead;
  joda::ome::OmeInfo mImageMeta;

  // IMAGE context///////////////////////////////////////////////////
  uint32_t nrOfZStacks  = 0;
  cv::Size tileSize     = {};
  bool loadImageInTiles = false;
  uint64_t mImageId;
};

}    // namespace joda::processor
