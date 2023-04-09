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
#include "duration_count/duration_count.h"
#include "image/image.hpp"
#include "image_processor/image_processor.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "image_reader/vsi/image_loader_vsi.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

////

void printLogo();
std::string readFolder(const std::string &text);

int main(int argc, char **argv)
{
  TiffLoader::initLibTif();

  printLogo();
  std::string inFolder  = readFolder("Input folder: ");
  std::string outFolder = readFolder("Output folder: ");

  joda::processor::ImageProcessor<::joda::pipeline::NucleusCounter> pc(inFolder, outFolder);
  pc.start();

  // std::cout << "Found nuclues " << std::to_string(reporting.counter) << std::endl;

  return 0;
}

std::string readFolder(const std::string &text)
{
  std::string inputFolder;
  std::cout << text;
  std::cin >> inputFolder;    // get user input from the keyboard
  return inputFolder;
}

///
/// \brief      Print logo imageC
/// \author     Joachim Danmayr
///
void printLogo()
{
  system("clear");
  std::cout << "\
  \n \
  (_)___ ___  ____ _____ ____  / ____/\n\
  / / __ `__ \\/ __ `/ __ `/ _ \\/ /     \n\
 / / / / / / / /_/ / /_/ /  __/ /___   \n\
/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/   \n\
                  /____/               \n\
                                       \
" << std::endl;
}
