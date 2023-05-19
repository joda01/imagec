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
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "image_processor/image_processor_base.hpp"
#include "image_processor/image_processor_factory.hpp"
#include "logger/console_logger.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "version.h"

namespace joda::http {

static const std::string PATH_TO_HTML_FILES_FOLDER = "imagec_gui";
static const std::string API_VERSION               = "v1";

using namespace httplib;

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

  ///////////////////////////////////////////////////////////////////////////
  //
  // API Server
  //
  std::string actProcessorUID;
  static const std::string welcomeMessage = "version: " + Version::getVersion() + "\n" +
                                            "sha256:  " + Version::getHash() + "\n" + "api version: " + API_VERSION +
                                            " | api port: " + std::to_string(listeningPort) + "\n" + Version::getLogo();

  std::cout << welcomeMessage << std::endl;

  //
  // Welcome page!
  //
  server.Get("/api", [&](const Request &req, Response &res) { res.set_content(welcomeMessage, "text/plain"); });

  //
  // Start analyzes
  //
  std::string start = "/api/" + API_VERSION + "/start";
  server.Post(start, [&](const Request &req, Response &res) {
    try {
      nlohmann::json object   = nlohmann::json::parse(req.body);
      std::string inputFolder = object["input_folder"];
      actProcessorUID         = joda::processor::ImageProcessorFactory::startProcessing(inputFolder, req.body);

      // actProcessor->wait();
      nlohmann::json retDoc;
      retDoc["status"]     = "RUNNING";
      retDoc["process_id"] = actProcessorUID;
      res.set_content(retDoc.dump(), "application/json");
      joda::log::logInfo("Analyze started from " + req.remote_addr + "!");

    } catch(const std::exception &ex) {
      nlohmann::json retDoc;
      retDoc["status"] = "error";
      retDoc["code"]   = ex.what();
      res.status       = 500;
      res.set_content(retDoc.dump(), "application/json");
      joda::log::logWarning("Analyze could not be started! Got " + std::string(ex.what()) + ".");
    }
  });

  //
  // Stop running analyzes
  //
  std::string stop = "/api/" + API_VERSION + "/stop";
  server.Get(stop, [&](const Request &req, Response &res) {
    joda::log::logInfo("Analyze stopped from " + req.remote_addr + "!");
    joda::processor::ImageProcessorFactory::stopProcess(actProcessorUID);
    nlohmann::json retDoc;
    retDoc["status"] = "STOPPING";
    res.set_content(retDoc.dump(), "application/json");
  });

  //
  // Get progress of running analyzing
  //
  std::string progress = "/api/" + API_VERSION + "/getstate";
  server.Get(progress, [&](const Request &req, Response &res) {
    nlohmann::json retDoc;

    try {
      auto [total, image] = joda::processor::ImageProcessorFactory::getProcess(actProcessorUID)->getProgress();
      retDoc["status"]    = "RUNNING";
      retDoc["actual_image"]["finished"] = image.finished;
      retDoc["actual_image"]["total"]    = image.total;
      retDoc["total"]["finished"]        = total.finished;
      retDoc["total"]["total"]           = total.total;
    } catch(const std::exception &ex) {
      retDoc["status"] = "FINISHED";
    }

    res.set_content(retDoc.dump(), "application/json");
  });

  ///////////////////////////////////////////////////////////////////////////
  //
  // HTTP Server
  //
  server.Get(R"(/(.+))", [&](const httplib::Request &req, httplib::Response &res) { handleRequest(req, res); });
  server.Get("/", [&](const httplib::Request &req, httplib::Response &res) { handleRequest(req, res); });

  //
  // Welcome messages
  //
  joda::log::logInfo("Server is listening on port " + std::to_string(listeningPort) + ".");
  joda::log::logInfo("Open imageC UI with http://localhost:" + std::to_string(listeningPort) + ".");
  joda::log::logInfo("API reachable under http://localhost/api/" + API_VERSION + ":" + std::to_string(listeningPort) +
                     ".");

  server.listen("0.0.0.0", listeningPort);
}
}    // namespace joda::http
