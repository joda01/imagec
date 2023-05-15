///
/// \file      web_Server.hpp
/// \author    Joachim Danmayr
/// \date      2023-05-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#pragma once

namespace joda::http {

///
/// \class      HttpServer
/// \author     Joachim Danmayr
/// \brief      HTTP server which can load html pages
///
class HttpServer
{
public:
  /////////////////////////////////////////////////////
  HttpServer()
  {
  }
  void start(int listeningPort);

private:
  /////////////////////////////////////////////////////
};

}    // namespace joda::http
