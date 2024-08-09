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
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
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
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/helper/username.hpp"
#include "backend/pipelines/processor/image_processor.hpp"
#include "backend/results/results.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/channel/channel_settings.hpp"
#include "backend/settings/settings.hpp"
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include "backend/settings/vchannel/vchannel_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include "ui/container/channel/container_channel.hpp"
#include "ui/container/giraf/container_giraf.hpp"
#include "ui/container/intersection/container_intersection.hpp"
#include "ui/container/voronoi/container_voronoi.hpp"
#include "ui/dialog_analyze_running.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include "ui/reporting/panel_reporting.hpp"
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
#include "build_info.h"
#include "version.h"

namespace joda::ui::qt {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller) : mController(controller)
{
  const QIcon myIcon(":/icons/outlined/icon.png");
  setWindowIcon(myIcon);
  setWindowTitle(Version::getTitle().data());
  createTopToolbar();
  createBottomToolbar();
  createLeftToolbar();
  setMinimumSize(1600, 800);
  setObjectName("windowMain");
  setCentralWidget(createStackedWidget());

  // Start with the main page
  showProjectOverview();

  mMainThread = new std::thread(&WindowMain::waitForFileSearchFinished, this);
  connect(this, &WindowMain::lookingForFilesFinished, this, &WindowMain::onLookingForFilesFinished);
  connect(this, &WindowMain::lookingForTemplateFinished, this, &WindowMain::onFindTemplatesFinished);
  connect(getFoundFilesCombo(), &QComboBox::currentIndexChanged, this, &WindowMain::onImageSelectionChanged);
  connect(getImageSeriesCombo(), &QComboBox::currentIndexChanged, this, &WindowMain::onImageSelectionChanged);
}

void WindowMain::setWindowTitlePrefix(const QString &txt)
{
  if(!txt.isEmpty()) {
    setWindowTitle(QString(Version::getTitle().data()) + " - " + txt);
  } else {
    setWindowTitle(QString(Version::getTitle().data()));
  }
}

void WindowMain::createBottomToolbar()
{
  mButtomToolbar = new QToolBar(this);
  mButtomToolbar->setMovable(true);
  // Middle

  {
    // Add a spacer to push the next action to the middle
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mButtomToolbar->addWidget(spacerWidget);
  }

  // Add the QComboBox in the middle
  mFoundFilesCombo = new QComboBox(mButtomToolbar);
  mFoundFilesCombo->setMinimumWidth(250);
  mFoundFilesCombo->setMaximumWidth(300);
  mFileSelectorComboBox = mButtomToolbar->addWidget(mFoundFilesCombo);
  mFileSelectorComboBox->setVisible(false);

  mImageSeriesCombo = new QComboBox(mButtomToolbar);
  mImageSeriesCombo->addItem("Series 0", 0);
  mImageSeriesCombo->addItem("Series 1", 1);
  mImageSeriesCombo->addItem("Series 2", 2);
  mImageSeriesCombo->addItem("Series 3", 3);
  mImageSeriesComboBox = mButtomToolbar->addWidget(mImageSeriesCombo);
  mImageSeriesComboBox->setVisible(false);

  mImageResolutionCombo = new QComboBox(mButtomToolbar);
  mImageResolutionCombo->addItem("Resolution 0", 0);
  mImageResolutionComboBox = mButtomToolbar->addWidget(mImageResolutionCombo);
  mImageResolutionComboBox->setVisible(false);

  mFoundFilesHint = new ClickableLabel(mButtomToolbar);
  mFoundFilesHint->setText("Please open a working directory ...");
  mFileSearchHintLabel = mButtomToolbar->addWidget(mFoundFilesHint);

  addToolBar(Qt::ToolBarArea::BottomToolBarArea, mButtomToolbar);

  // Right
  {
    QWidget *spacerWidget = new QWidget();
    spacerWidget->setContentsMargins(0, 0, 0, 0);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mButtomToolbar->addWidget(spacerWidget);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createTopToolbar()
{
  auto *toolbar = addToolBar("toolbar");
  toolbar->setMovable(true);

  mBackButton = new QAction(QIcon(":/icons/outlined/icons8-left-50.png"), "Back", toolbar);
  mBackButton->setEnabled(false);
  connect(mBackButton, &QAction::triggered, this, &WindowMain::onBackClicked);
  toolbar->addAction(mBackButton);
  mFirstSeparator = toolbar->addSeparator();

  mNewProjectButton = new QAction(QIcon(":/icons/outlined/icons8-file-50.png"), "New project", toolbar);
  // connect(mNewProjectButton, &QAction::triggered, this, &WindowMain::onOpenSettingsDialog);
  toolbar->addAction(mNewProjectButton);

  mOpenProjectButton = new QAction(QIcon(":/icons/outlined/icons8-opened-folder-50.png"), "Open project", toolbar);
  connect(mOpenProjectButton, &QAction::triggered, this, &WindowMain::onOpenAnalyzeSettingsClicked);
  toolbar->addAction(mOpenProjectButton);

  mSaveProject = new QAction(QIcon(":/icons/outlined/icons8-save-50.png"), "Save", toolbar);
  mSaveProject->setToolTip("Save project!");
  mSaveProject->setEnabled(false);
  connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProject);
  toolbar->addAction(mSaveProject);

  mSecondSeparator = toolbar->addSeparator();

  mStartAnalysis = new QAction(QIcon(":/icons/outlined/icons8-play-50.png"), "Start", toolbar);
  mStartAnalysis->setEnabled(false);
  mStartAnalysis->setToolTip("Start analysis!");
  connect(mStartAnalysis, &QAction::triggered, this, &WindowMain::onStartClicked);
  toolbar->addAction(mStartAnalysis);

  mDeleteChannel = new QAction(QIcon(":/icons/outlined/icons8-trash-50.png"), "Remove channel", toolbar);
  mDeleteChannel->setToolTip("Delete channel!");
  connect(mDeleteChannel, &QAction::triggered, this, &WindowMain::onRemoveChannelClicked);
  toolbar->addAction(mDeleteChannel);

  toolbar->addSeparator();

  mShowInfoDialog = new QAction(QIcon(":/icons/outlined/icons8-info-50.png"), "Info", toolbar);
  mShowInfoDialog->setToolTip("Info");
  connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);
  toolbar->addAction(mShowInfoDialog);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createLeftToolbar()
{
  mSidebar = new QToolBar(this);
  mSidebar->setMovable(false);
  auto *tabs = new QTabWidget(mSidebar);

  // Project Settings
  mPanelProjectSettings = new PanelProjectSettings(mAnalyzeSettings.experimentSettings, this);
  tabs->addTab(mPanelProjectSettings, "Project");

  // Pipeline Tab
  {
    auto *pipelineTab = new QWidget();
    auto *layout      = new QVBoxLayout();

    //
    // Open template
    //
    mTemplateSelection = new QComboBox();
    mTemplateSelection->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(mTemplateSelection);
    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &WindowMain::onAddChannel);

    // Channel list
    mPanelPipeline = new PanelPipeline(this, mAnalyzeSettings);
    layout->addWidget(mPanelPipeline);

    pipelineTab->setLayout(layout);
    tabs->addTab(pipelineTab, "Pipeline");
  }

  // Image Tab
  {
    auto *imageTab = new QWidget();
    auto *layout   = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    mLabelImageInfo = new QTableWidget(0, 2);
    mLabelImageInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mLabelImageInfo->verticalHeader()->setVisible(false);
    mLabelImageInfo->setHorizontalHeaderLabels({"Meta", "Value"});
    mLabelImageInfo->setAlternatingRowColors(true);
    mLabelImageInfo->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(mLabelImageInfo);
    imageTab->setLayout(layout);
    tabs->addTab(imageTab, "Image");
  }

  // Reportings tab
  {
    tabs->addTab(new QWidget(), "Reportings");
  }

  mSidebar->addWidget(tabs);

  resetImageInfo();
  mSidebar->setVisible(false);

  mSidebar->setMinimumWidth(350);
  addToolBar(Qt::ToolBarArea::LeftToolBarArea, mSidebar);
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createStackedWidget()
{
  mStackedWidget = new QStackedWidget();
  mStackedWidget->setObjectName("stackedWidget");
  // mStackedWidget->addWidget(createOverviewWidget());
  mStackedWidget->addWidget(createChannelWidget());
  mStackedWidget->addWidget(createReportingWidget());
  return mStackedWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelWidget()
{
  return new QWidget(this);
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createReportingWidget()
{
  mPanelReporting = new PanelReporting(this);
  return mPanelReporting;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenAnalyzeSettingsClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedImagesDirectory.empty()) {
    folderToOpen = mSelectedImagesDirectory.string().data();
  }
  if(!mSelectedProjectSettingsFilePath.empty()) {
    folderToOpen = mSelectedProjectSettingsFilePath.string().data();
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath = QFileDialog::getOpenFileName(this, "Open File", folderToOpen, "JSON Files (*.json)", nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }

  try {
    std::ifstream ifs(filePath.toStdString());
    joda::settings::AnalyzeSettings analyzeSettings = nlohmann::json::parse(ifs);
    mPanelPipeline->clear();

    for(const auto &channel : analyzeSettings.channels) {
      mPanelPipeline->addChannel(channel);
    }

    for(const auto &channel : analyzeSettings.vChannels) {
      if(channel.$voronoi.has_value()) {
        mPanelPipeline->addChannel(channel.$voronoi.value());
      }

      if(channel.$intersection.has_value()) {
        mPanelPipeline->addChannel(channel.$intersection.value());
      }
    }
    mPanelProjectSettings->fromSettings(analyzeSettings.experimentSettings);

    mAnalyzeSettings.experimentSettings = analyzeSettings.experimentSettings;
    mAnalyzeSettingsOld                 = mAnalyzeSettings;

    mSelectedProjectSettingsFilePath = filePath.toStdString();
    mSelectedImagesDirectory.clear();
    checkForSettingsChanged();
    onSaveProject();
    showProjectOverview();

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::setWorkingDirectory(const std::string &workingDir)
{
  if(mSelectedImagesDirectory.string() != workingDir) {
    mSelectedImagesDirectory = workingDir;

    std::lock_guard<std::mutex> lock(mLookingForFilesMutex);
    mFoundFilesHint->setText("Looking for images ...");
    mFoundFilesCombo->clear();
    mFileSelectorComboBox->setVisible(false);
    mImageSeriesComboBox->setVisible(false);
    mImageResolutionComboBox->setVisible(false);
    mFileSearchHintLabel->setVisible(true);
    mController->setWorkingDirectory(mSelectedImagesDirectory.string());
    mNewFolderSelected = true;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::waitForFileSearchFinished()
{
  std::thread([this]() {
    auto result = helper::templates::TemplateParser::findTemplates();
    emit lookingForTemplateFinished(result);

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
  }).detach();
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
    auto props = mController->getImageProperties(0, 0).getImageInfo();
    mFoundFilesHint->setText("Finished");
    mFileSearchHintLabel->setVisible(false);
    mFileSelectorComboBox->setVisible(true);
    mImageSeriesComboBox->setVisible(true);
    // mImageResolutionComboBox->setVisible(true);
    mStartAnalysis->setEnabled(true);

  } else {
    // mFoundFilesCombo->setVisible(false);
    resetImageInfo();
    mFoundFilesHint->setText("No images found!");
    mStartAnalysis->setEnabled(false);
  }
}

///
/// \brief      A new image has been selected
/// \author     Joachim Danmayr
///
void WindowMain::resetImageInfo()
{
#warning "Reset data"
}

///
/// \brief      A new image has been selected
/// \author     Joachim Danmayr
///
void WindowMain::onImageSelectionChanged()
{
  auto ome = mController->getImageProperties(mFoundFilesCombo->currentIndex(), mImageSeriesCombo->currentIndex());
  const auto &imgInfo = ome.getImageInfo();

  mLabelImageInfo->setRowCount(20 + ome.getChannelInfos().size() * 7);

  int32_t row = 0;

  auto addItem = [this, &row](const std::string name, int64_t value, const std::string &prefix) {
    mLabelImageInfo->setItem(row, 0, new QTableWidgetItem(name.data()));
    mLabelImageInfo->setItem(row, 1, new QTableWidgetItem(QString::number(value) + " " + QString(prefix.data())));
    row++;
  };

  auto addStringItem = [this, &row](const std::string name, const std::string &value) {
    mLabelImageInfo->setItem(row, 0, new QTableWidgetItem(name.data()));
    mLabelImageInfo->setItem(row, 1, new QTableWidgetItem(value.data()));
    row++;
  };

  auto addTitle = [this, &row](const std::string name) {
    auto *item = new QTableWidgetItem(name.data());
    QFont font;
    font.setBold(true);
    item->setFont(font);
    mLabelImageInfo->setItem(row, 0, item);
    mLabelImageInfo->setItem(row, 1, new QTableWidgetItem(""));
    row++;
  };

  addTitle("Image");
  addItem("Width", imgInfo.resolutions.at(0).imageWidth, "px");
  addItem("Height", imgInfo.resolutions.at(0).imageHeight, "px");
  addItem("Bits", imgInfo.resolutions.at(0).bits, "");

  addItem("Tile width", imgInfo.resolutions.at(0).optimalTileWidth, "px");
  addItem("Tile height", imgInfo.resolutions.at(0).optimalTileHeight, "px");
  addItem("Tile count", imgInfo.resolutions.at(0).getTileCount(), "");

  addItem("Composite tile width", joda::pipeline::COMPOSITE_TILE_WIDTH, "px");
  addItem("Composite tile height", joda::pipeline::COMPOSITE_TILE_HEIGHT, "px");
  addItem("Composite tile count",
          imgInfo.resolutions.at(0).getTileCount(joda::pipeline::COMPOSITE_TILE_WIDTH,
                                                 joda::pipeline::COMPOSITE_TILE_HEIGHT),
          "");
  addItem("Series", ome.getNrOfSeries(), "");
  addItem("Pyramids", ome.getResolutionCount().size(), "");

  const auto &objectiveInfo = ome.getObjectiveInfo();
  addTitle("Objective");
  addStringItem("Manufacturer", objectiveInfo.manufacturer);
  addStringItem("Model", objectiveInfo.model);
  addStringItem("Medium", objectiveInfo.medium);
  addStringItem("Magnification", "x" + std::to_string(objectiveInfo.magnification));

  QString channelInfoStr;
  for(const auto &[idx, channelInfo] : ome.getChannelInfos()) {
    int32_t zStack = 1;
    if(!channelInfo.zStackForTimeFrame.empty()) {
      zStack = channelInfo.zStackForTimeFrame.begin()->second.size();
    }
    addTitle("Channel " + std::to_string(idx));
    addStringItem("ID", channelInfo.channelId);
    addStringItem("Name", channelInfo.name);
    addItem("Emission wave length", channelInfo.emissionWaveLength, channelInfo.emissionWaveLengthUnit);
    addStringItem("Contrast method", channelInfo.contrastMethos);
    addItem("Exposure time", channelInfo.exposuerTime, channelInfo.exposuerTimeUnit);
    addItem("Z-Stack", zStack, "");
  }

  {
    mImageResolutionCombo->blockSignals(true);
    auto currentIdx = mImageResolutionCombo->currentIndex();
    mImageResolutionCombo->clear();
    for(const auto &[idx, pyramid] : imgInfo.resolutions) {
      mImageResolutionCombo->addItem(
          QString((std::to_string(pyramid.imageWidth) + "x" + std::to_string(pyramid.imageHeight)).data()) + " (" +
              bytesToString(pyramid.imageMemoryUsage) + ")",
          idx);
    }
    mImageResolutionCombo->blockSignals(false);
  }
}

///
/// \brief      Check if some settings have been changed
/// \author     Joachim Danmayr
///
void WindowMain::checkForSettingsChanged()
{
  if(!joda::settings::Settings::isEqual(mAnalyzeSettings, mAnalyzeSettingsOld)) {
    // Not equal
    mSaveProject->setIcon(QIcon(":/icons/outlined/icons8-save-50-red.png"));
    mSaveProject->setEnabled(true);
  } else {
    // Equal
    mSaveProject->setIcon(QIcon(":/icons/outlined/icons8-save-50.png"));
    mSaveProject->setEnabled(false);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProjectAsClicked()
{
  std::filesystem::path folderToSaveSettings(mSelectedProjectSettingsFilePath.parent_path());
  QString filePath =
      QFileDialog::getSaveFileName(this, "Save File", folderToSaveSettings.string().data(), "JSON Files (*.json)");
  if(!filePath.isEmpty()) {
    joda::settings::Settings::storeSettings(filePath.toStdString(), mAnalyzeSettings);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProject()
{
  try {
    if(mSelectedProjectSettingsFilePath.empty()) {
      std::filesystem::path filePath(mSelectedImagesDirectory);
      filePath = filePath / "imagec";
      if(!std::filesystem::exists(filePath)) {
        std::filesystem::create_directories(filePath);
      }
      filePath = filePath / "settings.json";
      QString filePathOfSettingsFile =
          QFileDialog::getSaveFileName(this, "Save File", filePath.string().data(), "JSON Files (*.json)");
      mSelectedProjectSettingsFilePath = filePathOfSettingsFile.toStdString();
    }

    if(!mSelectedProjectSettingsFilePath.empty()) {
      setWindowTitlePrefix(mSelectedProjectSettingsFilePath.filename().string().data());
      if(!joda::settings::Settings::isEqual(mAnalyzeSettings, mAnalyzeSettingsOld)) {
        joda::settings::Settings::storeSettings(mSelectedProjectSettingsFilePath.string(), mAnalyzeSettings);
      }
      mAnalyzeSettingsOld = mAnalyzeSettings;
      checkForSettingsChanged();
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not save settings!");
    messageBox.setText("Could not save settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void WindowMain::onFindTemplatesFinished(std::map<std::string, helper::templates::TemplateParser::Data> foundTemplates)
{
  mTemplateSelection->clear();
  mTemplateSelection->addItem("Add channel ...", "");

  mTemplateSelection->addItem(QIcon(":/icons/outlined/icons8-select-none-50.png").pixmap(28, 28), "Empty channel",
                              "emptyChannel");
  mTemplateSelection->addItem(QIcon(":/icons/outlined/dom-voronoi-50.png").pixmap(28, 28), "Voronoi", "voronoiChannel");
  mTemplateSelection->addItem(QIcon(":/icons/outlined/icons8-query-inner-join-50.png").pixmap(28, 28), "Intersection",
                              "intersectionChannel");
  /*mTemplateSelection->addItem(QIcon(":/icons/outlined/icons8-select-none-50.png").pixmap(28, 28), "Giraff",
                              "giraffeChannel");*/

  const QIcon myIcon(":/icons/outlined/icon_eva.png");
  for(const auto &[_, data] : foundTemplates) {
    mTemplateSelection->addItem(QIcon(myIcon.pixmap(28, 28)), data.title.data(), data.path.data());
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onStartClicked()
{
  try {
    if(mJobName.isEmpty()) {
      mJobName = joda::helper::RandomNameGenerator::GetRandomName().data();
    }
    joda::settings::Settings::checkSettings(mAnalyzeSettings);
    DialogAnalyzeRunning dialg(this, mAnalyzeSettings);
    dialg.exec();
    // Analysis finished -> generate new name
    mJobName.clear();
    mJobName = joda::helper::RandomNameGenerator::GetRandomName().data();
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-error-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Error in settings!");
    messageBox.setText(ex.what());
    messageBox.addButton(tr("Okay"), QMessageBox::YesRole);
    messageBox.exec();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onBackClicked()
{
  switch(mNavigation) {
    case Navigation::PROJECT_OVERVIEW:
      break;
    case Navigation::CHANNEL_EDIT:
      showProjectOverview();
      if(mSelectedChannel != nullptr) {
        mSelectedChannel->toSettings();
        mSelectedChannel->setActive(false);
        mSelectedChannel = nullptr;
      }
      checkForSettingsChanged();
      break;
    case Navigation::REPORTING:
      if(showProjectOverview()) {
        if(mPanelReporting != nullptr) {
          mPanelReporting->close();
        }
      }
      break;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
bool WindowMain::showProjectOverview()
{
  // setMiddelLabelText(mSelectedWorkingDirectory);
  mNewProjectButton->setVisible(true);
  mOpenProjectButton->setVisible(true);
  mSidebar->setVisible(true);
  mButtomToolbar->setVisible(true);
  mBackButton->setEnabled(false);
  mBackButton->setVisible(false);
  mSaveProject->setVisible(true);
  mSaveProject->setVisible(true);
  mStartAnalysis->setVisible(true);
  mDeleteChannel->setVisible(false);
  mFirstSeparator->setVisible(false);
  mSecondSeparator->setVisible(true);
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::PROJECT_OVERVIEW));
  mNavigation = Navigation::PROJECT_OVERVIEW;
  return true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showChannelEdit(ContainerBase *selectedChannel)
{
  mSelectedChannel = selectedChannel;
  // mNewProjectButton->setVisible(false);
  // mOpenProjectButton->setVisible(false);
  // mOpenResultsButton->setVisible(false);

  // mSidebar->setVisible(false);
  mButtomToolbar->setVisible(true);
  selectedChannel->setActive(true);
  mBackButton->setEnabled(true);
  mBackButton->setVisible(true);
  // mSaveProject->setVisible(false);
  // mSaveProject->setVisible(false);
  // mStartAnalysis->setVisible(false);
  mDeleteChannel->setVisible(true);
  // mFirstSeparator->setVisible(false);
  // mSecondSeparator->setVisible(false);
  // mOpenReportingArea->setVisible(false);
  mStackedWidget->removeWidget(mStackedWidget->widget(static_cast<int32_t>(Navigation::CHANNEL_EDIT)));
  mStackedWidget->insertWidget(static_cast<int32_t>(Navigation::CHANNEL_EDIT), selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::CHANNEL_EDIT));
  mNavigation = Navigation::CHANNEL_EDIT;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenReportingAreaClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mSelectedImagesDirectory.empty()) {
    folderToOpen = mSelectedImagesDirectory.string().data();
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", folderToOpen, "Results file (*.duckdb)", nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }
  try {
    mPanelReporting->setActualSelectedWorkingFile(filePath.toStdString());

    // Open reporting area
    mNewProjectButton->setVisible(false);
    mOpenProjectButton->setVisible(false);

    // mSidebar->setVisible(false);
    mButtomToolbar->setVisible(false);
    mBackButton->setEnabled(true);
    mBackButton->setVisible(true);
    mSaveProject->setVisible(false);
    mSaveProject->setVisible(false);
    mStartAnalysis->setVisible(false);
    mDeleteChannel->setVisible(false);
    mFirstSeparator->setVisible(false);
    mSecondSeparator->setVisible(false);
    mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::REPORTING));
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
  mNavigation = Navigation::REPORTING;
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
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Remove channel?");
    messageBox.setText("Do you want to remove the channel?");
    QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
    QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    messageBox.setDefaultButton(noButton);
    auto reply = messageBox.exec();
    if(messageBox.clickedButton() == yesButton) {
      mPanelPipeline->erase(mSelectedChannel);
    }
  }
}

///
/// \brief      On add giraf clicked
/// \author     Joachim Danmayr
///
void WindowMain::onAddGirafClicked()
{
  ContainerGiraf *panel1 = new ContainerGiraf(this);
  panel1->fromSettings();
  panel1->toSettings();
}

void WindowMain::onAddChannel()
{
  auto selection = mTemplateSelection->currentData().toString();
  if(selection == "") {
  } else if(selection == "emptyChannel") {
    mPanelPipeline->addChannel(joda::settings::ChannelSettings{});
  } else if(selection == "voronoiChannel") {
    mPanelPipeline->addChannel(joda::settings::VChannelVoronoi{});
  } else if(selection == "intersectionChannel") {
    mPanelPipeline->addChannel(joda::settings::VChannelIntersection{});
  } else {
    mPanelPipeline->addChannel(mTemplateSelection->currentData().toString());
  }
  checkForSettingsChanged();
  mTemplateSelection->blockSignals(true);
  mTemplateSelection->setCurrentIndex(0);
  mTemplateSelection->blockSignals(false);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::onShowInfoDialog()
{
  DialogShadow messageBox(this);
  messageBox.setWindowTitle("Info");
  auto *mainLayout = new QVBoxLayout(&messageBox);
  //   mainLayout->setContentsMargins(28, 28, 28, 28);
  QLabel *helpTextLabel = new QLabel(
      "<p style=\"text-align: left;\"><strong>" + QString(Version::getProgamName().data()) + " " +
      QString(Version::getVersion().data()) + " (" + QString(Version::getBuildTime().data()) +
      ")</strong></p>"
      "<p style=\"text-align: left;\"><em>Licensed under AGPL-3.0<br />Free for non commercial use."
      "</em></p>"
      "<p style=\"text-align: left;\"><strong>Many thanks</strong> for help in setting this project to Melanie "
      "Schuerz</p>"
      "<p style=\"text-align: left;\"><strong>Thank you very much for your help in training the AI "
      "models</strong><br "
      "/>Melanie Schuerz, Anna Mueller, Tanja Plank, Maria Jaritsch, Heloisa Melobenirschke, Patricia Hrasnova and "
      "Ritesh Khanna</p>"
      "<p style=\"text-align: left;\"><em>Icons from <a href=\"https://icons8.com/\">https://icons8.com/</a> and "
      "Dominik Handl.<br /> Special thanks to Tanja Plank for the logo design.</em></p>"
      "<p style=\"text-align: left;\">(c) 2022-2024 Joachim Danmayr</p>");
  helpTextLabel->setOpenExternalLinks(true);
  helpTextLabel->setWordWrap(true);
  QFont fontLineEdit;
  fontLineEdit.setPixelSize(16);
  helpTextLabel->setFont(fontLineEdit);
  mainLayout->addWidget(helpTextLabel);
  mainLayout->addStretch();
  mainLayout->invalidate();
  mainLayout->activate();
  helpTextLabel->adjustSize();
  helpTextLabel->setMinimumHeight(helpTextLabel->height() + 56);

  messageBox.exec();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
QString WindowMain::bytesToString(int64_t bytes)
{
  if(bytes >= 1000000000) {
    return QString::number(static_cast<double>(bytes) / 1000000000.0, 'f', 2) + " GB";
  }
  if(bytes >= 1000000) {
    return QString::number(static_cast<double>(bytes) / 1000000.0, 'f', 2) + " MB";
  }
  if(bytes > 1000) {
    return QString::number(static_cast<double>(bytes) / 1000.0, 'f', 2) + " kB";
  }
  return QString::number(static_cast<double>(bytes) / 1.0, 'f', 2) + "  Byte";
}

}    // namespace joda::ui::qt
