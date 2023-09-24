///
/// \file      controller.hpp
/// \author    Joachim Danmayr
/// \date      2023-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/helper/directory_iterator.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/settings/pipeline_settings.hpp"

namespace joda::ctrl {

///
/// \class      Controller
/// \author     Joachim Danmayr
/// \brief      Pipeline controller
///
class Controller
{
public:
  /////////////////////////////////////////////////////
  Controller();
  void start(const settings::json::AnalyzeSettings &settings);
  void stop();
  void reset();
  std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State, std::string> getState();
  auto getNrOfFoundImages() -> uint32_t;
  void getSettings();
  void setWorkingDirectory(const std::string &dir);
  struct Preview
  {
    std::vector<uchar> data;
    int height;
    int width;
  };
  auto preview(const settings::json::ChannelSettings &settings, int imgIndex) -> Preview;

private:
  /////////////////////////////////////////////////////
  joda::helper::ImageFileContainer mWorkingDirectory;
  std::string mActProcessId;
};

}    // namespace joda::ctrl
