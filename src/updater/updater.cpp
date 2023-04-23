///
/// \file      updater.cpp
/// \author    Joachim Danmayr
/// \date      2023-04-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "updater.hpp"
#include <httplib.h>
#include <stdio.h>

Updater::Updater()
{
}

void Updater::download()
{
  httplib::Client cli("https://cpp-httplib-server.yhirose.repl.co");
}
