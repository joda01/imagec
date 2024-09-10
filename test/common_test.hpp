///
/// \file      common_test.hpp
/// \author    Joachim Danmayr
/// \date      2024-06-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <exception>
#include <filesystem>
#include <memory>
#include "backend/helper/random_name_generator.hpp"
#include "controller/controller.hpp"
#include <nlohmann/json_fwd.hpp>

template <size_t N>
struct StringLiteral
{
  constexpr StringLiteral(const char (&str)[N])
  {
    std::copy_n(str, N, value);
  }

  char value[N];
};

namespace joda::test {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
inline std::unique_ptr<joda::ctrl::Controller> executePipeline(const std::filesystem::path &cfgJsonPath,
                                                               const std::filesystem::path &imagesPath)
{
  auto controller = std::make_unique<joda::ctrl::Controller>();

  joda::settings::AnalyzeSettings settings = nlohmann::json::parse(std::ifstream{cfgJsonPath});
  SettingParserLog_t logTrace;
  std::cout << "LOG----------" << std::endl;
  settings.getErrorLogRecursive(logTrace);
  for(const auto &trace : logTrace) {
    trace.print();
  }

  controller->setWorkingDirectory(0, imagesPath);
  std::this_thread::sleep_for(2s);
  controller->start(settings, controller->calcOptimalThreadNumber(settings),
                    joda::helper::RandomNameGenerator::GetRandomName());

  while(true) {
    std::this_thread::sleep_for(2s);
    const auto &pipState = controller->getState();
    if(pipState.isFinished()) {
      break;
    }
  }
  try {
    controller->stop();
  } catch(const std::exception &ex) {
    std::cout << ex.what() << std::endl;
  }
  return controller;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
inline std::unique_ptr<joda::db::Database> getAnalyze(const std::filesystem::path &databaseFile)
{
  auto analyze = std::make_unique<joda::db::Database>();
  analyze->openDatabase(databaseFile);
  return analyze;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
inline nlohmann::json parseProfiling(const std::filesystem::path &outputFolder)
{
  auto profilingFile = outputFolder / "profiling.json";

  std::ifstream input_file(profilingFile.string());
  if(!input_file.is_open()) {
    std::cerr << "Failed to open file: " << profilingFile.string() << std::endl;
    return 1;
  }

  nlohmann::json parsedProfiler;
  input_file >> parsedProfiler;
  return parsedProfiler;
}

}    // namespace joda::test
