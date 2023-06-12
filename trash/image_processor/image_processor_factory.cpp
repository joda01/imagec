///
/// \file      image_processor_factory.cpp
/// \author    Joachim Danmayr
/// \date      2023-05-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "image_processor_factory.hpp"
#include <memory>
#include <stdexcept>
#include <thread>
#include "helper/uid_generator.hpp"
#include "image_processor/image_processor.hpp"
#include "image_processor/image_processor_base.hpp"
#include "logger/console_logger.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"
#include "settings/analze_settings_parser.hpp"

namespace joda::processor {

using namespace std::chrono_literals;

///
/// \brief      Starts an observer thread
/// \author     Joachim Danmayr
///
void ImageProcessorFactory::initProcessorFactory()
{
  mProcessorObserver = std::make_shared<std::thread>(ImageProcessorFactory::observer);
}

///
/// \brief      Shutdown all running processes
/// \author     Joachim Danmayr
///
void ImageProcessorFactory::shutdownFactory()
{
  for(const auto &[_, processor] : mProcessors) {
    processor->stop();
  }
  mStopped = true;
}

///
/// \brief      Observer
/// \author     Joachim Danmayr
///
void ImageProcessorFactory::observer()
{
  while(!mStopped) {
    std::set<std::string> toDelete;

    for(const auto &[uid, processor] : mProcessors) {
      if(processor->isFinished()) {
        processor->wait();
        toDelete.emplace(uid);
        joda::log::logInfo("Analyze with process id >" + uid + "< finished!");
      }
    }

    for(const auto &uid : toDelete) {
      mProcessors.erase(uid);
    }
    std::this_thread::sleep_for(2.5s);
  }
}

///
/// \brief      Starts a processing pipeline bases on AnalyzeSettings.
///             The returning object contains a future which must be wait outside.
/// \author     Joachim Danmayr
/// \param[in]  inputFolder   Input folder
/// \param[in]  settings      Input settings
/// \return     UID of the running process
///
auto ImageProcessorFactory::startProcessing(const std::string &inputFolder,
                                            const joda::settings::json::AnalyzeSettings &settings) -> std::string
{
  if(!mProcessors.empty()) {
    throw std::runtime_error("There is still an analysis in progress!");
  }

  std::string resultsFolder = inputFolder + "/result";

  std::shared_ptr<joda::processor::ImageProcessorBase> processor;
  switch(settings.getPipeline()) {
    case joda::settings::json::AnalyzeSettings::Pipeline::COUNT:
      processor = std::make_shared<joda::processor::ImageProcessor<::joda::pipeline::NucleusCounter>>(inputFolder,
                                                                                                      resultsFolder);
      break;

    default:
      throw std::invalid_argument("Pipeline not supported!");
      break;
  }

  if(processor) {
    auto &workerThread = processor->start();
  }
  std::string uuid = createUuid();
  mProcessors.emplace(uuid, processor);
  return uuid;
}

///
/// \brief      Stop a running process
/// \author     Joachim Danmayr
/// \param[in]  processId Process to stop
///
void ImageProcessorFactory::stopProcess(const std::string &processId)
{
  for(const auto &[_, processor] : mProcessors) {
    processor->stop();
  }
}

///
/// \brief      Get running process
/// \author     Joachim Danmayr
/// \param[in]  processId Process to stop
///
auto ImageProcessorFactory::getProcess(const std::string &processId)
    -> std::shared_ptr<joda::processor::ImageProcessorBase>
{
  if(!mProcessors.contains(processId)) {
    throw std::invalid_argument("Process with ID >" + processId + "< does not exist!");
  }
  return mProcessors[processId];
}

}    // namespace joda::processor
