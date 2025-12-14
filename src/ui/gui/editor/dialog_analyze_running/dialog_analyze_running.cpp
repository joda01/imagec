///
/// \file      dialog_analyze_running.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_analyze_running.hpp"
#include <qdialog.h>
#include <qicon.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <exception>
#include <memory>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/debugging.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

using namespace std::chrono_literals;

///
/// \brief
/// \author     Joachim Danmayr
///
DialogAnalyzeRunning::DialogAnalyzeRunning(WindowMain *windowMain, const joda::settings::AnalyzeSettings &settings,
                                           const std::optional<std::filesystem::path> &fileToAnalyze) :
    QDialog(windowMain),
    mWindowMain(windowMain), mSettings(settings), mFileToAnalyze(fileToAnalyze)
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
  CHECK_GUI_THREAD(closeButton)
  closeButton->setEnabled(false);

  closeAndOpenButton = new QPushButton("Open results", this);
  closeAndOpenButton->setObjectName("ToolButton");
  CHECK_GUI_THREAD(closeAndOpenButton)
  closeAndOpenButton->setEnabled(false);

  stopButton = new QPushButton("Stop", this);
  stopButton->setObjectName("ToolButton");
  CHECK_GUI_THREAD(stopButton)
  stopButton->setEnabled(true);

  QPushButton *openResultsFolder = new QPushButton(generateSvgIcon<Style::REGULAR, Color::BLACK>("arrow-square-out"), "", this);
  openResultsFolder->setObjectName("ToolButton");
  openResultsFolder->setToolTip("Open results folder");

  connect(closeButton, &QPushButton::clicked, this, &DialogAnalyzeRunning::onCloseClicked);
  connect(closeAndOpenButton, &QPushButton::clicked, this, &DialogAnalyzeRunning::onCloseAndOpenClicked);
  connect(stopButton, &QPushButton::clicked, this, &DialogAnalyzeRunning::onStopClicked);
  connect(openResultsFolder, &QPushButton::clicked, this, &DialogAnalyzeRunning::onOpenResultsFolderClicked);

  buttonLayout->addWidget(openResultsFolder);

  buttonLayout->addStretch();
  buttonLayout->addWidget(stopButton);
  buttonLayout->addWidget(closeButton);
  buttonLayout->addWidget(closeAndOpenButton);
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
  CHECK_GUI_THREAD(stopButton)
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

void DialogAnalyzeRunning::onCloseAndOpenClicked()
{
  mStopping = true;
  if(mRefreshThread && mRefreshThread->joinable()) {
    mRefreshThread->join();
  }
  mWindowMain->openResultsSettings(mWindowMain->getController()->getJobInformation()->resultsDatabaseFilePath.string().data());
  close();
}

void DialogAnalyzeRunning::onOpenResultsFolderClicked()
{
  QString folderPath = mWindowMain->getController()->getJobInformation()->resultsOutputFolder.string().data();
  QDesktopServices::openUrl(QUrl("file:///" + folderPath));
}

void DialogAnalyzeRunning::refreshThread()
{
  mWindowMain->getController()->start(mSettings, mWindowMain->getJobName().toStdString(), mFileToAnalyze);
  mStartedTime = std::chrono::system_clock::now();

  // Wait unit new pipeline has been started. It could be that we are still waiting for finishing the prev thread.
  while(!mStopped) {
    std::this_thread::sleep_for(500ms);
    emit refreshEvent();
  }

  // Wait unit finished
  while(true) {
    try {
      const auto &state = mWindowMain->getController()->getState();
      if(state.getState() == joda::processor::ProcessState::FINISHED || state.getState() == joda::processor::ProcessState::FINISHED_WITH_ERROR) {
        break;
      }
    } catch(const std::exception &ex) {
    }
    std::this_thread::sleep_for(500ms);
  }

  emit refreshEvent();
}

void DialogAnalyzeRunning::onRefreshData()
{
  QString newTextAllOver          = "Processing Image 0/0";
  QString newTextImage            = "Processing Tile 0/0";
  QString newTextPipelineProgress = "Processing Tile 0/0";

  auto actState = joda::processor::ProcessState::INITIALIZING;
  try {
    const auto &state = mWindowMain->getController()->getState();
    if(state.getState() == joda::processor::ProcessState::RUNNING || state.getState() == joda::processor::ProcessState::RUNNING_PREPARING_PIPELINE) {
      mEndedTime = std::chrono::system_clock::now();
    }
    actState                = state.getState();
    newTextAllOver          = QString("Processing Image %1/%2").arg(state.finishedImages()).arg(state.totalImages());
    newTextImage            = QString("Processing Tile %1/%2").arg(state.finishedTiles()).arg(state.totalTiles());
    newTextPipelineProgress = QString("Processing pipelines %1").arg(state.finishedPipelineSteps());

  } catch(const std::exception &ex) {
    joda::log::logWarning("Pipeline error: " + std::string(ex.what()));
  }
  double elapsedTimeMs = std::chrono::duration<double, std::milli>(mEndedTime - mStartedTime).count();
  auto [timeDiff, exp] = exponentForTime(elapsedTimeMs);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << timeDiff << " " << exp;
  std::string timeDiffStr = stream.str();

  QString progressText;
  if(actState == joda::processor::ProcessState::STOPPING) {
    CHECK_GUI_THREAD(stopButton)
    stopButton->setEnabled(false);
    mStopped = true;
    progressBar->setRange(0, 100);
    progressBar->setMaximum(100);
    progressBar->setMinimum(0);
    progressBar->setValue(100);
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Stopping ...";

  } else if(actState == joda::processor::ProcessState::FINISHED) {
    CHECK_GUI_THREAD(closeButton)
    closeButton->setEnabled(true);
    CHECK_GUI_THREAD(closeAndOpenButton)
    closeAndOpenButton->setEnabled(true);
    CHECK_GUI_THREAD(stopButton)
    stopButton->setEnabled(false);
    mStopped = true;
    progressBar->setRange(0, 100);
    progressBar->setMaximum(100);
    progressBar->setMinimum(0);
    progressBar->setValue(100);
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Finished ...";
  } else if(actState == joda::processor::ProcessState::FINISHED_WITH_ERROR) {
    CHECK_GUI_THREAD(closeButton)
    closeButton->setEnabled(true);
    CHECK_GUI_THREAD(closeAndOpenButton)
    closeAndOpenButton->setEnabled(true);
    CHECK_GUI_THREAD(stopButton)
    stopButton->setEnabled(false);
    progressBar->setRange(0, 100);
    progressBar->setMaximum(100);
    progressBar->setMinimum(0);
    progressBar->setValue(100);
    if(!mStopped) {
      QMessageBox messageBox(this);
      // Color red = #860000
      messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::RED>("warning-diamond").pixmap(48, 48));
      messageBox.setWindowTitle("Error...");
      messageBox.setText("Error in execution got >" + QString(mWindowMain->getController()->getState().what().data()) + "<.");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      messageBox.exec();
    }
    mStopped     = true;
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Finished ...";
  } else if(actState == joda::processor::ProcessState::RUNNING_PREPARING_PIPELINE) {
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>Preparing images ...";
  } else {
    progressText = "<html>" + newTextAllOver + "<br/>" + newTextImage + "<br/>" + newTextPipelineProgress;
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

}    // namespace joda::ui::gui
