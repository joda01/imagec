///
/// \file      pipeline_count.hpp
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

#include <cstdint>
#include "algorithms/algorithm_executor.hpp"
#include "pipelines/pipeline.hpp"

namespace joda::pipeline {

///
/// \class      PipelineCount
/// \author     Joachim Danmayr
/// \brief      Count the number of particles in all channels
///
class PipelineCount : public Pipeline
{
public:
  /////////////////////////////////////////////////////
  using Pipeline::Pipeline;

private:
  /////////////////////////////////////////////////////
  void execute(const std::string &imgPath, const std::string &outputFolder, joda::reporting::Table &allOverReport,
               types::Progress *partialProgress) override;

  /////////////////////////////////////////////////////

  template <class T>
  void count(const std::string &imgPath, const std::string &outputFolder, joda::reporting::Table &allOverReport,
             types::Progress *partialProgress, uint32_t channel)
  {
    joda::algo::AlgorithmExecutor<T> counter(partialProgress);
    counter.executeAlgorithm(imgPath, outputFolder, allOverReport, channel, getStopReference());
  }
};

}    // namespace joda::pipeline
