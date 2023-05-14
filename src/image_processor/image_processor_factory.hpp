///
/// \file      image_processor_factory.hpp
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

#pragma once

#include <memory>
#include <thread>
#include "image_processor/image_processor_base.hpp"
#include "settings/analze_settings_parser.hpp"

namespace joda::processor {

///
/// \class      ImageProcessorFactory
/// \author     Joachim Danmayr
/// \brief      Image processor factory
///
class ImageProcessorFactory
{
public:
  static void initProcessorFactory();
  static void shutdownFactory();

  /////////////////////////////////////////////////////
  static auto startProcessing(const std::string &inputFolder, const std::string &settings) -> std::string
  {
    settings::json::AnalyzeSettings analyzeSettings;
    analyzeSettings.loadConfigFromString(settings);
    return startProcessing(inputFolder, analyzeSettings);
  }

  static auto startProcessing(const std::string &inputFolder, const joda::settings::json::AnalyzeSettings &settings)
      -> std::string;

  static void stopProcess(const std::string &processId);
  static auto getProcess(const std::string &processId) -> std::shared_ptr<joda::processor::ImageProcessorBase>;

private:
  /////////////////////////////////////////////////////
  static void observer();

  /////////////////////////////////////////////////////
  static inline std::shared_ptr<std::thread> mProcessorObserver;
  static inline std::map<std::string, std::shared_ptr<joda::processor::ImageProcessorBase>> mProcessors;
  static inline bool mStopped = false;
};

}    // namespace joda::processor
