///
/// \file      dialog_analyze_running.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qdialog.h>
#include <qtmetamacros.h>
#include <memory>
#include <thread>
#include "backend/settings/analze_settings.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/window_main/window_main.hpp"

namespace joda::ui::qt {

class DialogAnalyzeRunning : public DialogShadow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogAnalyzeRunning(WindowMain *windowMain, const joda::settings::AnalyzeSettings &settings);

signals:
  void refreshEvent();

private:
  /////////////////////////////////////////////////////
  void refreshThread();
  std::tuple<double, std::string> exponentForTime(double timeMs);

  std::shared_ptr<std::thread> mRefreshThread;
  bool mStopped  = false;
  bool mStopping = false;
  std::string mLastErrorMsg;
  std::chrono::system_clock::time_point mStartedTime;
  std::chrono::system_clock::time_point mEndedTime;
  std::mutex mRefreshMutex;

  QLabel *mProgressTitle;
  QLabel *mProgressText;
  QProgressBar *progressBar;
  QPushButton *closeButton;
  QPushButton *stopButton;
  WindowMain *mWindowMain;

  const joda::settings::AnalyzeSettings mSettings;

private slots:
  void onStopClicked();
  void onCloseClicked();
  void onRefreshData();
  void onOpenResultsFolderClicked();
};

}    // namespace joda::ui::qt
