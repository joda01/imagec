
///
/// \file      project_experiment_setup.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

#pragma once

#include <set>
#include <vector>
#include <nlohmann/json.hpp>

namespace joda::settings {

struct Address
{
  std::string firstName;
  std::string lastName;
  std::string country;
  std::string organization;
  std::string streetAddress;
  std::string postalCode;
  std::string city;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Address, firstName, lastName, country, organization, streetAddress,
                                              postalCode, city);
};

}    // namespace joda::settings