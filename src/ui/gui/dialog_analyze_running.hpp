///
/// \file      dialog_analyze_running.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qdialog.h>
#include <memory>
#include <thread>
#include "backend/settings/analze_settings.hpp"
#include "ui/gui/dialog_shadow/dialog_shadow.h"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

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

}    // namespace joda::ui::gui
