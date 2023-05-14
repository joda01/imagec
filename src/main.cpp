

#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "algorithms/rolling_ball/rolling_ball.hpp"
#include "api/api.hpp"
#include "duration_count/duration_count.h"
#include "helper/termbox/termbox2.h"
#include "image/image.hpp"
#include "image_processor/image_processor.hpp"
#include "image_processor/image_processor_factory.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "image_reader/vsi/image_loader_vsi.hpp"
#include "navigation/navigation.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"
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

static constexpr int LISTENING_PORT = 7367;

///
/// \brief      Main method
/// \author     Joachim Danmayr
///
int main(int argc, char **argv)
{
  Version::initVersion(std::string(argv[0]));
  Updater updaterService(argc, argv);
  TiffLoader::initLibTif();
  joda::processor::ImageProcessorFactory::initProcessorFactory();

  joda::api::Api api;
  api.start(LISTENING_PORT);

  joda::processor::ImageProcessorFactory::shutdownFactory();

  // tb_init();
  // Navigation navigation(&updaterService);
  // navigation.start();
  // tb_shutdown();

  return 0;
}
