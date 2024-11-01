///
/// \file      image_loader.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <tuple>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
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
  PipelineInitializer(const settings::ProjectImageSetup &settings);
  void init(ImageContext &imageContextOut);

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

  enums::ImageId loadImageToCache(const enums::PlaneId &planeToLoad, enums::ZProjection zProjection, const enums::tile_t &tile,
                                  joda::processor::ProcessContext &processContext) const;

  void initPipeline(const joda::settings::PipelineSettings &settings, const enums::tile_t &tile, const joda::enums::PlaneId &imagePartToLoad,
                    ProcessContext &processStepOu) const;

private:
  /////////////////////////////////////////////////////
  bool mLoadImageInTiles                  = false;
  std::tuple<int32_t, int32_t> mNrOfTiles = {1, 1};
  uint32_t mTstackToLoad                  = 0;
  uint32_t mZStackToLoad                  = 0;
  uint32_t mNrOfZStacks                   = 0;
  int32_t mTotalNrOfChannels              = 0;

  /////////////////////////////////////////////////////
  const settings::ProjectImageSetup &mSettings;
  processor::ImageContext *mImageContext = nullptr;
};

}    // namespace joda::processor
