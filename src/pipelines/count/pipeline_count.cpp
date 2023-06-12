///
/// \file      pipeline_count.cpp
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

#include "pipeline_count.hpp"
#include "algorithms/algorithm_executor.hpp"
#include "algorithms/nucleus_count_ai/nucleus_count.hpp"

namespace joda::pipeline {

///
/// \brief      Execute the analyze algorithms for counting
/// \author     Joachim Danmayr
/// \param[in]  imgPath Path to the image which should be analyzed
///
void PipelineCount::execute(const std::string &imgPath, const std::string &outputFolder,
                            joda::reporting::Table &allOverReport, types::Progress *partialProgress)
{
  joda::algo::AlgorithmExecutor<joda::algo::NucleusCounter> nucleusCounter(partialProgress);
  nucleusCounter.executeAlgorithm(imgPath, outputFolder, allOverReport, 14, getStopReference());
}

}    // namespace joda::pipeline
