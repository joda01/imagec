///
/// \file      pipeline.cpp
/// \author    Joachim Danmayr
/// \date      2023-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Abstract base class for a pipeline run
///

#include "pipeline.hpp"
#include "reporting/reporting.h"

namespace joda::pipeline {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  outputFolder  Output folder of the results and control images
///
Pipeline::Pipeline(const std::string &outputFolder, joda::reporting::Reporting *reporting) :
    mOutputFolder(outputFolder), mReporting(reporting)
{
}

///
/// \brief      Returns the output folder for the results and control images
/// \author     Joachim Danmayr
/// \return     folder
///
auto Pipeline::getOutputFolder() const -> const std::string &
{
  return mOutputFolder;
}

auto Pipeline::reporting() -> joda::reporting::Reporting *
{
  return mReporting;
}

}    // namespace joda::pipeline
