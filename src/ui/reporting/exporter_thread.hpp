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
                          const joda::helper::DirectoryWatcher<FileInfo> &,
                          std::function<void(const std::filesystem::path &)> functionForOverview,
                          std::function<void(const std::filesystem::path &)> functionForImages);

  ~ReportingExporterThread();

signals:
  /////////////////////////////////////////////////////
  void signalFileFinished(int);

private:
  void workerThread();
  /////////////////////////////////////////////////////
  const joda::helper::DirectoryWatcher<FileInfo> &mDirWatcher;
  std::shared_ptr<std::thread> mWorkerThread;
  QProgressBar *mProgressBar;
  QWidget *mWidgetToDeactivateDuringRuntime;
  std::function<void(const std::filesystem::path &)> mFunctionForOverview;
  std::function<void(const std::filesystem::path &)> mFunctionForImages;

private slots:
  /////////////////////////////////////////////////////
  void onFileFinished(int);
};

}    // namespace joda::ui::qt