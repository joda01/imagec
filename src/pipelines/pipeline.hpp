///
/// \file      pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2023-06-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <set>
#include <string>
#include <vector>
#include "helper/directory_iterator.hpp"
#include "helper/helper.hpp"
#include "image_processing/functions/func_types.hpp"
#include "reporting/reporting.h"
#include "settings/analze_settings_parser.hpp"

namespace joda::pipeline {

///
/// \class      Pipeline
/// \author     Joachim Danmayr
/// \brief
///
class Pipeline
{
  friend class PipelineFactory;

public:
  /////////////////////////////////////////////////////

  struct ProgressIndicator
  {
    types::Progress total;
    types::Progress image;
  };

  enum class State
  {
    STOPPED,
    RUNNING,
    PAUSED,
    STOPPING,
    FINISHED,
  };

  Pipeline(const settings::json::AnalyzeSettings &, joda::helper::ImageFileContainer *imageFileContainer);

protected:
  /////////////////////////////////////////////////////

  ///
  /// \brief Returns the analyze settings of this pipeline
  [[nodiscard]] auto getAnalyzeSetings() const -> const joda::settings::json::AnalyzeSettings &
  {
    return mAnalyzeSettings;
  }

  auto getStopReference() -> bool &
  {
    return mStop;
  }

private:
  enum class ColumnIndexDetailedReport : int
  {
    CONFIDENCE    = 0,
    INTENSITY     = 1,
    INTENSITY_MIN = 2,
    INTENSITY_MAX = 3,
    AREA_SIZE     = 4,
    CIRCULARITY   = 5
  };
  static const int NR_OF_COLUMNS_PER_CHANNEL_IN_DETAIL_REPORT = 6;

  /////////////////////////////////////////////////////
  void runJob(const std::string &inputFolder);

  ///
  /// \brief Stop a running job
  void stopJob()
  {
    mState = State::STOPPING;
    mStop  = true;
  }

  ///
  /// \brief Total progress and state of the analysis
  [[nodiscard]] auto getState() const -> std::tuple<ProgressIndicator, State>
  {
    return {mProgress, mState};
  }

private:
  /////////////////////////////////////////////////////
  static inline const std::string RESULTS_PATH_NAME{"results"};

  /////////////////////////////////////////////////////
  static auto prepareOutputFolder(const std::string &inputFolder) -> std::string;
  ///
  /// \brief Returns if the thread should be stopped
  [[nodiscard]] auto shouldThreadBeStopped() const -> bool
  {
    return mStop;
  }

  static void appendToDetailReport(joda::func::ProcessingResult &result, joda::reporting::Table &detailReportTable,
                                   const std::string &detailReportOutputPath,
                                   const settings::json::ChannelSettings &channelSettings, int tempChannelIdx);
  static void appendToAllOverReport(joda::reporting::Table &allOverReport, const joda::reporting::Table &detailedReport,
                                    const std::string &imageName, int nrOfChannels);

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  bool mStop = false;
  joda::settings::json::AnalyzeSettings mAnalyzeSettings;
  joda::helper::ImageFileContainer *mImageFileContainer;

  ProgressIndicator mProgress;
  State mState = State::STOPPED;
};

}    // namespace joda::pipeline
