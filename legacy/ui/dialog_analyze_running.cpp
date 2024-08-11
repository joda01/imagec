///
/// \file      dialog_analyze_running.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "dialog_analyze_running.hpp"
#include <qdialog.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <memory>
#include <thread>
#include "ui/dialog_shadow/dialog_shadow.h"

namespace joda::ui::qt {

using namespace std::chrono_literals;

///
/// \brief
/// \author     Joachim Danmayr
///
DialogAnalyzeRunning::DialogAnalyzeRunning(WindowMain *windowMain, const joda::settings::AnalyzeSettings &settings) :
    DialogShadow(windowMain, false), mStopped(false), mWindowMain(windowMain), mSettings(settings)
{
  //
  // Layout
  //
  // Set up the layout
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  //   // mainLayout->setContentsMargins(28, 28, 28, 28);

  QFont font;
  font.setBold(true);
  font.setPixelSize(16);
  mProgressTitle = new QLabel(this);
  mProgressTitle->setFont(font);
  mProgressTitle->setText("Processing Images...");
  mainLayout->addWidget(mProgressTitle);

  mProgressText = new QLabel(this);
  mProgressText->setText("<html>Processing Image ./.<br/>Processing Tile .../...<br/>Starting ...</html>");
  mainLayout->addWidget(mProgressText);

  // Create and set up the progress bar
  progressBar = new QProgressBar(this);
  progressBar->setRange(0, 0);
  progressBar->setMaximum(0);
  progressBar->setMinimum(0);
  mainLayout->addWidget(progressBar);
  mainLayout->addStretch();

  // Create and set up the buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout;
  closeButton               = new QPushButton("Close", this);
  closeButton->setObjectName("ToolButton");
  closeButton->setEnabled(false);
  stopButton = new QPushButton("Stop", this);
  stopButton->setObjectName("ToolButton");
  stopButton->setEnabled(true);

  QPushButton *openResultsFolder = new QPushButton(QIcon(":/icons/outlined/icons8-scatter-plot-50.png"), "", this);
  openResultsFolder->setObjectName("ToolButton");
  openResultsFolder->setToolTip("Open results folder");

  connect(closeButton, &QPushButton::clicked, this, &DialogAnalyzeRunning::onCloseClicked);
  connect(stopButton, &QPushButton::clicked, this, &DialogAnalyzeRunning::onStopClicked);
  connect(openResultsFolder, &QPushButton::clicked, this, &DialogAnalyzeRunning::onOpenResultsFolderClicked);

  buttonLayout->addWidget(openResultsFolder);

  buttonLayout->addStretch();
  buttonLayout->addWidget(stopButton);
  buttonLayout->addWidget(closeButton);
  mainLayout->addLayout(buttonLayout);

  setWindowTitle("Progress Dialog");
  setMinimumWidth(500);

  //
  // Start analyze
  //
  connect(this, &DialogAnalyzeRunning::refreshEvent, this, &DialogAnalyzeRunning::onRefreshData);
  mRefreshThread = std::make_shared<std::thread>(&DialogAnalyzeRunning::refreshThread, this);
}

void DialogAnalyzeRunning::onStopClicked()
{
  stopButton->setEnabled(false);
  mWindowMain->getController()->stop();
  mStopping = true;
}
void DialogAnalyzeRunning::onCloseClicked()
{
  mStopping = true;
  if(mRefreshThread && mRefreshThread->joinable()) {
    mRefreshThread->join();
  }
  close();
}

void DialogAnalyzeRunning::onOpenResultsFolderClicked()
{
  QString folderPath = mWindowMain->getController()->getOutputFolder().data();
  QDesktopServices::openUrl(QUrl("file:///" + folderPath));
}

void DialogAnalyzeRunning::refreshThread()
{
  auto threadSettings = mWindowMain->getController()->calcOptimalThreadNumber(mSettings, 0);
  mWindowMain->getController()->reset();
  mWindowMain->getController()->start(mSettings, threadSettings, mWindowMain->getJobName().toStdString());
  mStartedTime = std::chrono::high_resolution_clock::now();

  while(!mStopped) {
    emit refreshEvent();
    std::this_thread::sleep_for(500ms);
  }

  // Wait unit finished
  while(true) {
    auto [progress, state, errorMsg] = mWindowMain->getController()->getState();
    if(state == joda::pipeline::Pipeline::State::FINISHED) {
      break;
    }
    std::this_thread::sleep_for(500ms);
  }

  emit refreshEvent();
}

void DialogAnalyzeRunning::onRefreshData()
{
  QString newTextAllOver                   = "Processing Image 0/0";
  QString newTextImage                     = "Processing Tile 0/0";
  joda::pipeline::Pipeline::State actState = joda::pipeline::Pipeline::State::STOPPED;
  try {
    auto [progress, state, errorMsg] = mWindowMain->getController()->getState();
    if(state == joda::pipeline::Pipeline::State::ERROR_) {
      mLastErrorMsg = errorMsg;
    }
    if(state == joda::pipeline::Pipeline::State::RUNNING) {
      mEndedTime = std::chrono::high_resolution_clock::now();
    }
    actState = state;

    newTextAllOver = QString("Processing Image %1/%2").arg(progress.total.finished).arg(progress.total.total);
    newTextImage   = QString("Processing Tile %1/%2").arg(progress.image.finished).arg(progress.image.total);

    // progressBar->setMaximum(progress.total.total);
    // if(progress.total.finished <= progress.total.total) {
    //   progressBar->setValue(progress.total.finished);
    // }

  } catch(const std::exception &ex) {
    onStopClicked();
  }
  double elapsedTimeMs = std::chrono::duration<double, std::milli>(mEndedTime - mStartedTime).count();
  auto [timeDiff, exp] = exponentForTime(elapsedTimeMs);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << timeDiff << " " << exp;
  std::string timeDiffStr = stream.str();

  if(!mStopped && actState == joda::pipeline::Pipeline::State::ERROR_) {
    mStopped = true;
    // showErrorDialog(mLastErrorMsg);
  }
  QString progressText;
  if(actState != joda::pipeline::Pipeline::State::RUNNING || actState == joda::pipeline::Pipeline::State::STOPPED) {
    stopButton->setEnabled(false);
    mStopped = true;
    progressBar->setRange(0, 100);
    progressBar->setMaximum(100);
    progressBar->setMinimum(0);
    progressBar->setValue(100);
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Stopping ...";

  } else {
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage;
  }

  if(actState == joda::pipeline::Pipeline::State::FINISHED || actState == joda::pipeline::Pipeline::State::ERROR_) {
    closeButton->setEnabled(true);
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Finished ...";
  }
  mProgressText->setText(progressText);

  // mLabelReporting->SetLabel(timeDiffStr);
}

///
/// \brief      Finds the best exponent view
/// \author     Joachim Danmayr
///
std::tuple<double, std::string> DialogAnalyzeRunning::exponentForTime(double timeMs)
{
  if(timeMs > 1e3 * 60) {
    return {timeMs / (1e3 * 60), " min"};
  }

  if(timeMs > 1e3) {
    return {timeMs / 1e3, " s"};
  }
  return {timeMs, " ms"};
}

}    // namespace joda::ui::qt
