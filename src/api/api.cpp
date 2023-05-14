///
/// \file      api.cpp
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

#include "api.hpp"
#include <exception>
#include <memory>
#include <string>
#include "image_processor/image_processor_base.hpp"
#include "image_processor/image_processor_factory.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "version.h"

namespace joda::api {

using namespace httplib;

Api::Api()
{
}

///
/// \brief      Start API Server and wait
/// \author     Joachim Danmayr
///
void Api::start(int listenPort)
{
  static std::string apiVersion = "v1";
  std::string actProcessorUID;

  //
  // Welcome page!
  //
  mServer.Get("/", [](const Request &req, Response &res) {
    res.set_content("version: " + Version::getVersion() + "\nsha256:  " + Version::getHash() + "\napi: " + apiVersion +
                        "\n" + Version::getLogo(),
                    "text/plain");
  });

  //
  // Start analyzes
  //
  std::string start = "/" + apiVersion + "/start";
  mServer.Post(start, [&](const Request &req, Response &res) {
    try {
      std::cout << req.body << std::endl;
      nlohmann::json object   = nlohmann::json::parse(req.body);
      std::string inputFolder = object["input_folder"];
      actProcessorUID         = joda::processor::ImageProcessorFactory::startProcessing(inputFolder, req.body);

      // actProcessor->wait();
      nlohmann::json retDoc;
      retDoc["status"]     = "running";
      retDoc["process_id"] = actProcessorUID;
      res.set_content(retDoc.dump(), "application/json");

    } catch(const std::exception &ex) {
      nlohmann::json retDoc;
      retDoc["status"] = "error";
      retDoc["code"]   = ex.what();
      res.status       = 500;
      res.set_content(retDoc.dump(), "application/json");
    }
  });

  //
  // Stop running analyzes
  //
  std::string stop = "/" + apiVersion + "/stop";
  mServer.Get(stop, [&](const Request &req, Response &res) {
    joda::processor::ImageProcessorFactory::stopProcess(actProcessorUID);
    nlohmann::json retDoc;
    retDoc["status"] = "stopping";
    res.set_content(retDoc.dump(), "application/json");
  });

  //
  // Get progress of running analyzing
  //
  std::string progress = "/" + apiVersion + "/progress";
  mServer.Get(progress, [&](const Request &req, Response &res) {
    nlohmann::json retDoc;

    try {
      auto [total, image] = joda::processor::ImageProcessorFactory::getProcess(actProcessorUID)->getProgress();
      retDoc["status"]    = "running";
      retDoc["actual_image"]["finished"] = image.finished;
      retDoc["actual_image"]["total"]    = image.total;
      retDoc["total"]["finished"]        = total.finished;
      retDoc["total"]["total"]           = total.total;
    } catch(const std::exception &ex) {
      retDoc["status"] = "stopped";
    }

    res.set_content(retDoc.dump(), "application/json");
  });

  mServer.listen("0.0.0.0", listenPort);
}

}    // namespace joda::api
