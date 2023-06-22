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
#include "helper/helper.hpp"
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

  Pipeline(const settings::json::AnalyzeSettings &);

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
  static inline const std::set<std::string> ALLOWED_EXTENSIONS = {".tif", ".tiff", ".btif", ".btiff", ".btf"};
  static inline const std::string RESULTS_PATH_NAME{"results"};

  /////////////////////////////////////////////////////
  virtual void execute(const std::string &imgPath, const std::string &outputFolder,
                       joda::reporting::Table &allOverReport, types::Progress *partialProgress) = 0;
  void lookForImagesInFolderAndSubfolder(const std::string &inputFolder);
  static auto prepareOutputFolder(const std::string &inputFolder) -> std::string;
  ///
  /// \brief Returns if the thread should be stopped
  [[nodiscard]] auto shouldThreadBeStopped() const -> bool
  {
    return mStop;
  }

  /////////////////////////////////////////////////////
  std::string mInputFolder;
  std::string mOutputFolder;
  std::vector<std::string> mListOfImagePaths;
  bool mStop = false;
  joda::settings::json::AnalyzeSettings mAnalyzeSettings;
  joda::reporting::Table mAllOverReporting;

  ProgressIndicator mProgress;
  State mState = State::STOPPED;
};

}    // namespace joda::pipeline
