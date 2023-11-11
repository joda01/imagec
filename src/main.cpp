

#include <opencv2/core/hal/interface.h>
#include <unistd.h>
#include <wx/app.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "backend/duration_count/duration_count.h"
#include "backend/image_reader/bioformats/bioformats_loader.hpp"
#include "backend/image_reader/tif/image_loader_tif.hpp"
#include "backend/pipelines/pipeline_factory.hpp"
#include "backend/reporting/report_printer.h"
#include "controller/controller.hpp"
#include "ui/wxwidgets/frame_main_controller.h"
#include "ui/wxwidgets/wxwidget.h"
#include "version.h"

#ifdef _WIN32
// #include "wx.rc"
// #include "wx/msw/wx.rc"
#include <windows.h>

#endif

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
  auto *controller = new joda::ctrl::Controller();
  wxSizerFlags::DisableConsistencyChecks();
  auto *frame = new joda::ui::wxwidget::FrameMainController(nullptr, controller);
  frame->Maximize();
  frame->Show(true);
  return true;
}

///
/// \brief      Main method
/// \author     Joachim Danmayr
///
int main(int argc, char **argv)
{
  std::cout << "Main" << std::endl;
#ifdef _WIN32
  const char *libraryPath = "./java/jre_win/bin/server/jvm.dll";
  int size_needed         = MultiByteToWideChar(CP_UTF8, 0, libraryPath, -1, NULL, 0);
  wchar_t *wlibraryPath   = new wchar_t[size_needed];
  MultiByteToWideChar(CP_UTF8, 0, libraryPath, -1, wlibraryPath, size_needed);
  HINSTANCE jvmDll = LoadLibrary(wlibraryPath);

  if(jvmDll == NULL) {
    std::cerr << "Failed to load jvm.dll" << std::endl;
    return 1;
  }
#endif

  wxInitAllImageHandlers();
  Version::initVersion(std::string(argv[0]));
  TiffLoader::initLibTif();
  BioformatsLoader::init();
  joda::pipeline::PipelineFactory::init();

  auto wxThread = std::thread([&]() {
    wxApp *app = new MainApp();
    wxApp::SetInstance(app);
    wxEntry(argc, argv);
  });

  wxThread.join();
  joda::pipeline::PipelineFactory::shutdown();
  BioformatsLoader::destroy();

  return 0;
}
