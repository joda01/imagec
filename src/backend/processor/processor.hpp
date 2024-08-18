
///
/// \file      processor.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/global_enums.hpp"
#include "backend/helper/file_grouper/file_grouper.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include <opencv2/core/mat.hpp>

namespace joda::processor {

using imagesList_t = std::map<uint8_t, std::unique_ptr<joda::filesystem::DirectoryWatcher>>;

class Processor
{
public:
  /////////////////////////////////////////////////////
  Processor();
  void execute(const joda::settings::AnalyzeSettings &program);

  std::string initializeGlobalContext(const joda::settings::AnalyzeSettings &program, GlobalContext &globalContext);
  void initializePipelineContext(const joda::settings::AnalyzeSettings &program, const GlobalContext &globalContext,
                                 const PlateContext &plateContext, joda::grp::FileGrouper &grouper,
                                 const joda::filesystem::FileInfo &imagePath, PipelineInitializer &imageLoader,
                                 ImageContext &imageContext);

  void listImages(const joda::settings::AnalyzeSettings &program, imagesList_t &);
};
}    // namespace joda::processor
