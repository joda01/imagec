///
/// \file      window_main.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "window_main.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstackedwidget.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
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
#include "backend/helper/system/system_resources.hpp"
#include "backend/helper/username.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "backend/settings/settings.hpp"
#include "backend/updater/updater.hpp"
#include "backend/user_settings/user_settings.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/dialog_analyze_running.hpp"
#include "ui/gui/dialog_save_project_template/dialog_save_project_template.hpp"
#include "ui/gui/dialog_shadow/dialog_shadow.h"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/pipeline_compile_log/panel_pipeline_compile_log.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/panel_image.hpp"
#include "ui/gui/window_main/panel_pipeline.hpp"
#include "ui/gui/window_main/panel_project_settings.hpp"
#include "build_info.h"
#include "version.h"

namespace joda::ui::gui {

using namespace std::chrono_literals;

WindowMain::WindowMain(joda::ctrl::Controller *controller, joda::updater::Updater *updater) :
    mController(controller), mCompilerLog(new PanelCompilerLog(this))
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
  statusBar();

  //
  // Watch for working directory changes
  //
  getController()->registerImageLookupCallback([this](joda::filesystem::State state) {
    if(state == joda::filesystem::State::FINISHED) {
      if(getController()->getNrOfFoundImages() > 0) {
        mPanelPipeline->setActionStartEnabled(true);
        mStartAnalysisToolButton->setEnabled(true);
      } else {
        mPanelPipeline->setActionStartEnabled(false);
        mStartAnalysisToolButton->setEnabled(false);
      }
    } else if(state == joda::filesystem::State::RUNNING) {
      mPanelPipeline->setActionStartEnabled(false);
      mStartAnalysisToolButton->setEnabled(false);
    }
  });

  //
  // Watch for new templates added
  //
  mTemplateDirWatcher.addPath(joda::templates::TemplateParser::getUsersTemplateDirectory().string().data());    // Replace with your desired path
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::fileChanged, [&](const QString &path) {
    mPanelPipeline->loadTemplates();
    loadProjectTemplates();
  });
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::directoryChanged, [&](const QString &path) {
    mPanelPipeline->loadTemplates();
    loadProjectTemplates();
  });

  //
  //
  //
  {
    mPreviewImage = new DialogImageViewer(this);
    mPreviewImage->setVisible(false);
    mPreviewImage->setContentsMargins(0, 0, 0, 0);
    mPreviewImage->resetImage();
    addDockWidget(Qt::RightDockWidgetArea, mPreviewImage);
  }

  mPanelPipeline->loadTemplates();
  loadProjectTemplates();
  loadLastOpened();

  //
  // Initial background tasks
  //
  std::thread([]() { joda::ai::AiModelParser::findAiModelFiles(); }).detach();
  std::thread([mainWindow = this, updater]() {
    joda::updater::Updater::Status status = joda::updater::Updater::Status::PENDING;
    joda::updater::Updater::CheckForUpdateResponse response;
    do {
      status = updater->getCheckForUpdateResponse(response);
      std::this_thread::sleep_for(1s);
    } while(status == joda::updater::Updater::Status::PENDING);
    if(status == joda::updater::Updater::Status::NEWER_VERSION_AVAILABLE) {
      QMetaObject::invokeMethod(
          mainWindow,
          [mainWindow, response]() {
            auto *layout = new QHBoxLayout();
            layout->setContentsMargins(4, 0, 0, 4);
            auto *labelIcon = new QLabel();
            labelIcon->setPixmap(generateSvgIcon("update-high").pixmap(16, 16));
            labelIcon->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            layout->addWidget(labelIcon);
            auto *label = new QLabel();
            label->setText("ImageC update <b>" + QString(response.newVersion.data()) +
                           "</b> available. To download, visit <a href=\"https://imagec.org/#download\">imagec.org</a>.");
            label->setOpenExternalLinks(true);
            label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            layout->addWidget(label);
            auto *widget = new QWidget();
            widget->setContentsMargins(0, 0, 0, 0);
            widget->setLayout(layout);

            mainWindow->statusBar()->addWidget(widget);
          },
          Qt::QueuedConnection);
    } else if(status == joda::updater::Updater::Status::UPDATE_SERVER_NOT_REACHABLE) {
      QMetaObject::invokeMethod(
          mainWindow, [mainWindow, response]() { mainWindow->statusBar()->showMessage("Could not check for updates!", 5000); }, Qt::QueuedConnection);
    }
  }).detach();
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

void WindowMain::setSideBarVisible(bool visible)
{
  mSidebar->setVisible(visible);
  mTopToolBar->setVisible(visible);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createTopToolbar()
{
  ////////////
  mTopToolBar = addToolBar("File toolbar");

  mNewProjectMenu   = new QMenu();
  mNewProjectButton = new QAction(generateSvgIcon("folder-new"), "New project", mTopToolBar);
  mNewProjectButton->setStatusTip("Create new project or create new from template");
  mNewProjectButton->setMenu(mNewProjectMenu);
  connect(mNewProjectButton, &QAction::triggered, this, &WindowMain::onNewProjectClicked);
  mTopToolBar->addAction(mNewProjectButton);

  mOpenProjectMenu   = new QMenu();
  mOpenProjectButton = new QAction(generateSvgIcon("document-open-folder"), "Open project or results", mTopToolBar);
  mOpenProjectButton->setStatusTip("Open existing project, template or results");
  mOpenProjectButton->setMenu(mOpenProjectMenu);
  connect(mOpenProjectButton, &QAction::triggered, this, &WindowMain::onOpenClicked);
  mTopToolBar->addAction(mOpenProjectButton);

  mSaveProject = new QAction(generateSvgIcon("document-save"), "Save", mTopToolBar);
  mSaveProject->setStatusTip("Save project");
  mSaveProject->setEnabled(false);
  connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProject);
  mTopToolBar->addAction(mSaveProject);

  mSaveProjectAs = new QAction(generateSvgIcon("document-save-as"), "Save as", mTopToolBar);
  mSaveProjectAs->setStatusTip("Save project as new name");
  connect(mSaveProjectAs, &QAction::triggered, this, &WindowMain::onSaveProjectAs);
  mTopToolBar->addAction(mSaveProjectAs);

  mTopToolBar->addSeparator();

  // auto *showResultsTemplate = new QAction(generateIcon("table"), "Results template", toolbar);
  // showResultsTemplate->setToolTip("Results template!");
  // connect(showResultsTemplate, &QAction::triggered, [this]() { mResultsTemplate->exec(); });
  // toolbar->addAction(showResultsTemplate);
  // mTopToolBar->addSeparator();

  mShowCompilerLog = new QAction(generateSvgIcon("show-all-effects"), "Compiler log", mTopToolBar);
  mShowCompilerLog->setStatusTip("Show possible pipeline issues");
  mShowCompilerLog->setCheckable(true);
  connect(mShowCompilerLog, &QAction::triggered, [this](bool checked) {
    if(checked) {
      mCompilerLog->showDialog();
    } else {
      mCompilerLog->hideDialog();
    }
  });
  connect(mCompilerLog->getDialog(), &QDialog::finished, [this] { mShowCompilerLog->setChecked(false); });

  mTopToolBar->addAction(mShowCompilerLog);

  mStartAnalysisToolButton = new QAction(generateSvgIcon("media-playback-start"), "Start analyze", mTopToolBar);
  mStartAnalysisToolButton->setStatusTip("Start analyze");
  mStartAnalysisToolButton->setEnabled(false);
  connect(mStartAnalysisToolButton, &QAction::triggered, this, &WindowMain::onStartClicked);
  mTopToolBar->addAction(mStartAnalysisToolButton);

  auto *spacerTop = new QWidget();
  spacerTop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mTopToolBar->addWidget(spacerTop);

  // auto *helpButton = new QAction(generateSvgIcon("help-contents"), "Help", mTopToolBar);
  // helpButton->setToolTip("Help");
  // connect(helpButton, &QAction::triggered, this, &WindowMain::onShowHelpClicked);
  // mTopToolBar->addAction(helpButton);

  mTopToolBar->addSeparator();

  mShowInfoDialog = new QAction(generateSvgIcon("help-about"), "About", mTopToolBar);
  mShowInfoDialog->setStatusTip("Open about dialog");
  connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);
  mTopToolBar->addAction(mShowInfoDialog);
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
    mPanelPipeline = new PanelPipeline(this, mAnalyzeSettings);
    mTabWidget->addTab(mPanelPipeline, "Pipelines");
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
bool WindowMain::askForNewProject()
{
  QMessageBox messageBox(this);
  auto icon = generateSvgIcon("data-information");
  messageBox.setIconPixmap(icon.pixmap(42, 42));
  messageBox.setWindowTitle("Create new project?");
  messageBox.setText("Unsaved settings will get lost! Create new project?");
  QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  auto reply = messageBox.exec();
  return messageBox.clickedButton() != noButton;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onNewProjectClicked()
{
  if(!mSelectedProjectSettingsFilePath.empty()) {
    if(!askForNewProject()) {
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
  mSelectedProjectSettingsFilePath.clear();
  mPanelPipeline->clear();
  mAnalyzeSettings    = {};
  mAnalyzeSettingsOld = {};
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
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
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
void WindowMain::addToLastLoadedResults(const QString &path, const QString &jobName)
{
  joda::user_settings::UserSettings::addLastOpenedResult(path.toStdString(), jobName.toStdString());
  loadLastOpened();
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
      saveProject(mSelectedProjectSettingsFilePath, false, false);
      joda::user_settings::UserSettings::addLastOpenedProject(filePath.toStdString());
      loadLastOpened();
    }
    showPanelStartPage();

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
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
void WindowMain::saveProject(std::filesystem::path filename, bool saveAs, bool createHistoryEntry)
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
          if(createHistoryEntry) {
            for(const auto &[pip, _] : mPanelPipeline->getPipelineWidgets()) {
              if(pip) {
                pip->pipelineSavedEvent();
              }
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
        auto *dialog = new DialogSaveProjectTemplate(this);
        if(dialog->exec() != 0) {
          joda::settings::Settings::storeSettingsTemplate(filename, mAnalyzeSettings, dialog->toSettingsMeta());
        }
      }
    }

    if(!storeAsTemplate) {
      mSelectedProjectSettingsFilePath = filename;
      setWindowTitlePrefix(filename.filename().string().data());
    }

  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
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
void WindowMain::loadProjectTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {"templates/projects", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_PROJECT_TEMPLATE);

  mNewProjectMenu->clear();
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mNewProjectMenu->addSeparator();
        }
      }
      QAction *action;
      if(!data.icon.isNull()) {
        action = mNewProjectMenu->addAction(QIcon(data.icon.scaled(28, 28)), data.title.data());

      } else {
        action = mNewProjectMenu->addAction(generateSvgIcon("favorite"), data.title.data());
      }
      connect(action, &QAction::triggered, this, [this, path = data.path]() {
        if(!askForNewProject()) {
          return;
        }
        checkForSettingsChanged();
        openProjectSettings(path.data(), true);
      });
    }
    addedPerCategory = dataInCategory.size();
  }
}

///
/// \brief      Load last opened files
/// \author     Joachim Danmayr
///
void WindowMain::loadLastOpened()
{
  mOpenProjectMenu->clear();
  mOpenProjectMenu->addSection("Projects");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedProject()) {
    auto *action = mOpenProjectMenu->addAction(path.path.data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() { openProjectSettings(path.data(), false); });
  }
  mOpenProjectMenu->addSection("Results");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedResult()) {
    auto *action = mOpenProjectMenu->addAction((path.path + " (" + path.title + ")").data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() { openResultsSettings(path.data()); });
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
    addToLastLoadedResults(jobIinfo.resultsFilePath.string().data(), jobIinfo.jobName.data());
    // Analysis finished -> generate new name
    mPanelProjectSettings->generateNewJobName();
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-error").pixmap(48, 48));
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
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
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
  auto *about = new QDialog(this);
  about->setWindowTitle("About ImageC");
  auto *tab = new QTabWidget();
  //
  // About Tab
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "About");
    auto *labelAbout = new QLabel("ImageC " + QString(Version::getVersion().data()) +
                                  " is an application for high throughput image processing.<br/><br/>"
                                  "Copyright 2019-2025 Joachim Danmayr<br/><br/>"
                                  "<a href=\"https://imagec.org/\">https://imagec.org/</a><br/><br/>"
                                  "ALL RIGHTS RESERVED");
    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // Contributors
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "Contributors");
    auto *labelAbout = new QLabel(
        "<u>Melanie Schuerz</u> : Coordination, Application testing, AI-Training<br/><br/>"
        "<u>Tanja Plank</u> : Logo design, Application testing, AI-Training<br/><br/>"
        "<u>Maria Jaritsch</u> : Application testing, AI-Training<br/><br/>"
        "<u>Patricia Hrasnova</u> : Application testing, AI-Training<br/><br/>"
        "<u>Anna Dlugosch</u> : Application testing<br/><br/>"
        "<u>Heloisa Melobenirschke</u> : AI-Training<br/><br/>"
        "<u>Manfred Seiwald</u> : Integration testing<br/><br/>"
        "<u>Joachim Danmayr</u> : Idea, Programming, Documentation, Testing<br/><br/>");

    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // Version
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "Version");
    auto *labelAbout = new QLabel("<b>" + QString(Version::getProgamName().data()) + " " + QString(Version::getVersion().data()) + "</b><br/>" +
                                  QString(Version::getBuildTime().data()) + "<br/><br/>" +
                                  "CPU cores: " + QString(std::to_string(joda::system::getNrOfCPUs()).data()) +
                                  "<br/>"
                                  "RAM Total: " +
                                  QString::number((double) joda::system::getTotalSystemMemory() / 1000000.0, 'f', 2) +
                                  "MB <br/>"
                                  "RAM Available: " +
                                  QString::number((double) joda::system::getAvailableSystemMemory() / 1000000.0, 'f', 2) + " MB <br/>");

    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // Open source
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "Libraries");
    QFile file(":/other/other/open_source_libs.html");
    QString openSourceLibs;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      openSourceLibs = file.readAll();
      file.close();
    }
    auto *labelAbout = new QTextBrowser();
    labelAbout->setHtml(openSourceLibs);
    labelAbout->setOpenExternalLinks(true);

    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // 3rd party
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "3rd party");
    auto *labelAbout = new QLabel();
    QString others =
        "<u>icons8</u> : Thanks to <a href=\"https://icons8.com/\">http://icon8.org</a> for providing some of the icons!<br/><br/>"
        "<u>KDE project</u> : Thanks to the KDE project for providing the <a "
        "href=\"https://develop.kde.org/frameworks/breeze-icons/\">Breeze</a> icon set!<br/><br/>";
    labelAbout->setText(others);
    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // License
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "License");
    QFile file(":/other/other/license.md");
    QString openSourceLibs;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      openSourceLibs = file.readAll();
      file.close();
    }
    auto *preamble = new QLabel(
        "(c) 2019-2025 Joachim Danmayr. Free to use and licensed under AGPL V3 for <b>non commercial</b>!<br/>For usage in "
        "commercial environment, please contact <a href= \"mailto:support@imagec.org\">support@imagec.org</a>.");
    preamble->setOpenExternalLinks(true);
    preamble->setAlignment(Qt::AlignCenter);
    layoutAbout->addWidget(preamble);

    auto *labelAbout = new QTextBrowser();
    labelAbout->setMarkdown(openSourceLibs);
    labelAbout->setOpenExternalLinks(true);
    labelAbout->setOpenExternalLinks(true);
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // Log
  //
  {
    auto *widgetAbout = new QWidget();
    auto *layoutAbout = new QVBoxLayout();
    layoutAbout->setAlignment(Qt::AlignCenter);
    widgetAbout->setLayout(layoutAbout);
    tab->addTab(widgetAbout, "Log");
    auto *labelAbout      = new QTextBrowser();
    const auto &logBuffer = joda::log::getLogBuffer();
    labelAbout->setHtml(joda::log::logBufferToHtml().data());
    labelAbout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layoutAbout->addWidget(labelAbout);
  }

  //
  // Main layout
  //
  auto *mainLayout = new QVBoxLayout();
  mainLayout->addWidget(tab);
  about->setLayout(mainLayout);
  about->resize(650, 550);
  about->exec();
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
