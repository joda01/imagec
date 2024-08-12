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
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_loader/image_loader_settings.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"

namespace joda::cmd::functions {

class ImageLoader : public Command
{
public:
  /////////////////////////////////////////////////////
  ImageLoader(const ImageLoaderSettings &settings, const std::filesystem::path &imagePath);
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

  void execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
               ObjectsListMap &result) override;

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

  /////////////////////////////////////////////////////
  joda::ome::OmeInfo mOmeInfo;
  const ImageLoaderSettings &mSettings;
};

}    // namespace joda::cmd::functions
