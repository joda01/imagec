///
/// \file      window_main.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "window_main.hpp"
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QAction>
#include <QIcon>
#include <QMainWindow>
#include <QToolBar>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include "backend/settings/channel_settings.hpp"
#include "backend/settings/pipeline_settings.hpp"
#include "ui/qt/dialog_analyze_running.hpp"
#include "build_info.h"
#include "container_channel.hpp"
#include "version.h"

namespace joda::ui::qt {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller) : mController(controller)
{
  setWindowTitle("imageC");
  createToolbar();
  setMinimumSize(1300, 800);
  setObjectName("windowMain");
  setStyleSheet(
      "QMainWindow#windowMain {"
      "   background-color: rgb(251, 252, 253); "
      "   border: none;"
      "}");
  setCentralWidget(createStackedWidget());

  // Start with the main page
  onBackClicked();

  mMainThread = new std::thread(&WindowMain::waitForFileSearchFinished, this);
  connect(this, &WindowMain::lookingForFilesFinished, this, &WindowMain::onLookingForFilesFinished);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createToolbar()
{
  auto *toolbar = addToolBar("toolbar");
  toolbar->setMovable(false);
  toolbar->setStyleSheet("QToolBar {background-color: rgb(251, 252, 253); border: 0px; border-bottom: 0px;}");

  // Left
  {
    mBackButton = new QAction(QIcon(":/icons/outlined/icons8-left-50.png"), "Back", toolbar);
    mBackButton->setEnabled(false);
    connect(mBackButton, &QAction::triggered, this, &WindowMain::onBackClicked);
    toolbar->addAction(mBackButton);
    mFirstSeparator = toolbar->addSeparator();

    mSaveProject = new QAction(QIcon(":/icons/outlined/icons8-save-50.png"), "Save", toolbar);
    mSaveProject->setToolTip("Save project!");
    connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProjectClicked);
    toolbar->addAction(mSaveProject);

    mOPenProject = new QAction(QIcon(":/icons/outlined/icons8-folder-50.png"), "Open", toolbar);
    mOPenProject->setToolTip("Open folder!");
    connect(mOPenProject, &QAction::triggered, this, &WindowMain::onOpenProjectClicked);
    toolbar->addAction(mOPenProject);

    mStartAnalysis = new QAction(QIcon(":/icons/outlined/icons8-play-50.png"), "Start", toolbar);
    mStartAnalysis->setEnabled(false);
    mStartAnalysis->setToolTip("Start analysis!");
    connect(mStartAnalysis, &QAction::triggered, this, &WindowMain::onStartClicked);
    toolbar->addAction(mStartAnalysis);
    mSecondSeparator = toolbar->addSeparator();

    mDeleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolbar);
    mDeleteChannel->setToolTip("Delete channel!");
    connect(mDeleteChannel, &QAction::triggered, this, &WindowMain::onRemoveChannelClicked);
    toolbar->addAction(mDeleteChannel);
  }

  {
    // Add a spacer to push the next action to the middle
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->setMaximumHeight(32);
    toolbar->addWidget(spacerWidget);
  }

  // Middle
  {
    // Add the QComboBox in the middle
    mFoundFilesCombo = new QComboBox(toolbar);
    mFoundFilesCombo->setMinimumWidth(250);
    mFoundFilesCombo->setMaximumWidth(300);
    mFileSelectorComboBox = toolbar->addWidget(mFoundFilesCombo);
    mFileSelectorComboBox->setVisible(false);

    mImageSeriesCombo = new QComboBox(toolbar);
    mImageSeriesCombo->addItem("Series 0", 0);
    mImageSeriesCombo->addItem("Series 1", 1);
    mImageSeriesCombo->addItem("Series 2", 2);
    mImageSeriesCombo->addItem("Series 3", 3);
    mImageSeriesComboBox = toolbar->addWidget(mImageSeriesCombo);
    mImageSeriesComboBox->setVisible(false);

    mFoundFilesHint = new ClickableLabel(toolbar);
    mFoundFilesHint->setText("Please open a working directory ...");
    mFileSearchHintLabel = toolbar->addWidget(mFoundFilesHint);
    connect(mFoundFilesHint, &ClickableLabel::clicked, this, &WindowMain::onOpenProjectClicked);
  }

  // Right
  {
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(spacerWidget);
  }

  toolbar->addSeparator();

  {
    mSettings = new QAction(QIcon(":/icons/outlined/icons8-settings-50.png"), "Settings", toolbar);
    mSettings->setToolTip("Settings");
    connect(mSettings, &QAction::triggered, this, &WindowMain::onOpenProjectClicked);
    toolbar->addAction(mSettings);

    toolbar->addSeparator();

    mShowInfoDialog = new QAction(QIcon(":/icons/outlined/icons8-info-50.png"), "Info", toolbar);
    mShowInfoDialog->setToolTip("Info");
    connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);
    toolbar->addAction(mShowInfoDialog);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createStackedWidget()
{
  mStackedWidget = new QStackedWidget();
  mStackedWidget->setObjectName("stackedWidget");
  mStackedWidget->addWidget(createOverviewWidget());
  mStackedWidget->addWidget(createChannelWidget());
  return mStackedWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createOverviewWidget()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setFrameStyle(0);
  scrollArea->setObjectName("scrollAreaOverview");
  scrollArea->setStyleSheet("QScrollArea#scrollAreaOverview { background-color: rgb(251, 252, 253);}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setObjectName("mainWindowHLayout");
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);

  auto createVerticalContainer = []() -> std::tuple<QGridLayout *, QWidget *> {
    QWidget *contentWidget = new QWidget;
    QGridLayout *layout    = new QGridLayout(contentWidget);
    layout->setObjectName("mainWindowGridLayout");
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);    // Adjust this value as needed
    contentWidget->setLayout(layout);
    return {layout, contentWidget};
  };

  {
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    mLayoutChannelOverview                                = channelsOverViewLayout;

    channelsOverViewLayout->addWidget(createAddChannelPanel());
    mLastElement = new QLabel();
    channelsOverViewLayout->addWidget(mLastElement, 1, 0, 1, 3);

    channelsOverViewLayout->setRowStretch(0, 1);
    channelsOverViewLayout->setRowStretch(1, 1);
    channelsOverViewLayout->setRowStretch(2, 1);
    channelsOverViewLayout->setRowStretch(4, 3);

    // channelsOverViewLayout->addStretch();

    horizontalLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
  }

  {
    /*
    auto [channelsOverViewLayout, channelsOverviewWidget] = createVerticalContainer();
    PanelChannelOverview *panel1                          = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel1);
    PanelChannelOverview *panel2 = new PanelChannelOverview();
    channelsOverViewLayout->addWidget(panel2);
    channelsOverViewLayout->addStretch();
    horizontalLayout->addWidget(channelsOverviewWidget);
    */
  }

  horizontalLayout->addStretch();

  return scrollArea;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelWidget()
{
  return new QWidget(this);
}

QWidget *WindowMain::createAddChannelPanel()
{
  QWidget *addChannelWidget = new QWidget();
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  addChannelWidget->setObjectName("PanelChannelOverview");
  addChannelWidget->setMinimumHeight(250);
  addChannelWidget->setMinimumWidth(350);
  addChannelWidget->setMaximumWidth(350);
  QVBoxLayout *layout = new QVBoxLayout(); /*this*/
  layout->setObjectName("mainWindowChannelGridLayout");
  addChannelWidget->setStyleSheet(
      "QWidget#PanelChannelOverview { border-radius: 12px; border: 2px solid rgba(0, 104, 117, 0.05); padding-top: "
      "10px; "
      "padding-bottom: 10px;"
      "background-color: rgba(0, 104, 117, 0);}");

  addChannelWidget->setLayout(layout);
  layout->setSpacing(0);

  //
  // Add channel
  //
  QPushButton *addChannelButton = new QPushButton();
  addChannelButton->setStyleSheet(
      "QPushButton {"
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 1px solid rgb(111, 121, 123);"
      "   color: rgb(0, 104, 117);"
      "   padding: 10px 20px;"
      "   border-radius: 12px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "}"

      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
      "}"

      "QPushButton:pressed {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
      "}");
  addChannelButton->setText("Add Channel");
  connect(addChannelButton, &QPushButton::pressed, this, &WindowMain::onAddChannelClicked);
  layout->addWidget(addChannelButton);

  //
  // Open settings
  //
  QPushButton *openSettingsButton = new QPushButton();
  openSettingsButton->setStyleSheet(
      "QPushButton {"
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 1px solid rgb(111, 121, 123);"
      "   color: rgb(0, 104, 117);"
      "   padding: 10px 20px;"
      "   border-radius: 12px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "}"

      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
      "}"

      "QPushButton:pressed {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
      "}");
  openSettingsButton->setText("Open settings");
  connect(openSettingsButton, &QPushButton::pressed, this, &WindowMain::onOpenSettingsClicked);
  layout->addWidget(openSettingsButton);

  //
  // Open template
  //
  QPushButton *openTemplate = new QPushButton();
  openTemplate->setStyleSheet(
      "QPushButton {"
      "   background-color: rgba(0, 0, 0, 0);"
      "   border: 1px solid rgb(111, 121, 123);"
      "   color: rgb(0, 104, 117);"
      "   padding: 10px 20px;"
      "   border-radius: 12px;"
      "   font-size: 14px;"
      "   font-weight: normal;"
      "   text-align: center;"
      "   text-decoration: none;"
      "}"

      "QPushButton:hover {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on hover
      "}"

      "QPushButton:pressed {"
      "   background-color: rgba(0, 0, 0, 0);"    // Darken on press
      "}");
  openTemplate->setText("Open template");
  connect(openTemplate, &QPushButton::pressed, this, &WindowMain::onOpenTemplateClicked);
  layout->addWidget(openTemplate);

  layout->setSpacing(4);    // Adjust this value as needed
  layout->addStretch();
  addChannelWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  mAddChannelPanel = addChannelWidget;
  return addChannelWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenProjectClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }
  QString selectedDirectory = QFileDialog::getExistingDirectory(this, "Select a directory", folderToOpen);

  if(selectedDirectory.isEmpty()) {
    return;
  }
  setWorkingDirectory(selectedDirectory.toStdString());
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenSettingsClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }
  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", QDir::homePath(), "JSON Files (*.json);;All Files (*)");

  if(filePath.isEmpty()) {
    return;
  }

  try {
    settings::json::AnalyzeSettings settings;
    settings.loadConfigFromFile(filePath.toStdString());
    fromJson(settings);
  } catch(const std::exception &ex) {
    if(mSelectedChannel != nullptr) {
      QMessageBox messageBox(this);
      auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
      messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
      messageBox.setIconPixmap(icon->pixmap(42, 42));
      messageBox.setWindowTitle("Could not load settings!");
      messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      auto reply = messageBox.exec();
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenTemplateClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedWorkingDirectory.isEmpty()) {
    folderToOpen = mSelectedWorkingDirectory;
  }
  QString filePath =
      QFileDialog::getOpenFileName(this, "Open template", "./templates", "JSON Files (*.json);;All Files (*)");

  if(filePath.isEmpty()) {
    return;
  }

  try {
    settings::json::ChannelSettings settings;
    settings.loadConfigFromFile(filePath.toStdString());
    auto *newChannel = addChannel();

    std::optional<joda::settings::json::PipelineStepCellApproximation> cellApprox;
    if(settings.getChannelInfo().getType() == settings::json::ChannelInfo::Type::NUCLEUS) {
      cellApprox = joda::settings::json::PipelineStepCellApproximation{.nucleus_channel_index =
                                                                           settings.getChannelInfo().getChannelIndex(),
                                                                       .cell_channel_index = -1,
                                                                       .max_cell_radius    = 500};
    }

    newChannel->fromJson(settings, cellApprox, std::nullopt, std::nullopt);
  } catch(const std::exception &ex) {
    if(mSelectedChannel != nullptr) {
      QMessageBox messageBox(this);
      auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
      messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
      messageBox.setIconPixmap(icon->pixmap(42, 42));
      messageBox.setWindowTitle("Could not load settings!");
      messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      auto reply = messageBox.exec();
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::setWorkingDirectory(const std::string &workingDir)
{
  mSelectedWorkingDirectory = workingDir.data();

  std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
  mFoundFilesHint->setText("Looking for images ...");
  mFoundFilesCombo->clear();
  mFileSelectorComboBox->setVisible(false);
  mImageSeriesComboBox->setVisible(false);
  mFileSearchHintLabel->setVisible(true);
  mController->setWorkingDirectory(mSelectedWorkingDirectory.toStdString());
  mNewFolderSelected = true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::waitForFileSearchFinished()
{
  while(true) {
    while(true) {
      {
        std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
        if(mNewFolderSelected) {
          break;
        }
      }
      std::this_thread::sleep_for(2s);
    }
    while(mController->isLookingForFiles()) {
      std::this_thread::sleep_for(500ms);
    }
    {
      std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
      mNewFolderSelected = false;
    }
    emit lookingForFilesFinished();
  }
}

///
/// \brief      Generate JSON document
/// \author     Joachim Danmayr
///
nlohmann::json WindowMain::toJson()
{
  nlohmann::json jsonSettings;

  nlohmann::json channelsArray     = nlohmann::json::array();    // Initialize an empty JSON array
  nlohmann::json pipelineStepArray = nlohmann::json::array();    // Initialize an empty JSON array

  ContainerChannel::IntersectionSettings intersectionChannels;
  std::map<int32_t, float> minIntersectionPerGroup;

  for(const auto &ch : mChannels) {
    auto converter = ch->toJson();
    if(!converter.channelSettings.empty()) {
      channelsArray.push_back(converter.channelSettings);
    }
    if(!converter.pipelineStep.empty()) {
      pipelineStepArray.push_back(converter.pipelineStep);
    }

    for(const auto &[group, ch] : converter.intersection) {
      if(intersectionChannels.contains(group)) {
        intersectionChannels.at(group).channel.insert(ch.channel.begin(), ch.channel.end());
        // There may be differences in the min intersections even if there should not be.
        // Use the biggest one if there is a difference.
        if(minIntersectionPerGroup.at(group) < ch.minIntersect) {
          minIntersectionPerGroup[group] = ch.minIntersect;
        }
      } else {
        intersectionChannels.emplace(group, ch);
        minIntersectionPerGroup.emplace(group, ch.minIntersect);
      }
    }
  }

  // Intersection settings
  for(const auto &[group, intersectGroup] : intersectionChannels) {
    nlohmann::json colocPipelineStep;
    colocPipelineStep["intersection"]["min_intersection"] = minIntersectionPerGroup[group];
    colocPipelineStep["intersection"]["channel_index"]    = intersectGroup.channel;
    pipelineStepArray.push_back(colocPipelineStep);
  }

  jsonSettings["input_folder"]                    = static_cast<std::string>(mSelectedWorkingDirectory.toStdString());
  jsonSettings["channels"]                        = channelsArray;
  jsonSettings["pipeline_steps"]                  = pipelineStepArray;
  jsonSettings["options"]["pixel_in_micrometer"]  = 1;
  jsonSettings["options"]["with_control_images"]  = true;
  jsonSettings["options"]["with_detailed_report"] = true;

  return jsonSettings;
}

///
/// \brief      Generate JSON document
/// \author     Joachim Danmayr
///
void WindowMain::fromJson(const settings::json::AnalyzeSettings &settings)
{
  // Remove all channels
  std::set<ContainerChannel *> channelsToDelete = mChannels;
  for(auto *const channel : channelsToDelete) {
    removeChannel(channel);
  }
  channelsToDelete.clear();

  // Load functions
  std::map<int32_t, const joda::settings::json::PipelineStepCellApproximation *> cellApproxMap;
  std::vector<const joda::settings::json::PipelineStepIntersection *> intersectionMap;
  for(const auto &pipelineStep : settings.getPipelineSteps()) {
    if(pipelineStep.getCellApproximation() != nullptr) {
      cellApproxMap.emplace(pipelineStep.getCellApproximation()->nucleus_channel_index,
                            pipelineStep.getCellApproximation());
    }
    if(pipelineStep.getIntersection() != nullptr) {
      intersectionMap.push_back(pipelineStep.getIntersection());
    }
  }

  std::map<int32_t, ContainerChannel::IntersectionRead> intersectReadVal;
  int intersectionGroup = 0;
  for(const auto &intersect : intersectionMap) {
    for(const auto &chIdx : intersect->channel_index) {
      intersectReadVal.emplace(chIdx,
                               ContainerChannel::IntersectionRead{.intersectionGroup = intersectionGroup,
                                                                  .minColocFactor    = intersect->min_intersection});
    }
    intersectionGroup++;
  }

  int series = 0;
  // Load channels
  for(const auto &[_, channel] : settings.getChannelsOrderedByChannelIndex()) {
    series             = channel.getChannelInfo().getChannelSeries();
    auto *channelAdded = addChannel();
    if(nullptr != channelAdded) {
      std::optional<joda::settings::json::PipelineStepCellApproximation> cellApprox;
      if(cellApproxMap.contains(channel.getChannelInfo().getChannelIndex())) {
        cellApprox = *cellApproxMap.at(channel.getChannelInfo().getChannelIndex());
      }

      std::optional<ContainerChannel::IntersectionRead> channelIntersection;
      std::optional<ContainerChannel::IntersectionRead> cellApproxIntersection;
      if(intersectReadVal.contains(channel.getChannelInfo().getChannelIndex())) {
        channelIntersection = intersectReadVal.at(channel.getChannelInfo().getChannelIndex());
      }
      if(intersectReadVal.contains(channel.getChannelInfo().getChannelIndex() +
                                   settings::json::PipelineStepSettings::CELL_APPROX_INDEX_OFFSET)) {
        cellApproxIntersection = intersectReadVal.at(channel.getChannelInfo().getChannelIndex() +
                                                     settings::json::PipelineStepSettings::CELL_APPROX_INDEX_OFFSET);
      }

      channelAdded->fromJson(channel, cellApprox, channelIntersection, cellApproxIntersection);
    }
  }

  mImageSeriesCombo->setCurrentIndex(series);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onLookingForFilesFinished()
{
  int idx = 0;
  for(const auto &file : mController->getListOfFoundImages()) {
    mFoundFilesCombo->addItem(QString(file.getFilename().data()), idx);
    idx++;
  }
  if(mController->getNrOfFoundImages() > 0) {
    mFoundFilesCombo->setCurrentIndex(0);
    mFoundFilesHint->setText("Finished");
    mFileSearchHintLabel->setVisible(false);
    mFileSelectorComboBox->setVisible(true);
    mImageSeriesComboBox->setVisible(true);
    mStartAnalysis->setEnabled(true);
  } else {
    // mFoundFilesCombo->setVisible(false);
    mFoundFilesHint->setText("No images found!");
    mStartAnalysis->setEnabled(false);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProjectClicked()
{
  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", QDir::homePath(), "JSON Files (*.json);;All Files (*)");

  if(!filePath.isEmpty()) {
    joda::settings::json::AnalyzeSettings settings;
    settings.loadConfigFromString(toJson().dump(2));
    std::string path = filePath.toStdString();
    if(!path.ends_with(".json")) {
      path += ".json";
    }
    settings.storeConfigToFile(path);
  }
}

///
/// \brief      Two channels in the same coloc group are not allowed to have different min intersection
/// \author     Joachim Danmayr
///
void WindowMain::syncColocSettings()
{
  if(mSelectedChannel != nullptr) {
    auto [group, factor] = mSelectedChannel->getMinColocFactor();
    for(const auto &ch : mChannels) {
      ch->setMinColocFactor(group, factor);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onStartClicked()
{
  DialogAnalyzeRunning dialg(this);
  dialg.exec();
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onBackClicked()
{
  mBackButton->setEnabled(false);
  mSaveProject->setVisible(true);
  mSaveProject->setVisible(true);
  mOPenProject->setVisible(true);
  mStartAnalysis->setVisible(true);
  mDeleteChannel->setVisible(false);
  mFirstSeparator->setVisible(true);
  mSecondSeparator->setVisible(true);
  syncColocSettings();
  mStackedWidget->setCurrentIndex(0);
  if(mSelectedChannel != nullptr) {
    mSelectedChannel->stopPreviewGeneration();
    mSelectedChannel = nullptr;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showChannelEdit(ContainerChannel *selectedChannel)
{
  mSelectedChannel = selectedChannel;

  mBackButton->setEnabled(true);
  mSaveProject->setVisible(false);
  mSaveProject->setVisible(false);
  mOPenProject->setVisible(false);
  mStartAnalysis->setVisible(false);
  mDeleteChannel->setVisible(true);
  mFirstSeparator->setVisible(false);
  mSecondSeparator->setVisible(false);

  mStackedWidget->removeWidget(mStackedWidget->widget(1));
  mStackedWidget->addWidget(selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(1);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onRemoveChannelClicked()
{
  if(mSelectedChannel != nullptr) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // messageBox.setAttribute(Qt::WA_TranslucentBackground);
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Remove channel?");
    messageBox.setText("Do you want to remove the channel?");
    messageBox.addButton(tr("No"), QMessageBox::NoRole);
    messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    auto reply = messageBox.exec();
    if(reply == 1) {
      removeChannel(mSelectedChannel);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
ContainerChannel *WindowMain::addChannel()
{
  if(mAddChannelPanel != nullptr) {
    {
      int row = (mChannels.size() + 1) / 3;
      int col = (mChannels.size() + 1) % 3;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
    }

    int row     = mChannels.size() / 3;
    int col     = mChannels.size() % 3;
    auto panel1 = new ContainerChannel(this);
    mLayoutChannelOverview->addWidget(panel1->getOverviewPanel(), row, col);
    mChannels.emplace(panel1);
    return panel1;
  }
  return nullptr;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onAddChannelClicked()
{
  addChannel();
}

void WindowMain::removeChannel(ContainerChannel *toRemove)
{
  /// \todo reorder
  if(toRemove != nullptr) {
    mChannels.erase(toRemove);

    mLayoutChannelOverview->removeWidget(toRemove->getOverviewPanel());
    toRemove->getOverviewPanel()->setParent(nullptr);
    delete toRemove;

    // Reorder all panels
    int cnt = 0;
    for(const auto &panelToReorder : mChannels) {
      mLayoutChannelOverview->removeWidget(panelToReorder->getOverviewPanel());
      int row = (cnt) / 3;
      int col = (cnt) % 3;
      mLayoutChannelOverview->addWidget(panelToReorder->getOverviewPanel(), row, col);
      cnt++;
    }

    {
      int row = (mChannels.size()) / 3;
      int col = (mChannels.size()) % 3;
      mLayoutChannelOverview->removeWidget(mAddChannelPanel);
      mLayoutChannelOverview->removeWidget(mLastElement);
      mLayoutChannelOverview->addWidget(mAddChannelPanel, row, col);
      mLayoutChannelOverview->addWidget(mLastElement, row + 1, 0, 1, 3);
    }
    mSelectedChannel = nullptr;
    onBackClicked();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::onShowInfoDialog()
{
  QMessageBox messageBox(this);
  auto *icon = new QIcon(":/icons/outlined/icons8-info-50-blue.png");

  messageBox.setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  messageBox.setIconPixmap(icon->pixmap(42, 42));
  // messageBox.setAttribute(Qt::WA_TranslucentBackground);
  // messageBox.setIconPixmap(icon->pixmap(42, 42));
  messageBox.setWindowTitle("Info");
  messageBox.setText(
      "<p style=\"text-align: left;\"><strong>imageC " + QString(Version::getVersion().data()) + " (" +
      QString(Version::getBuildTime().data()) +
      ")</strong></p>"
      "<p style=\"text-align: left;\"><em>Licensed under GPL-v3<br />Preferable for use in the non-profit research "
      "environment.</em></p>"
      "<p style=\"text-align: left;\"><strong>Many thanks</strong> for help in setting this project to Melanie "
      "Schuerz</p>"
      "<p style=\"text-align: left;\"><strong>Thank you very much for your help in training the AI models</strong><br "
      "/>Melanie Schuerz, Anna Mueller, Tanja Plank, Maria Jaritsch, Heloisa Melobenirschke and Patricia Hrasnova</p>"
      "<p style=\"text-align: left;\"><em>Icons from <a href=\"https://icons8.com/\">https://icons8.com/</a> and "
      "Dominik Handl</em></p>"
      "<p style=\"text-align: left;\">copyright 2022-2023 Joachim Danmayr</p>");
  QFont font;
  font.setPixelSize(10);
  messageBox.setFont(font);
  messageBox.addButton(tr("Close"), QMessageBox::AcceptRole);
  messageBox.exec();
}

}    // namespace joda::ui::qt