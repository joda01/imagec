///
/// \file      web_Server.cpp
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

#include "web_server.hpp"
#include <httplib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "logger/console_logger.hpp"

namespace joda::http {

static const std::string PATH_TO_HTML_FILES_FOLDER = "imagec_gui";

///
/// \brief      Handle incoming file requests
/// \author     Joachim Danmayr
///
void handleRequest(const httplib::Request &req, httplib::Response &res)
{
  std::string path = req.path;
  if(path == "/") {
    path = "/index.html";
  }
  // Load HTML page from file
  std::ifstream file(PATH_TO_HTML_FILES_FOLDER + path);
  if(file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    // Set the response content type based on the file extension
    if(path.find(".html") != std::string::npos) {
      res.set_content(content, "text/html");
    } else if(path.find(".css") != std::string::npos) {
      res.set_content(content, "text/css");
    } else if(path.find(".js") != std::string::npos) {
      res.set_content(content, "text/javascript");
    } else {
      res.set_content(content, "text/plain");
    }
  } else {
    // File not found
    res.status = 404;
    res.set_content("File not found", "text/plain");
  }
}

///
/// \brief      HTTP listener
/// \author     Joachim Danmayr
/// \param[in]  listeningPort   Port the http server should listen on
///
void HttpServer::start(int listeningPort)
{
  httplib::Server server;

  // Set up request handler
  server.Get(R"(/(.+))", [&](const httplib::Request &req, httplib::Response &res) { handleRequest(req, res); });
  server.Get("/", [&](const httplib::Request &req, httplib::Response &res) { handleRequest(req, res); });

  joda::log::logInfo("Server is listening on port " + std::to_string(listeningPort) + ".");
  joda::log::logInfo("Open imageC UI with http://localhost:" + std::to_string(listeningPort) + ".");

  server.listen("0.0.0.0", listeningPort);
}
}    // namespace joda::http
