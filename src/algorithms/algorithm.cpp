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

#include "algorithm.hpp"
#include "reporting/reporting.h"
#include "algorithm"

namespace joda::algo {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
/// \param[in]  outputFolder  Output folder of the results and control images
///
Algorithm::Algorithm(const std::string &outputFolder, joda::reporting::Table *reporting) :
    mOutputFolder(outputFolder), mReporting(reporting)
{
}

///
/// \brief      Returns the output folder for the results and control images
/// \author     Joachim Danmayr
/// \return     folder
///
auto Algorithm::getOutputFolder() const -> const std::string &
{
  return mOutputFolder;
}

auto Algorithm::reporting() -> joda::reporting::Table *
{
  return mReporting;
}

}    // namespace joda::algo
