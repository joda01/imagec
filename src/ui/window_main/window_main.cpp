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
#include "ui/results/panel_results.hpp"
#include "ui/window_main/panel_image.hpp"
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
#include "ui/window_main/panel_results_info.hpp"
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
  createLeftToolbar();
  setMinimumSize(1600, 800);
  setObjectName("windowMain");
  setCentralWidget(createStackedWidget());
  showPanelStartPage();

  getController()->registerWorkingDirectoryCallback([this](joda::helper::fs::State state) {
    if(state == joda::helper::fs::State::FINISHED) {
      if(getController()->getNrOfFoundImages() > 0) {
        mStartAnalysis->setEnabled(true);
      } else {
        mStartAnalysis->setEnabled(false);
      }
    } else if(state == joda::helper::fs::State::RUNNING) {
      mStartAnalysis->setEnabled(false);
    }
  });
}

void WindowMain::setWindowTitlePrefix(const QString &txt)
{
  if(!txt.isEmpty()) {
    setWindowTitle(QString(Version::getTitle().data()) + " - " + txt);
  } else {
    setWindowTitle(QString(Version::getTitle().data()));
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createTopToolbar()
{
  auto *toolbar = addToolBar("toolbar");
  toolbar->setMovable(false);

  mNewProjectButton = new QAction(QIcon(":/icons/outlined/icons8-file-50.png"), "New project", toolbar);
  connect(mNewProjectButton, &QAction::triggered, this, &WindowMain::onNewProjectClicked);
  toolbar->addAction(mNewProjectButton);

  mOpenProjectButton =
      new QAction(QIcon(":/icons/outlined/icons8-opened-folder-50.png"), "Open project or results", toolbar);
  connect(mOpenProjectButton, &QAction::triggered, this, &WindowMain::onOpenClicked);
  toolbar->addAction(mOpenProjectButton);

  mSaveProject = new QAction(QIcon(":/icons/outlined/icons8-save-50.png"), "Save", toolbar);
  mSaveProject->setToolTip("Save project!");
  mSaveProject->setEnabled(false);
  connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProject);
  toolbar->addAction(mSaveProject);

  auto *spacerTop = new QWidget();
  spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolbar->addWidget(spacerTop);

  auto *helpButton = new QAction(QIcon(":/icons/outlined/icons8-help-50.png"), "Info", toolbar);
  helpButton->setToolTip("Help");
  connect(helpButton, &QAction::triggered, this, &WindowMain::onShowHelpClicked);
  toolbar->addAction(helpButton);

  mShowInfoDialog = new QAction(QIcon(":/icons/outlined/icons8-info-50-circle.png"), "Info", toolbar);
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
  mSidebar->setMovable(true);
  auto *tabs = new QTabWidget(mSidebar);

  // Experiment Settings
  {
    mPanelProjectSettings = new PanelProjectSettings(mAnalyzeSettings.experimentSettings, this);
    tabs->addTab(mPanelProjectSettings, "Experiment");
  }

  // Pipeline Tab
  {
    auto *pipelineTab = new QWidget();
    auto *layout      = new QVBoxLayout();

    //
    // Open template
    //
    auto *innerLayout  = new QHBoxLayout();
    mTemplateSelection = new QComboBox();
    innerLayout->addWidget(mTemplateSelection);

    mStartAnalysis = new QPushButton(QIcon(":/icons/outlined/icons8-play-50.png"), "");
    mStartAnalysis->setEnabled(false);
    mStartAnalysis->setToolTip("Run pipeline!");
    innerLayout->addWidget(mStartAnalysis);

    innerLayout->setStretch(0, 1);
    layout->addLayout(innerLayout);

    // Channel list
    mPanelPipeline = new PanelPipeline(this, mAnalyzeSettings);
    layout->addWidget(mPanelPipeline);

    pipelineTab->setLayout(layout);
    tabs->addTab(pipelineTab, "Pipeline");

    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &WindowMain::onAddChannel);
    connect(mStartAnalysis, &QPushButton::clicked, this, &WindowMain::onStartClicked);
    loadTemplates();
  }

  // Images Tab
  {
    mPanelImages = new PanelImages(this);
    tabs->addTab(mPanelImages, "Images");
  }

  // Reportings tab
  {
    mPanelResultsInfo = new PanelResultsInfo(this);
    tabs->addTab(mPanelResultsInfo, "Results");
  }

  mSidebar->addWidget(tabs);
  mSidebar->setMinimumWidth(365);
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
  mStackedWidget->addWidget(createStartPageWidget());
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
QWidget *WindowMain::createStartPageWidget()
{
  auto *layout = new QVBoxLayout(); /*this*/
  layout->setContentsMargins(16, 16, 16, 16);
  layout->setSpacing(8);
  layout->setAlignment(Qt::AlignTop);

  //
  // Label
  //
  // Create a label
  auto *iconLabel = new QLabel();
  QPixmap pixmap(":/icons/outlined/icon.png");
  iconLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  layout->addWidget(iconLabel);
  QString text = "<p><span style='font-weight: bold;'>" + QString(Version::getTitle().data()) +
                 "&nbsp;</span><span style='font-weight: bold; font-size: 10pt;'>" +
                 QString(Version::getSubtitle().data()) +
                 "</span><br>"
                 "<span style='font-size: 10pt; color: darkgray;'>" +
                 QString(Version::getVersion().data()) + "</span></p>";
  auto *startText = new QLabel(text);
  layout->addWidget(startText);

  //
  // Separator
  //
  auto *line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  layout->addWidget(line);
  /*
    //
    // New project
    //
    auto *newProject = new QPushButton();
    const QIcon voronoiIcon(":/icons/outlined/icons8-add-new-50.png");
    newProject->setText("New project");
    newProject->setIconSize({16, 16});
    newProject->setIcon(voronoiIcon);
    layout->addWidget(newProject);

    //
    // Open existing project
    //
    auto *openProject = new QPushButton();
    const QIcon intersectionIcon(":/icons/outlined/icons8-opened-folder-50.png");
    openProject->setIconSize({16, 16});
    openProject->setIcon(intersectionIcon);
    openProject->setText("Open project");
    layout->addWidget(openProject);

    //
    // Open results
    //
    auto *openResults = new QPushButton();
    const QIcon openResultsIcon(":/icons/outlined/icons8-graph-50.png");
    openResults->setIconSize({16, 16});
    openResults->setIcon(openResultsIcon);
    openResults->setText("Open results");
    layout->addWidget(openResults);
  */

  ////////////////////////////////////////
  auto *startScreenWidget = new QWidget();
  startScreenWidget->setMaximumWidth(300);
  startScreenWidget->setLayout(layout);
  return startScreenWidget;
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createReportingWidget()
{
  mPanelReporting = new PanelResults(this);
  return mPanelReporting;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onNewProjectClicked()
{
  if(!mSelectedProjectSettingsFilePath.empty()) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-info-50-blue.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Create new project?");
    messageBox.setText("Unsaved settings will get lost! Create new project?");
    QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
    QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
    messageBox.setDefaultButton(noButton);
    auto reply = messageBox.exec();
    if(messageBox.clickedButton() == noButton) {
      return;
    }
  }

  showPanelStartPage();
  mSelectedProjectSettingsFilePath.clear();
  mAnalyzeSettings    = {};
  mAnalyzeSettingsOld = {};
  mPanelPipeline->clear();
  mPanelProjectSettings->fromSettings({});
  checkForSettingsChanged();
  onSaveProject();
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mAnalyzeSettings.experimentSettings.workingDirectory.empty()) {
    folderToOpen = mAnalyzeSettings.experimentSettings.workingDirectory.data();
  }
  if(!mSelectedProjectSettingsFilePath.empty()) {
    folderToOpen = mSelectedProjectSettingsFilePath.string().data();
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", folderToOpen,
                                   "ImageC project or results files (*.icproj *.icresult);;ImageC project files "
                                   "(*.icproj);;ImageC results files (*.icresult)",
                                   nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }

  if(filePath.endsWith(".icproj")) {
    openProjectSettings(filePath);
  }
  if(filePath.endsWith(".icresult")) {
    openResultsSettings(filePath);
  }
}

///
/// \brief      Open results settings
/// \author     Joachim Danmayr
///
void WindowMain::openResultsSettings(const QString &filePath)
{
  showPanelResults();
  mPanelReporting->openFromFile(filePath);
}

///
/// \brief      Open project settings
/// \author     Joachim Danmayr
///
void WindowMain::openProjectSettings(const QString &filePath)
{
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
    checkForSettingsChanged();
    onSaveProject();
    showPanelStartPage();

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
/// \brief      Check if some settings have been changed
/// \author     Joachim Danmayr
///
void WindowMain::checkForSettingsChanged()
{
  if(!joda::settings::Settings::isEqual(mAnalyzeSettings, mAnalyzeSettingsOld)) {
    // Not equal
    // mSaveProject->setIcon(QIcon(":/icons/outlined/icons8-save-50-red.png"));
    mSaveProject->setEnabled(true);
  } else {
    // Equal
    // mSaveProject->setIcon(QIcon(":/icons/outlined/icons8-save-50.png"));
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
  QString filePath = QFileDialog::getSaveFileName(this, "Save File", folderToSaveSettings.string().data(),
                                                  "ImageC project files (*.icproj)");
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
      std::filesystem::path filePath(mAnalyzeSettings.experimentSettings.workingDirectory);
      filePath = filePath / "imagec";
      if(!std::filesystem::exists(filePath)) {
        std::filesystem::create_directories(filePath);
      }
      filePath = filePath / "settings.icproj";
      QString filePathOfSettingsFile =
          QFileDialog::getSaveFileName(this, "Save File", filePath.string().data(), "ImageC project files (*.icproj)");
      mSelectedProjectSettingsFilePath = filePathOfSettingsFile.toStdString();
    }

    if(!mSelectedProjectSettingsFilePath.empty()) {
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

  setWindowTitlePrefix(mSelectedProjectSettingsFilePath.filename().string().data());
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void WindowMain::loadTemplates()
{
  auto foundTemplates = joda::helper::templates::TemplateParser::findTemplates();

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
    joda::settings::Settings::checkSettings(mAnalyzeSettings);
    DialogAnalyzeRunning dialg(this, mAnalyzeSettings);
    dialg.exec();
    // Analysis finished -> generate new name
    mPanelProjectSettings->generateNewJobName();
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
    case Navigation::START_PAGE:
      break;
    case Navigation::CHANNEL_EDIT:
      showPanelStartPage();
      if(mSelectedChannel != nullptr) {
        mSelectedChannel->toSettings();
        mSelectedChannel->setActive(false);
        mSelectedChannel = nullptr;
      }
      checkForSettingsChanged();
      break;
    case Navigation::REPORTING:
      if(showPanelStartPage()) {
        if(mPanelReporting != nullptr) {
          mPanelReporting->setActive(false);
        }
      }
      break;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
bool WindowMain::showPanelStartPage()
{
  mNewProjectButton->setVisible(true);
  mOpenProjectButton->setVisible(true);
  mSidebar->setVisible(true);
  mSaveProject->setVisible(true);
  mSaveProject->setVisible(true);
  mStartAnalysis->setVisible(true);
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::START_PAGE));
  mNavigation = Navigation::START_PAGE;
  return true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showPanelChannelEdit(ContainerBase *selectedChannel)
{
  mSelectedChannel = selectedChannel;
  selectedChannel->setActive(true);
  mStackedWidget->removeWidget(mStackedWidget->widget(static_cast<int32_t>(Navigation::CHANNEL_EDIT)));
  mStackedWidget->insertWidget(static_cast<int32_t>(Navigation::CHANNEL_EDIT), selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::CHANNEL_EDIT));
  mNavigation = Navigation::CHANNEL_EDIT;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showPanelResults()
{
  mPanelReporting->setActive(true);
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::REPORTING));
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
      auto *selectedChanel = mSelectedChannel;
      showPanelStartPage();
      mPanelPipeline->erase(selectedChanel);
      mSelectedChannel = nullptr;
    }
  }
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
void WindowMain::onShowHelpClicked()
{
  QUrl url("https://imagec.org/doc");
  QDesktopServices::openUrl(url);
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
