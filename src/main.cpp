

#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "duration_count/duration_count.h"
#include "functions/rolling_ball/rolling_ball.hpp"
#include "helper/termbox/termbox2.h"
#include "http/web_server.hpp"
#include "image/image.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "pipelines/pipeline_factory.hpp"
#include "reporting/report_printer.h"
#include "updater/updater.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include "version.h"

using namespace std;
using namespace cv;
using namespace dnn;

////

static constexpr int LISTENING_PORT_API = 7367;

///
/// \brief      Main method
/// \author     Joachim Danmayr
///
int main(int argc, char **argv)
{
  Version::initVersion(std::string(argv[0]));
  // joda::upd::Updater updaterService(argc, argv);
  TiffLoader::initLibTif();
  joda::pipeline::PipelineFactory::init();
  joda::http::HttpServer http;

  auto serverThread = std::thread(&joda::http::HttpServer::start, &http, LISTENING_PORT_API);
  while(!http.getServer().is_running()) {
    usleep(100000);
  }

  joda::log::logInfo("Server is listening on port " + std::to_string(LISTENING_PORT_API));
  joda::log::logInfo("Open imageC UI with http://localhost:" + std::to_string(LISTENING_PORT_API));
  joda::log::logInfo("API reachable under http://localhost:" + std::to_string(LISTENING_PORT_API) + "/api/v1");

  auto browserThread = std::thread([]() {
    int exitState = 0;
    joda::helper::execCommand("open http://127.0.0.1:7367", exitState);
  });

  serverThread.join();
  browserThread.join();

  joda::pipeline::PipelineFactory::shutdown();

  // tb_init();
  // Navigation navigation(&updaterService);
  // navigation.start();
  // tb_shutdown();

  return 0;
}
