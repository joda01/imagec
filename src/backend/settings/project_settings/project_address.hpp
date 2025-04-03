
///
/// \file      project_experiment_setup.hpp
/// \author    Joachim Danmayr
/// \date      2024-04-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///

#pragma once

#include <set>
#include <vector>
#include "backend/settings/setting.hpp"
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
  std::string email;

  void check() const
  {
  }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT_EXTENDED(Address, firstName, lastName, country, organization,
                                                       streetAddress, postalCode, city, email);
};

}    // namespace joda::settings
