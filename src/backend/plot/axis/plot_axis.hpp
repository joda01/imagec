///
/// \file      plot_axis.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <string>
#include <vector>
namespace joda::plot {

///
/// \class
/// \author     Joachim Danmayr
/// \brief
///
class Axis
{
public:
  /////////////////////////////////////////////////////
  void setLabels(const std::vector<std::string> &);
  [[nodiscard]] auto getLabels() const -> const std::vector<std::string> &;
};

}    // namespace joda::plot
