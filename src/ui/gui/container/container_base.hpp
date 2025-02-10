///
/// \file      container_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qwidget.h>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

///
/// \class      ContainerBase
/// \author     Joachim Danmayr
/// \brief      Base container class for channels and virtual channels
///
class ContainerBase
{
public:
  ContainerBase()
  {
  }
  virtual ~ContainerBase()
  {
  }
  virtual void toSettings()                                     = 0;
  virtual QWidget *getOverviewPanel()                           = 0;
  virtual QWidget *getEditPanel()                               = 0;
  virtual void setActive(bool)                                  = 0;
  virtual nlohmann::json toJson(const std::string &titlePrefix) = 0;
};

}    // namespace joda::ui::gui
