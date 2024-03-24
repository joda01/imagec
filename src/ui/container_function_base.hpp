///
/// \file      container_function_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <QtWidgets>

namespace joda::ui::qt {

class ContainerFunctionBase : public QWidget
{
  Q_OBJECT

public:
  ContainerFunctionBase();
  ~ContainerFunctionBase()
  {
  }

signals:
  void valueChanged();

protected:
  void triggerValueChanged();
};

}    // namespace joda::ui::qt
