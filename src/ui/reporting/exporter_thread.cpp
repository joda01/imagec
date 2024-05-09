///
/// \file      exporter_thread.cpp
/// \author
/// \date      2024-05-05
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "exporter_thread.hpp"
#include <qtmetamacros.h>
#include <memory>
#include <string>
#include "backend/results/results.hpp"

namespace joda::ui::qt {

ReportingExporterThread::ReportingExporterThread(QProgressBar *progressBar, QWidget *widgetToDeactivateDuringRuntime,
                                                 const std::filesystem::path &packFile,
                                                 std::function<void(const results::WorkSheet &)> functionForOverview,
                                                 std::function<void(const results::WorkSheet &)> functionForImages) :
    mImageCPackFile(packFile),
    mProgressBar(progressBar), mWidgetToDeactivateDuringRuntime(widgetToDeactivateDuringRuntime),
    mFunctionForOverview(functionForOverview), mFunctionForImages(functionForImages)
{
  mWorkerThread = std::make_shared<std::thread>(&ReportingExporterThread::workerThread, this);
  widgetToDeactivateDuringRuntime->setEnabled(false);
  mProgressBar->setValue(0);
  mProgressBar->setVisible(true);
  connect(this, &ReportingExporterThread::signalFileFinished, this, &ReportingExporterThread::onFileFinished);
}

ReportingExporterThread::~ReportingExporterThread()
{
  if(mWorkerThread && mWorkerThread->joinable()) {
    mWorkerThread->join();
  }
}

///
/// \brief      Do the work
/// \author     Joachim Danmayr
///
void ReportingExporterThread::workerThread()
{
  static const std::string separator(1, std::filesystem::path::preferred_separator);
  mProgressBar->setRange(0, 100);
  int finished       = 0;
  auto files         = results::WorkBook::listResultsFiles(mImageCPackFile.string());
  uint32_t nrOfFiles = files.size();
  for(const auto &resultsFilePath : files) {
    if(resultsFilePath.starts_with(joda::results::RESULTS_SUMMARY_FILE_NAME)) {
      mFunctionForOverview(results::WorkBook::readWorksheetFromArchive(mImageCPackFile.string(), resultsFilePath));
    } else {
      // Detail view
      mFunctionForImages(results::WorkBook::readWorksheetFromArchive(mImageCPackFile.string(), resultsFilePath));
    }
    finished++;
    emit signalFileFinished((100 * finished) / nrOfFiles);
  }
  emit signalFileFinished(100);
}

void ReportingExporterThread::onFileFinished(int val)
{
  mProgressBar->setValue(val);
  if(val >= 100) {
    mWidgetToDeactivateDuringRuntime->setEnabled(true);
    mProgressBar->setVisible(false);
  }
}

}    // namespace joda::ui::qt
