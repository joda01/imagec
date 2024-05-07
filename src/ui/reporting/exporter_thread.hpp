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

#pragma once

#include <qobject.h>
#include <QtWidgets>
#include <functional>
#include <memory>
#include <thread>
#include "backend/helper/directory_iterator.hpp"
#include "backend/results/results.hpp"

namespace joda::ui::qt {

///
/// \class
/// \author
/// \brief
///
class ReportingExporterThread : public QObject
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  ReportingExporterThread(QProgressBar *progressBar, QWidget *widgetToDeactivateDuringRuntime,
                          const std::filesystem::path &,
                          std::function<void(const results::WorkSheet &)> functionForOverview,
                          std::function<void(const results::WorkSheet &)> functionForImages);

  ~ReportingExporterThread();

signals:
  /////////////////////////////////////////////////////
  void signalFileFinished(int);

private:
  void workerThread();
  /////////////////////////////////////////////////////
  const std::filesystem::path &mImageCPackFile;
  std::shared_ptr<std::thread> mWorkerThread;
  QProgressBar *mProgressBar;
  QWidget *mWidgetToDeactivateDuringRuntime;
  std::function<void(const results::WorkSheet &)> mFunctionForOverview;
  std::function<void(const results::WorkSheet &)> mFunctionForImages;

private slots:
  /////////////////////////////////////////////////////
  void onFileFinished(int);
};

}    // namespace joda::ui::qt
