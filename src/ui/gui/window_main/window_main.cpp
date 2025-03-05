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

///

#include "window_main.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
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
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/ai_model_parser/ai_model_parser.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/ome_parser/ome_info.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/helper/username.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/settings.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/dialog_analyze_running.hpp"
#include "ui/gui/dialog_shadow/dialog_shadow.h"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/pipeline_compile_log/panel_pipeline_compile_log.hpp"
#include "ui/gui/results/dialog_results_template_generator.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/panel_image.hpp"
#include "ui/gui/window_main/panel_pipeline.hpp"
#include "ui/gui/window_main/panel_project_settings.hpp"
#include "ui/gui/window_main/panel_results_info.hpp"
#include "build_info.h"
#include "version.h"

namespace joda::ui::gui {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller) : mController(controller), mCompilerLog(new PanelCompilerLog(this))
{
  const QIcon myIcon(":/icons/icons/icon.png");
  setWindowIcon(myIcon);
  setWindowTitle(Version::getTitle().data());
  createTopToolbar();
  createLeftToolbar();
  setMinimumSize(1500, 800);
  setObjectName("windowMain");
  setCentralWidget(createStackedWidget());
  showPanelStartPage();
  clearSettings();

  //
  // Watch for working directory changes
  //
  getController()->registerImageLookupCallback([this](joda::filesystem::State state) {
    if(state == joda::filesystem::State::FINISHED) {
      if(getController()->getNrOfFoundImages() > 0) {
        mStartAnalysis->setEnabled(true);
        mStartAnalysisToolButton->setEnabled(true);
      } else {
        mStartAnalysis->setEnabled(false);
        mStartAnalysisToolButton->setEnabled(false);
      }
    } else if(state == joda::filesystem::State::RUNNING) {
      mStartAnalysis->setEnabled(false);
      mStartAnalysisToolButton->setEnabled(false);
    }
  });

  //
  // Watch for new templates added
  //
  mTemplateDirWatcher.addPath(joda::templates::TemplateParser::getUsersTemplateDirectory().string().data());    // Replace with your desired path
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::fileChanged, [&](const QString &path) {
    loadTemplates();
    mPanelProjectSettings->loadTemplates();
  });
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::directoryChanged, [&](const QString &path) {
    loadTemplates();
    mPanelProjectSettings->loadTemplates();
  });

  loadTemplates();
  mPanelProjectSettings->loadTemplates();

  //
  // Initial background tasks
  //
  std::thread([]() { joda::ai::AiModelParser::findAiModelFiles(); }).detach();

  //
  // Results template
  //
  mResultsTemplate = new DialogResultsTemplateGenerator(this, &mAnalyzeSettings);
}

WindowMain::~WindowMain()
{
}

void WindowMain::closeEvent(QCloseEvent *event)
{
  // Perform any actions before closing
  int result = QMessageBox::question(this, "Confirm Exit", "Are you sure you want to exit?", QMessageBox::Yes | QMessageBox::No);

  if(result == QMessageBox::Yes) {
    // Accept the close event to allow the window to close
    showPanelStartPage();
    event->accept();
  } else {
    // Ignore the close event to keep the window open
    event->ignore();
  }
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

  mNewProjectButton = new QAction(generateIcon("file"), "New project", toolbar);
  connect(mNewProjectButton, &QAction::triggered, this, &WindowMain::onNewProjectClicked);
  toolbar->addAction(mNewProjectButton);

  mOpenProjectButton = new QAction(generateIcon("opened-folder"), "Open project or results", toolbar);
  connect(mOpenProjectButton, &QAction::triggered, this, &WindowMain::onOpenClicked);
  toolbar->addAction(mOpenProjectButton);

  mSaveProject = new QAction(generateIcon("save"), "Save", toolbar);
  mSaveProject->setToolTip("Save project!");
  mSaveProject->setEnabled(false);
  connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProject);
  toolbar->addAction(mSaveProject);

  mSaveProjectAs = new QAction(generateIcon("save-as"), "Save as", toolbar);
  mSaveProjectAs->setToolTip("Save project as!");
  connect(mSaveProjectAs, &QAction::triggered, this, &WindowMain::onSaveProjectAs);
  toolbar->addAction(mSaveProjectAs);

  toolbar->addSeparator();

  auto *showResultsTemplate = new QAction(generateIcon("table"), "Results template", toolbar);
  showResultsTemplate->setToolTip("Results template!");
  connect(showResultsTemplate, &QAction::triggered, [this]() { mResultsTemplate->exec(); });
  toolbar->addAction(showResultsTemplate);

  toolbar->addSeparator();

  mShowCompilerLog = new QAction(generateIcon("popup"), "Compiler log", toolbar);
  mShowCompilerLog->setToolTip("CompileLog!");
  mShowCompilerLog->setCheckable(true);
  connect(mShowCompilerLog, &QAction::triggered, [this](bool checked) {
    if(checked) {
      mCompilerLog->showDialog();
    } else {
      mCompilerLog->hideDialog();
    }
  });
  connect(mCompilerLog->getDialog(), &QDialog::finished, [this] { mShowCompilerLog->setChecked(false); });

  toolbar->addAction(mShowCompilerLog);

  mStartAnalysisToolButton = new QAction(generateIcon("play"), "Start analyze", toolbar);
  mStartAnalysisToolButton->setEnabled(false);
  mStartAnalysisToolButton->setToolTip("Run pipeline!");
  connect(mStartAnalysisToolButton, &QAction::triggered, this, &WindowMain::onStartClicked);
  toolbar->addAction(mStartAnalysisToolButton);

  auto *spacerTop = new QWidget();
  spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  toolbar->addWidget(spacerTop);

  auto *helpButton = new QAction(generateIcon("help"), "Help", toolbar);
  helpButton->setToolTip("Help");
  connect(helpButton, &QAction::triggered, this, &WindowMain::onShowHelpClicked);
  toolbar->addAction(helpButton);

  mShowInfoDialog = new QAction(generateIcon("info"), "Info", toolbar);
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
  mTabWidget = new QTabWidget(mSidebar);

  // Experiment Settings
  {
    mPanelProjectSettings = new PanelProjectSettings(mAnalyzeSettings, this);
    mTabWidget->addTab(mPanelProjectSettings, "Project");
  }

  // Images Tab
  {
    mPanelImages = new PanelImages(this);
    mTabWidget->addTab(mPanelImages, "Images");
  }

  // Classification tab
  {
    mPanelClassification = new PanelClassification(mAnalyzeSettings.projectSettings, this);
    mTabWidget->addTab(mPanelClassification, "Classification");
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

    //
    // Start button
    //
    {
      mStartAnalysis = new QPushButton(generateIcon("play"), "");
      mStartAnalysis->setEnabled(false);
      mStartAnalysis->setToolTip("Run pipeline!");
      innerLayout->addWidget(mStartAnalysis);
    }

    //
    // Add layout
    //
    innerLayout->setStretch(0, 1);
    layout->addLayout(innerLayout);

    // Channel list
    mPanelPipeline = new PanelPipeline(this, mAnalyzeSettings);
    layout->addWidget(mPanelPipeline);

    pipelineTab->setLayout(layout);
    mTabWidget->addTab(pipelineTab, "Pipelines");

    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &WindowMain::onAddChannel);
    connect(mStartAnalysis, &QPushButton::clicked, this, &WindowMain::onStartClicked);
  }

  // Reportings tab
  {
    mPanelResultsInfo = new PanelResultsInfo(this);
    mTabWidget->addTab(mPanelResultsInfo, "Results");
  }

  mSidebar->addWidget(mTabWidget);
  mSidebar->setMinimumWidth(LEFT_TOOLBAR_WIDTH);
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
  QPixmap pixmap(":/icons/icons/icon.png");
  iconLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  layout->addWidget(iconLabel);
  QString text = "<p><span style='font-weight: bold;'>" + QString(Version::getTitle().data()) +
                 "&nbsp;</span><span style='font-weight: bold; font-size: 10pt;'>" + QString(Version::getSubtitle().data()) +
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
    auto icon = generateIcon("info-blue");
    messageBox.setIconPixmap(icon.pixmap(42, 42));
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
  clearSettings();
  checkForSettingsChanged();
  onSaveProject();
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::clearSettings()
{
  mPanelResultsInfo->clearHistory();
  mSelectedProjectSettingsFilePath.clear();
  mPanelPipeline->clear();
  mAnalyzeSettings.resultsSettings = {};
  mAnalyzeSettings                 = {};
  mAnalyzeSettingsOld              = {};
  mAnalyzeSettings.pipelines.clear();
  mAnalyzeSettingsOld.pipelines.clear();
  mPanelProjectSettings->fromSettings({});
  mPanelClassification->fromSettings({});
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onOpenClicked()
{
  QString folderToOpen = QDir::homePath();
  if(!mAnalyzeSettings.projectSettings.workingDirectory.empty()) {
    folderToOpen = mAnalyzeSettings.projectSettings.workingDirectory.data();
  }
  if(!mSelectedProjectSettingsFilePath.empty()) {
    folderToOpen = mSelectedProjectSettingsFilePath.string().data();
  }

  QFileDialog::Options opt;
  opt.setFlag(QFileDialog::DontUseNativeDialog, false);

  QString filePath =
      QFileDialog::getOpenFileName(this, "Open File", folderToOpen,
                                   "ImageC project, template or results files (*" + QString(joda::fs::EXT_PROJECT.data()) + " *" +
                                       QString(joda::fs::EXT_PROJECT_TEMPLATE.data()) + " *" + QString(joda::fs::EXT_DATABASE.data()) +
                                       ");;ImageC project files "
                                       "(*" +
                                       QString(joda::fs::EXT_PROJECT.data()) + ");;ImageC results files (*" + QString(joda::fs::EXT_DATABASE.data()) +
                                       ");;ImageC template files (*" + QString(joda::fs::EXT_PROJECT_TEMPLATE.data()) + ")",
                                   nullptr, opt);

  if(filePath.isEmpty()) {
    return;
  }

  if(filePath.endsWith(joda::fs::EXT_PROJECT.data())) {
    openProjectSettings(filePath, false);
  }
  if(filePath.endsWith(joda::fs::EXT_PROJECT_TEMPLATE.data())) {
    openProjectSettings(filePath, true);
  }
  if(filePath.endsWith(joda::fs::EXT_DATABASE.data())) {
    openResultsSettings(filePath);
  }
}

///
/// \brief      Open results settings
/// \author     Joachim Danmayr
///
void WindowMain::openResultsSettings(const QString &filePath)
{
  onBackClicked();

  try {
    mPanelReporting->openFromFile(filePath);
    mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::REPORTING));
    mNavigation = Navigation::REPORTING;
    mTabWidget->setCurrentIndex((int) Tabs::RESULTS);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief      Open project settings
/// \author     Joachim Danmayr
///
void WindowMain::openProjectSettings(const QString &filePath, bool openFromTemplate)
{
  try {
    joda::settings::AnalyzeSettings analyzeSettings = joda::settings::Settings::openSettings(filePath.toStdString());

    // Assign temporary the newly loaded settings.
    // This is needed to avoid a hen and eg problem when loading the output classes which are needed for the pipelines.
    // They must be known before the pipeline steps are loaded.
    mActAnalyzeSettings = &analyzeSettings;
    showPanelStartPage();
    clearSettings();

    mPanelProjectSettings->fromSettings(analyzeSettings);
    mPanelClassification->fromSettings(analyzeSettings.projectSettings.classification);

    mAnalyzeSettings.resultsSettings                = analyzeSettings.resultsSettings;
    mAnalyzeSettings.projectSettings                = analyzeSettings.projectSettings;
    mAnalyzeSettings.projectSettings.classification = analyzeSettings.projectSettings.classification;
    mAnalyzeSettingsOld                             = mAnalyzeSettings;

    for(const auto &channel : analyzeSettings.pipelines) {
      mPanelPipeline->addChannel(channel);
    }

    mActAnalyzeSettings = &mAnalyzeSettings;
    emit onOutputClassifierChanges();
    if(openFromTemplate) {
      mSelectedProjectSettingsFilePath.clear();
    } else {
      mSelectedProjectSettingsFilePath = filePath.toStdString();
    }
    checkForSettingsChanged();
    if(!openFromTemplate) {
      onSaveProject();
    }
    showPanelStartPage();

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
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
  std::lock_guard<std::mutex> lock(mCheckForSettingsChangedMutex);
  if(!joda::settings::Settings::isEqual(mAnalyzeSettings, mAnalyzeSettingsOld)) {
    // Not equal
    mSaveProject->setEnabled(true);
    /// \todo check if all updates still work
    auto actClasses = getOutputClasses();

    if(actClasses != mOutPutClassesOld) {
      mOutPutClassesOld = actClasses;
      emit onOutputClassifierChanges();
    }
  } else {
    // Equal
    mSaveProject->setEnabled(false);
  }
  mCompilerLog->updateCompilerLog(mAnalyzeSettings);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProject()
{
  saveProject(mSelectedProjectSettingsFilePath);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onSaveProjectAs()
{
  saveProject("", true);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::saveProject(std::filesystem::path filename, bool saveAs)
{
  try {
    if(filename.empty()) {
      std::filesystem::path filePath(mAnalyzeSettings.projectSettings.workingDirectory);
      filePath = filePath / "imagec";
      if(!std::filesystem::exists(filePath)) {
        std::filesystem::create_directories(filePath);
      }
      filePath = filePath / ("settings" + joda::fs::EXT_PROJECT);
      QString filePathOfSettingsFile =
          QFileDialog::getSaveFileName(this, "Save File", filePath.string().data(),
                                       "ImageC project files (*" + QString(joda::fs::EXT_PROJECT.data()) + ");;ImageC project template (*" +
                                           QString(joda::fs::EXT_PROJECT_TEMPLATE.data()) + ")");
      filename = filePathOfSettingsFile.toStdString();
    }
    bool storeAsTemplate = false;
    if(filename.string().ends_with(joda::fs::EXT_PROJECT_TEMPLATE.data())) {
      storeAsTemplate = true;
    }

    if(!filename.empty()) {
      if(!storeAsTemplate) {
        //
        // Store project
        //
        if(!joda::settings::Settings::isEqual(mAnalyzeSettings, mAnalyzeSettingsOld) || saveAs) {
          for(const auto &[pip, _] : mPanelPipeline->getPipelineWidgets()) {
            if(pip) {
              pip->pipelineSavedEvent();
            }
          }
          joda::settings::Settings::storeSettings(filename, mAnalyzeSettings);
        }
        mAnalyzeSettingsOld = mAnalyzeSettings;
        checkForSettingsChanged();
      } else {
        //
        // Store project as template
        //
        joda::settings::Settings::storeSettingsTemplate(filename, mAnalyzeSettings);
      }
    }

    if(!storeAsTemplate) {
      mSelectedProjectSettingsFilePath = filename;
      setWindowTitlePrefix(filename.filename().string().data());
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
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
void WindowMain::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {"templates/pipelines", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_PIPELINE_TEMPLATE);

  mTemplateSelection->clear();
  mTemplateSelection->addItem("Add pipelines ...", "");
  mTemplateSelection->insertSeparator(mTemplateSelection->count());
  mTemplateSelection->addItem(generateIcon("flow-many"), "Empty pipeline", "emptyChannel");
  mTemplateSelection->insertSeparator(mTemplateSelection->count());
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateSelection->insertSeparator(mTemplateSelection->count());
        }
      }
      if(!data.icon.isNull()) {
        mTemplateSelection->addItem(QIcon(data.icon.scaled(28, 28)), data.title.data(), data.path.data());
      } else {
        mTemplateSelection->addItem(generateIcon("favorite"), data.title.data(), data.path.data());
      }
    }
    addedPerCategory = dataInCategory.size();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onStartClicked()
{
  // If there are errors, starting the pipeline is not allowed
  if(mCompilerLog->getNumberOfErrors() > 0) {
    mCompilerLog->showDialog();
    return;
  }

  // Go back to the start panel to free the RAM of the preview
  showPanelStartPage();

  try {
    mAnalyzeSettings.projectSettings.experimentSettings.experimentId   = mPanelProjectSettings->getExperimentId().toStdString();
    mAnalyzeSettings.projectSettings.experimentSettings.experimentName = mPanelProjectSettings->getExperimentName().toStdString();
    DialogAnalyzeRunning dialg(this, mAnalyzeSettings);
    dialg.exec();
    auto jobIinfo = getController()->getJobInformation();
    mPanelResultsInfo->addResultsFileToHistory(jobIinfo.resultsFilePath, jobIinfo.jobName, jobIinfo.timestampStarted);
    // Analysis finished -> generate new name
    mPanelProjectSettings->generateNewJobName();
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("error-red").pixmap(48, 48));
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
  mStartAnalysisToolButton->setVisible(true);
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::START_PAGE));
  if(nullptr != mPanelReporting) {
    mPanelReporting->setActive(false);
  }

  if(mSelectedChannel != nullptr) {
    mSelectedChannel->toSettings();
    mSelectedChannel->setActive(false);
    mSelectedChannel = nullptr;
  }

  mNavigation = Navigation::START_PAGE;

  return true;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::showPanelPipelineSettingsEdit(PanelPipelineSettings *selectedChannel)
{
  if(mNavigation == Navigation::REPORTING) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/icons/icons8-info-50-blue.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Info");
    messageBox.setText("Please close results view first!");
    messageBox.addButton(tr("Okay"), QMessageBox::YesRole);
    messageBox.exec();
    return;
  }
  onBackClicked();
  mSelectedChannel = selectedChannel;
  selectedChannel->setActive(true);

  if(mStackedWidget->count() == 3) {
    mStackedWidget->removeWidget(mStackedWidget->widget(static_cast<int32_t>(Navigation::CHANNEL_EDIT)));
  }
  mStackedWidget->insertWidget(static_cast<int32_t>(Navigation::CHANNEL_EDIT), selectedChannel->getEditPanel());
  mStackedWidget->setCurrentIndex(static_cast<int32_t>(Navigation::CHANNEL_EDIT));

  mNavigation = Navigation::CHANNEL_EDIT;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onRemoveChannelClicked()
{
  if(mSelectedChannel != nullptr) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
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
    mPanelPipeline->addChannel(joda::settings::Pipeline{});
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
  QLabel *helpTextLabel = new QLabel("<p style=\"text-align: left;\"><strong>" + QString(Version::getProgamName().data()) + " " +
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

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
auto WindowMain::getOutputClasses() -> std::set<joda::enums::ClassId>
{
  if(mActAnalyzeSettings != nullptr) {
    return mActAnalyzeSettings->getOutputClasses();
  }
  return mAnalyzeSettings.getOutputClasses();
}

}    // namespace joda::ui::gui
