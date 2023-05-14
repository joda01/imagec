///
/// \file      api.hpp
/// \author    Joachim Danmayr
/// \date      2023-05-14
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

#include <httplib.h>

namespace joda::api {

class Api
{
public:
  /////////////////////////////////////////////////////
  Api();
  void start(int listenPort);

private:
  /////////////////////////////////////////////////////
  void startAnalyzing(const std::string &analyzingSettings);

  /////////////////////////////////////////////////////
  httplib::Server mServer;
};

}    // namespace joda::api
