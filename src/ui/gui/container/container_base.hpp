///
/// \file      container_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-03-24
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
  virtual QWidget *getEditPanel()                               = 0;
  virtual void setActive(bool)                                  = 0;
  virtual nlohmann::json toJson(const std::string &titlePrefix) = 0;
};

}    // namespace joda::ui::gui
