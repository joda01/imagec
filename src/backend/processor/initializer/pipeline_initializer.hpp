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

#include "backend/enums/enum_images.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/project_settings/project_image_setup.hpp"
#include "pipeline_settings.hpp"

namespace joda::processor {

class PipelineInitializer
{
public:
  /////////////////////////////////////////////////////
  PipelineInitializer(const settings::ProjectImageSetup &settings, const std::filesystem::path &imagePath,
                      ImageContext &imageContextOut, processor::GlobalContext &globalContextOut);
  [[nodiscard]] const std::tuple<int32_t, int32_t> &getNrOfTilesToProcess() const
  {
    return mNrOfTiles;
  }
  [[nodiscard]] int32_t getNrOfTStacksToProcess() const
  {
    return mTstackToLoad;
  }
  [[nodiscard]] int32_t getNrOfZStacksToProcess() const
  {
    return mZStackToLoad;
  }

  [[nodiscard]] int32_t getNrOfCStacksToProcess() const
  {
    return mCStackToLoad;
  }

  void initPipeline(const joda::settings::PipelineSettings &settings, const enums::tile_t &tile,
                    const joda::enums::PlaneId &imagePartToLoad, ProcessContext &processStepOu);

private:
  /////////////////////////////////////////////////////
  inline static constexpr int64_t MAX_IMAGE_SIZE_BYTES_TO_LOAD_AT_ONCE = 100000000;
  inline static constexpr int32_t COMPOSITE_TILE_WIDTH                 = 4096;
  inline static constexpr int32_t COMPOSITE_TILE_HEIGHT                = 4096;

  /////////////////////////////////////////////////////
  bool mLoadImageInTiles                  = false;
  std::tuple<int32_t, int32_t> mNrOfTiles = {1, 1};
  int32_t mTstackToLoad                   = 0;
  int32_t mZStackToLoad                   = 0;
  int32_t mCStackToLoad                   = 0;

  /////////////////////////////////////////////////////
  const settings::ProjectImageSetup &mSettings;
  const processor::ImageContext &mImageContext;
};

}    // namespace joda::processor