///
/// \file        uuid.hpp
/// \author      Joachim Danmayr
/// \date        Created at: 2022-05-23
/// \date        Last modified at: 2022-05-23
/// ---
/// \copyright   Copyright 2022 Fronius International GmbH.
///              https://www.fronius.com
///

#pragma once

#define UUID_SYSTEM_GENERATOR

#include <uuid/uuid.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

inline std::string createUuid() noexcept
{
  uuid_t uuid;
  uuid_generate_random(uuid);

  char uuidStr[37];    // UUID string representation buffer
  uuid_unparse_lower(uuid, uuidStr);

  return uuidStr;
}