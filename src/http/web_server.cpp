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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include "logger/console_logger.hpp"
#include "pipelines/pipeline.hpp"
#include "pipelines/pipeline_factory.hpp"
#include "settings/analze_settings_parser.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "version.h"

namespace fs = std::filesystem;

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
  HttpServer::addResponseHeader(res);
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
  ///////////////////////////////////////////////////////////////////////////
  //
  // API Server
  //
  std::string actProcessorUID;
  static const std::string welcomeMessage = "version: " + Version::getVersion() + "\n" +
                                            "sha256:  " + Version::getHash() + "\n" + "api version: " + API_VERSION +
                                            " | api port: " + std::to_string(listeningPort) + "\n" + Version::getLogo();

  std::cout << welcomeMessage << std::endl;
  joda::log::logInfo("Starting server...");

  //
  // Welcome page!
  //
  server.Get("/api", [&](const Request &req, Response &res) { res.set_content(welcomeMessage, "text/plain"); });

  server.Options(R"(/api/v1/(.+))",
                 [&](const httplib::Request &req, httplib::Response &res) { HttpServer::addResponseHeader(res); });
  server.Options("/api/v1",
                 [&](const httplib::Request &req, httplib::Response &res) { HttpServer::addResponseHeader(res); });

  //
  // Start analyzes
  //
  std::string start = "/api/" + API_VERSION + "/start";
  server.Post(start, [&](const Request &req, Response &res) {
    HttpServer::addResponseHeader(res);

    try {
      nlohmann::json object   = nlohmann::json::parse(req.body);
      std::string inputFolder = object["input_folder"];
      settings::json::AnalyzeSettings settings;
      settings.loadConfigFromString(req.body);
      actProcessorUID = joda::pipeline::PipelineFactory::startNewJob(settings, inputFolder);

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
    HttpServer::addResponseHeader(res);

    joda::log::logInfo("Analyze stopped from " + req.remote_addr + "!");
    joda::pipeline::PipelineFactory::stopJob(actProcessorUID);
    nlohmann::json retDoc;
    retDoc["status"] = "STOPPING";
    res.set_content(retDoc.dump(), "application/json");
  });

  //
  // Get progress of running analyzing
  //
  std::string progress = "/api/" + API_VERSION + "/getstate";
  server.Get(progress, [&](const Request &req, Response &res) {
    HttpServer::addResponseHeader(res);

    nlohmann::json retDoc;

    try {
      auto [progress, state] = joda::pipeline::PipelineFactory::getState(actProcessorUID);
      if(state == joda::pipeline::Pipeline::State::RUNNING) {
        retDoc["status"] = "RUNNING";
      }
      if(state == joda::pipeline::Pipeline::State::FINISHED) {
        retDoc["status"] = "FINISHED";
      }
      if(state == joda::pipeline::Pipeline::State::STOPPING) {
        retDoc["status"] = "STOPPING";
      }
      retDoc["actual_image"]["finished"] = progress.image.finished;
      retDoc["actual_image"]["total"]    = progress.image.total;
      retDoc["total"]["finished"]        = progress.total.finished;
      retDoc["total"]["total"]           = progress.total.total;
    } catch(const std::exception &ex) {
      retDoc["status"] = "FINISHED";
    }

    res.set_content(retDoc.dump(), "application/json");
  });

  //
  // List folders
  //
  std::string listFolders = "/api/" + API_VERSION + "/listfolders";
  server.Post(listFolders, [&](const Request &req, Response &res) {
    HttpServer::addResponseHeader(res);
    nlohmann::json retDoc;
    try {
      std::set<std::string> directories;
      std::set<std::string> files;

      nlohmann::json reqJson = nlohmann::json::parse(req.body);
      std::string startPath  = reqJson["path"];

      // File extensions to show: If empty only folders are shown
      std::set<std::string> fileExtensionsToShow;
      if(reqJson.contains("file_extensions")) {
        for(const auto &fileExtension : reqJson["file_extensions"]) {
          std::string ext = fileExtension;
          fileExtensionsToShow.emplace(ext);
        }
      }

      if(startPath.empty()) {
        startPath = fs::path(getenv("HOME"));
      }
      for(const auto &entry : fs::directory_iterator(startPath)) {
        if(fs::is_directory(entry.status())) {
          directories.emplace(entry.path());
        } else if(fileExtensionsToShow.contains(entry.path().extension())) {
          files.emplace(entry.path());
        }
      }
      retDoc["directories"] = directories;
      retDoc["files"]       = files;
      retDoc["home"]        = fs::path(getenv("HOME"));
    } catch(const std::exception &ex) {
      retDoc["directories"] = std::set<std::string>();
      retDoc["files"]       = std::set<std::string>();
      retDoc["home"]        = fs::path(getenv("HOME"));
    }
    res.set_content(retDoc.dump(), "application/json");
  });

  //
  // Return settings JSON
  //
  std::string getSettingsJson = "/api/" + API_VERSION + "/getsettings";
  server.Post(listFolders, [&](const Request &req, Response &res) {
    HttpServer::addResponseHeader(res);
    nlohmann::json retDoc;
    try {
      nlohmann::json reqJson         = nlohmann::json::parse(req.body);
      std::string pathToSettingsJson = reqJson["path"];
      if(pathToSettingsJson.ends_with(".json")) {
        std::ifstream input(pathToSettingsJson);
        std::stringstream buffer;
        buffer << input.rdbuf();
        auto setingsJson   = nlohmann::json::parse(buffer.str());
        retDoc["settings"] = setingsJson;
        input.close();
        res.status = 200;
      } else {
        res.status         = 204;
        retDoc["settings"] = {};
      }

    } catch(const std::exception &ex) {
      res.status         = 400;
      retDoc["settings"] = {};
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

  try {
    server.listen("0.0.0.0", listeningPort);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
  }
  joda::log::logError("Could not open port!");
}

void HttpServer::addResponseHeader(Response &res)
{
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Access-Control-Allow-Methods", "GET,PUT,PATCH,POST,DELETE,OPTIONS");
  //  res.set_header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
}

}    // namespace joda::http
