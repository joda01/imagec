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
#include <qlabel.h>
#include <qprogressbar.h>
#include <memory>
#include <thread>
#include "backend/settings/analze_settings.hpp"

namespace joda::ui::gui {

class WindowMain;

class DialogAnalyzeRunning : public QDialog
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogAnalyzeRunning(WindowMain *windowMain, const joda::settings::AnalyzeSettings &settings,
                       const std::optional<std::filesystem::path> &fileToAnalyze);

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
  QPushButton *closeAndOpenButton;
  QPushButton *stopButton;
  WindowMain *mWindowMain;

  const joda::settings::AnalyzeSettings mSettings;
  const std::optional<std::filesystem::path> mFileToAnalyze;

private slots:
  void onStopClicked();
  void onCloseAndOpenClicked();
  void onCloseClicked();
  void onRefreshData();
  void onOpenResultsFolderClicked();
};

}    // namespace joda::ui::gui
