///
/// \file      controller.hpp
/// \author    Joachim Danmayr
/// \date      2023-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

namespace joda::ctrl {

class Controller
{
public:
  Controller();
  void start();
  void stop();
  void getState();
  void listFolders();
  void getSettings();
  void setWorkingDirectory();
  void preview();

private:
};

}    // namespace joda::ctrl
