///
/// \file      pipeline.hpp
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

#pragma once

#include "image/image.hpp"
#include "reporting/reporting.h"
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

namespace joda::pipeline {

///
/// \class      Pipeline
/// \author     Joachim Danmayr
/// \brief      Pipeline base class
///
class Pipeline
{
public:
  /////////////////////////////////////////////////////
  Pipeline(const std::string &outputFolder, joda::reporting::Table *);

  virtual void analyzeImage(const joda::Image &img) = 0;

protected:
  /////////////////////////////////////////////////////
  auto getOutputFolder() const -> const std::string &;
  auto reporting() -> joda::reporting::Table *;

private:
  /////////////////////////////////////////////////////
  std::string mOutputFolder;
  joda::reporting::Table *mReporting;
};
}    // namespace joda::pipeline
