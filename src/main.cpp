#include <opencv2/core/hal/interface.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "image/image.hpp"
#include "image_reader/tif/image_loader_tif.hpp"
#include "image_reader/vsi/image_loader_vsi.hpp"
#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>

#include "algorithms/rolling_ball/rolling_ball.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"

////
#include <fstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace dnn;

////

void printProgress(double percentage);

int main(int argc, char **argv)
{
  TiffLoader::initLibTif();

  std::string imgName = "test/GMEV5minM1OT3_0001.btf";

  // convert("test/GMEV5minM1OT3_0001.btf", imgName);

  joda::reporting::Reporting reporting;
  auto nrOfTIles        = TiffLoader::getNrOfTiles(imgName, 14);
  int tilesToLoadPerRun = 36;
  int runs              = nrOfTIles / tilesToLoadPerRun;

  for(int n = 0; n < runs; n++) {
    try {
      auto tilePart = TiffLoader::loadImageTile(imgName, 14, n, tilesToLoadPerRun);
      joda::pipeline::NucleusCounter counter("out", &reporting);
      counter.analyzeImage(joda::Image{.mImage = tilePart, .mName = "ctrl_" + std::to_string(n)});
      float percent = (float) n / 200;
      printProgress(percent);
      std::cout << " " << std::to_string(n) << "/" << std::to_string(runs) << std::endl;
    } catch(...) {
    }
  }

  std::cout << "Found nuclues " << std::to_string(reporting.counter) << std::endl;

  return 0;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage)
{
  int val  = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush(stdout);
}
