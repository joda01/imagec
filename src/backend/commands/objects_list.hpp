///
/// \file      objects_list.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include "backend/settings/anaylze_settings_enums.hpp"

namespace joda::cmd {

struct ObjectsList
{
  ObjectsList()                               = default;
  ObjectsList(const ObjectsList &)            = delete;
  ObjectsList &operator=(const ObjectsList &) = delete;

  joda::settings::ObjectClassId classId;
};

class ObjectsListMap : public std::map<joda::settings::ObjectClassId, ObjectsList>
{
public:
  ObjectsListMap()                                  = default;
  ObjectsListMap(const ObjectsListMap &)            = delete;
  ObjectsListMap &operator=(const ObjectsListMap &) = delete;
  ObjectsListMap clone()
  {
    return {};
  }
};

}    // namespace joda::cmd
