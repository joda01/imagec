///
/// \file      nucleus_count.hpp
/// \author    Joachim Danmayr
/// \date      2023-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     Pipeline which implements an AI based nuclues counter
///

#include <cmath>
#include <mutex>
#include "algorithms/algorithm.hpp"
#include "functions/ai_object_detection/ai_object_detection.h"

namespace joda::algo {

///
/// \class      NucleusCounter
/// \author     Joachim Danmayr
/// \brief      Nucleus counter pipeline
///
class NucleusCounter : public Algorithm
{
public:
  /////////////////////////////////////////////////////
  using Algorithm::Algorithm;
  void execute(const joda::Image &img) override;
  static void mergeReport(const std::string &rowName, joda::reporting::Table &mergeTo,
                          const joda::reporting::Table &mergeFrom);

private:
  /////////////////////////////////////////////////////
  static void mergeReportInt(const std::string &rowName, joda::reporting::Table &mergeTo,
                             const joda::reporting::Table &mergeFrom);
  void writeReport(const joda::func::ai::DetectionResults &, const joda::Image &img);
  std::mutex mWriteMutex;
};

}    // namespace joda::algo
