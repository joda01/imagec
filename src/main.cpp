

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
#include "backend/duration_count/duration_count.h"
#include "backend/image/image.hpp"
#include "backend/image_reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/reporting/report_printer.h"
#include "ui/http/web_server.hpp"
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

#include "ui/wxwidgets/main_windows.hpp"

using namespace std;
using namespace cv;
using namespace dnn;

class MainApp : public wxApp
{
public:
  bool OnInit() override;
};

///
/// \brief This is the main method
///
wxIMPLEMENT_APP_NO_MAIN(MainApp);

///
/// \brief      This is the main class
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
bool MainApp::OnInit()
{
  MainFrame *frame = new MainFrame();
  frame->Show(true);
  return true;
}

///
/// \brief      Main method
/// \author     Joachim Danmayr
///
int main(int argc, char **argv)
{
  wxInitAllImageHandlers();
  Version::initVersion(std::string(argv[0]));
  TiffLoader::initLibTif();
  joda::pipeline::PipelineFactory::init();

  auto wxThread = std::thread([&]() {
    wxApp *app = new MainApp();
    wxApp::SetInstance(app);
    wxEntry(argc, argv);
  });

  wxThread.join();
  joda::pipeline::PipelineFactory::shutdown();

  return 0;
}
