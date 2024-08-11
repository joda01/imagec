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

#include "backend/global_enums.hpp"
#include "backend/helper/roi/spartial_hash.hpp"

namespace joda::cmd {

struct ObjectsList
{
  ObjectsList()                               = default;
  ObjectsList(const ObjectsList &)            = delete;
  ObjectsList &operator=(const ObjectsList &) = delete;

  joda::enums::ObjectClassId classId;
};

class ObjectsListMap : public joda::roi::SpatialHash
{
public:
  using SpatialHash::SpatialHash;
  void createBinaryImage(cv::Mat &img, const std::set<joda::enums::ObjectClassId> &objectClasses) const;
};

}    // namespace joda::cmd
