///
/// \file      controller.cpp
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

#include "controller.hpp"

namespace joda::ctrl {

Controller::Controller()
{
}

///
/// \brief      Start a new process
/// \author     Joachim Danmayr
///
void Controller::start(const settings::json::AnalyzeSettings &settings)
{
  try {
    mActProcessId = joda::pipeline::PipelineFactory::startNewJob(settings, mWorkingDirectory.getWorkingDirectory(),
                                                                 &mWorkingDirectory);
    joda::log::logInfo("Analyze started!");
  } catch(const std::exception &ex) {
    joda::log::logWarning("Analyze could not be started! Got " + std::string(ex.what()) + ".");
  }
}

///
/// \brief      Stop a running process
/// \author     Joachim Danmayr
///
void Controller::stop()
{
  joda::pipeline::PipelineFactory::stopJob(mActProcessId);
}

///
/// \brief      Returns process state
/// \author     Joachim Danmayr
///
std::tuple<joda::pipeline::Pipeline::ProgressIndicator, joda::pipeline::Pipeline::State> Controller::getState()
{
  return joda::pipeline::PipelineFactory::getState(mActProcessId);
}

///
/// \brief      Get actual settings
/// \author     Joachim Danmayr
///
void Controller::getSettings()
{
}

///
/// \brief      Sets the working directory
/// \author     Joachim Danmayr
///
void Controller::setWorkingDirectory(const std::string &dir)
{
  mWorkingDirectory.setWorkingDirectory(dir);
}

///
/// \brief      Sets the working directory
/// \author     Joachim Danmayr
///
auto Controller::getNrOfFoundImages() -> uint32_t
{
  return mWorkingDirectory.getNrOfFiles();
}

///
/// \brief      Returns preview
/// \author     Joachim Danmayr
///
void Controller::preview()
{
}

}    // namespace joda::ctrl
