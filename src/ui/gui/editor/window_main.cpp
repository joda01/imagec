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
#include "ui/gui/dialogs/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/editor/dialog_analyze_running/dialog_analyze_running.hpp"
#include "ui/gui/editor/dialog_open_template/dialog_open_template.hpp"
#include "ui/gui/editor/dialog_save_project_template/dialog_save_project_template.hpp"
#include "ui/gui/editor/pipeline_compile_log/panel_pipeline_compile_log.hpp"
#include "ui/gui/editor/widget_pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_classification.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_pipeline.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_project_settings.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/results/window_results.hpp"
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
  setMinimumSize(1500, 800);
  setObjectName("windowMain");

  //
  // Reporting panel
  //
  {
    mPanelReporting = new WindowResults(this);
  }

  //
  // Preview image area
  //
  {
    mTopToolBar->addSeparator();
    mPreviewImage = new DialogImageViewer(this, mPreviewResult.results.objectMap, &mAnalyzeSettings, mTopToolBar);
    connect(mPreviewImage, &DialogImageViewer::settingChanged, [this]() { checkForSettingsChanged(); });
  }

  createLeftToolbar();

  //
  // Preview results
  //
  {
    mPreviewResultsDialog =
        new DialogPreviewResults(&mPreviewResult, mPreviewImage->getImagePanel(), getSettings().projectSettings.classification, this);
  }

  setCentralWidget(mPreviewImage);
  clearSettings();
  statusBar();

  mDialogOpenProjectTemplates = new DialogOpenTemplate({joda::templates::TemplateParser::getGlobalTemplateDirectory("projects").string(),
                                                        joda::templates::TemplateParser::getUsersTemplateDirectory().string()},
                                                       joda::fs::EXT_PROJECT_TEMPLATE, this);

  //
  // Watch for working directory changes
  //
  getController()->registerImageLookupCallback([this](joda::filesystem::State state) {
    if(state == joda::filesystem::State::FINISHED) {
      if(getController()->getNrOfFoundImages() > 0) {
        mStartAnalysisToolButton->setEnabled(true);
      } else {
        mStartAnalysisToolButton->setEnabled(false);
      }
    } else if(state == joda::filesystem::State::RUNNING) {
      mStartAnalysisToolButton->setEnabled(false);
    }
  });

  //
  // Watch for new templates added
  //
  mTemplateDirWatcher.addPath(joda::templates::TemplateParser::getUsersTemplateDirectory().string().data());    // Replace with your desired path
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::fileChanged, [&](const QString & /*path*/) {
    mPanelPipeline->loadTemplates();
    mDialogOpenProjectTemplates->loadTemplates();
  });
  QObject::connect(&mTemplateDirWatcher, &QFileSystemWatcher::directoryChanged, [&](const QString & /*path*/) {
    mPanelPipeline->loadTemplates();
    mDialogOpenProjectTemplates->loadTemplates();
  });

  mPanelPipeline->loadTemplates();
  mDialogOpenProjectTemplates->loadTemplates();
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
            labelIcon->setPixmap(generateSvgIcon<Style::REGULAR, Color::RED>("box-arrow-down").pixmap(16, 16));
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

  mSaveProject->setEnabled(false);

  QTimer::singleShot(0, this, SLOT(onNewProjectClicked();));
}

WindowMain::~WindowMain() = default;

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
  ////////////
  mTopToolBar = addToolBar("File toolbar");
  mTopToolBar->setObjectName("MainWindowTopToolBar");
  mTopToolBar->setMovable(false);

  auto *newProject = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("file"), "New", mTopToolBar);
  newProject->setStatusTip("Creates a new project either from template ar an empty one.");
  connect(newProject, &QAction::triggered, this, &WindowMain::onNewProjectClicked);

  mOpenProjectButton = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("folder-open"), "Open", mTopToolBar);
  mOpenProjectButton->setStatusTip("Open existing project, template or results");
  connect(mOpenProjectButton, &QAction::triggered, this, &WindowMain::onOpenClicked);

  mOpenRecentProjectMenu = new QMenu("Recent projects");
  mOpenRecentProjectMenu->setIcon(generateSvgIcon<Style::REGULAR, Color::BLACK>("folder-simple"));

  mSaveProject = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("floppy-disk"), "Save", mTopToolBar);
  mSaveProject->setStatusTip("Save project");
  mSaveProject->setEnabled(false);
  connect(mSaveProject, &QAction::triggered, this, &WindowMain::onSaveProject);

  mSaveProjectAs = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("floppy-disk-back"), "Save as", mTopToolBar);
  mSaveProjectAs->setStatusTip("Save project as new name");
  connect(mSaveProjectAs, &QAction::triggered, this, &WindowMain::onSaveProjectAs);

  mShowCompilerLog = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("list-checks"), "Compiler log", mTopToolBar);
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

  mStartAnalysisToolButton = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("person-simple-run"), "Start analyze", mTopToolBar);
  mStartAnalysisToolButton->setStatusTip("Start analyze");
  mStartAnalysisToolButton->setEnabled(false);
  connect(mStartAnalysisToolButton, &QAction::triggered, this, &WindowMain::onStartClicked);

  mShowInfoDialog = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("info"), "About", mTopToolBar);
  mShowInfoDialog->setStatusTip("Open about dialog");
  connect(mShowInfoDialog, &QAction::triggered, this, &WindowMain::onShowInfoDialog);

  // =====================================
  // Toolbar
  // =====================================
  mTopToolBar->addAction(mOpenProjectButton);
  mTopToolBar->addAction(mSaveProject);
  mTopToolBar->addSeparator();
  mTopToolBar->addAction(mStartAnalysisToolButton);

  // =====================================
  // Menu bar
  // =====================================
  mTopMenuBar    = menuBar();
  auto *fileMenu = mTopMenuBar->addMenu("File");
  fileMenu->addAction(newProject);
  fileMenu->addSeparator();
  fileMenu->addAction(mOpenProjectButton);
  fileMenu->addMenu(mOpenRecentProjectMenu);
  fileMenu->addSeparator();
  fileMenu->addAction(mSaveProject);
  fileMenu->addAction(mSaveProjectAs);

  auto *pipelineMenu = mTopMenuBar->addMenu("Pipeline");
  pipelineMenu->addAction(mStartAnalysisToolButton);
  pipelineMenu->addAction(mShowCompilerLog);

  auto *helpMenu = mTopMenuBar->addMenu("Help");
  helpMenu->addAction(mShowInfoDialog);
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::createLeftToolbar()
{
  QDockWidget *firstDock = nullptr;

  auto createDock = [this, &firstDock](const QString &title, QWidget *panel, Qt::DockWidgetArea area = Qt::DockWidgetArea::LeftDockWidgetArea,
                                       int32_t size = LEFT_TOOLBAR_WIDTH) -> QDockWidget * {
    auto *dock = new QDockWidget(this);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable /*DockWidgetClosable*/);

    panel->setMinimumWidth(size);
    panel->setParent(dock);
    dock->setWidget(panel);
    dock->setWindowTitle(title);
    dock->setTitleBarWidget(nullptr);
    mDockWidgets.push_back(dock);

    addDockWidget(area, dock);
    if(firstDock != nullptr) {
      tabifyDockWidget(firstDock, dock);
    } else {
      firstDock = dock;
    }
    return dock;
  };
  setTabPosition(Qt::DockWidgetArea::LeftDockWidgetArea, QTabWidget::TabPosition::North);

  // Experiment Settings
  {
    mPanelProjectSettings = new PanelProjectSettings(mAnalyzeSettings, this);
    createDock("Project", mPanelProjectSettings);
  }

  // Images Tab
  {
    mPanelImages = new PanelImages(this);
    createDock("Images", mPanelImages);
  }

  // Classification tab
  {
    mPanelClassification =
        new PanelClassification(mPreviewResult.results.objectMap, &mAnalyzeSettings.projectSettings.classification, this, mPreviewImage);
    createDock("Classification", mPanelClassification);
  }

  // Pipeline Tab
  {
    mPanelPipeline = new PanelPipeline(&mPreviewResult, this, mAnalyzeSettings);
    createDock("Pipelines", mPanelPipeline);
  }

  firstDock->raise();    // Make project tab visible
}

///
/// \brief
/// \author     Joachim Danmayr
///
QWidget *WindowMain::createChannelWidget()
{
  return new QWidget(this);
}

[[nodiscard]] auto WindowMain::getJobName() const -> QString
{
  return mPanelProjectSettings->getJobName();
}

[[nodiscard]] const PanelImages *WindowMain::getImagePanel() const
{
  return mPanelImages;
}

///
/// \brief
/// \author     Joachim Danmayr
///
WindowMain::AskEnum WindowMain::askForNewProject()
{
  QMessageBox messageBox(this);
  auto icon = generateSvgIcon<Style::REGULAR, Color::BLUE>("warning-circle");
  messageBox.setIconPixmap(icon.pixmap(42, 42));
  messageBox.setWindowTitle("Create new project?");
  messageBox.setText("Save settings and create new project?");
  QPushButton *yesButton    = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  QPushButton *noButton     = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *cancelButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
  messageBox.setDefaultButton(yesButton);
  messageBox.exec();
  if(messageBox.clickedButton() == noButton) {
    return AskEnum::no;
  }
  if(messageBox.clickedButton() == yesButton) {
    return AskEnum::yes;
  }
  if(messageBox.clickedButton() == cancelButton) {
    return AskEnum::cancel;
  }
  return AskEnum::cancel;
}

///
/// \brief
/// \author     Joachim Danmayr
///
void WindowMain::onNewProjectClicked()
{
  auto [mode, selectedTemplate] = mDialogOpenProjectTemplates->show();

  if(mode == DialogOpenTemplate::ReturnCode::CANCEL) {
    return;
  }
  if(mode == DialogOpenTemplate::ReturnCode::OPEN_FILE_DIALOG) {
    onOpenClicked();
    return;
  }
  if(mode == DialogOpenTemplate::ReturnCode::OPEN_RESULTS) {
    openResultsSettings(selectedTemplate);
    return;
  }
  if(mSaveProject->isEnabled()) {
    auto ret = askForNewProject();
    if(ret == AskEnum::cancel) {
      return;
    }
    if(ret == AskEnum::yes) {
      if(!saveProject(mSelectedProjectSettingsFilePath)) {
        // If save was not successful return
        return;
      }
    } else if(ret == AskEnum::no) {
      // Just continue
    }
  }
  showPanelStartPage();
  if(mode == DialogOpenTemplate::ReturnCode::EMPTY_PROJECT) {
    clearSettings();
    checkForSettingsChanged();
  } else {
    checkForSettingsChanged();
    if(mode == DialogOpenTemplate::ReturnCode::OPEN_PROJECT) {
      openProjectSettings(selectedTemplate, false);
    }
    if(mode == DialogOpenTemplate::ReturnCode::OPEN_TEMPLATE) {
      openProjectSettings(selectedTemplate, true);
    }
  }
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
  mAnalyzeSettings.pipelineSetup = {};
  mAnalyzeSettingsOld.pipelines.clear();
  mPanelProjectSettings->fromSettings({});
  mPanelClassification->fromSettings({});
  mPanelPipeline->fromSettings({});
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
/// \brief      Open an image
/// \author     Joachim Danmayr
///
void WindowMain::openImage(const std::filesystem::path &imagePath, const ome::OmeInfo *omeInfo)
{
  mPreviewImage->getImagePanel()->openImage(imagePath, omeInfo);
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
    mPanelReporting->show();
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load database!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    messageBox.exec();
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
    mPanelPipeline->fromSettings(analyzeSettings);
    mPanelClassification->fromSettings(analyzeSettings.projectSettings.classification);
    mPreviewImage->fromSettings(analyzeSettings);

    mAnalyzeSettings.projectSettings                = analyzeSettings.projectSettings;
    mAnalyzeSettings.projectSettings.classification = analyzeSettings.projectSettings.classification;
    mAnalyzeSettings.pipelineSetup                  = analyzeSettings.pipelineSetup;
    mAnalyzeSettingsOld                             = mAnalyzeSettings;

    mPreviewImage->setImagePlane(DialogImageViewer::ImagePlaneSettings{.plane      = {.tStack = 0, .zStack = 0, .cStack = 0},
                                                                       .series     = analyzeSettings.imageSetup.series,
                                                                       .tileWidth  = analyzeSettings.imageSetup.imageTileSettings.tileWidth,
                                                                       .tileHeight = analyzeSettings.imageSetup.imageTileSettings.tileHeight,
                                                                       .tileX      = 0,
                                                                       .tileY      = 0});

    for(const auto &channel : analyzeSettings.pipelines) {
      mPanelPipeline->addChannelFromSettings(channel);
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
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    messageBox.exec();
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
    if(mSelectedProjectSettingsFilePath.empty()) {
      setWindowTitlePrefix("Untitled*");
    } else {
      setWindowTitlePrefix(QString((mSelectedProjectSettingsFilePath.filename().string() + "*").data()));
    }
    /// \todo check if all updates still work
    auto actClasses = getOutputClasses();

    if(actClasses != mOutPutClassesOld) {
      mOutPutClassesOld = actClasses;
      emit onOutputClassifierChanges();
    }
  } else {
    // Equal
    mSaveProject->setEnabled(false);
    if(mSelectedProjectSettingsFilePath.empty()) {
      setWindowTitlePrefix("Untitled");
    } else {
      setWindowTitlePrefix(mSelectedProjectSettingsFilePath.filename().string().data());
    }
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
bool WindowMain::saveProject(std::filesystem::path filename, bool saveAs, bool createHistoryEntry)
{
  bool okay = false;
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
            for(const auto &pip : mPanelPipeline->getPipelineWidgets()) {
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
    okay = true;

  } catch(const std::exception &ex) {
    okay = false;
    joda::log::logError(ex.what());
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save settings!");
    messageBox.setText("Could not save settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    messageBox.exec();
  }

  return okay;
}

///
/// \brief      Load last opened files
/// \author     Joachim Danmayr
///
void WindowMain::loadLastOpened()
{
  mOpenRecentProjectMenu->clear();
  mOpenRecentProjectMenu->addSection("Projects");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedProject()) {
    auto *action = mOpenRecentProjectMenu->addAction(path.path.data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() { openProjectSettings(path.data(), false); });
  }
  mOpenRecentProjectMenu->addSection("Results");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedResult()) {
    auto *action = mOpenRecentProjectMenu->addAction((path.path + " (" + path.title + ")").data());
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
    messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::RED>("warning-diamond").pixmap(48, 48));
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
  showPanelStartPage();
}

///
/// \brief
/// \author     Joachim Danmayr
///
bool WindowMain::showPanelStartPage()
{
  getPanelPipeline()->unselectPipeline();
  return true;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::moveEvent(QMoveEvent *event)
{
  QMainWindow::moveEvent(event);

  if(mPreviewResultsDialog != nullptr) {
    QPoint topRight = this->geometry().topRight();
    mPreviewResultsDialog->move(topRight - QPoint(mPreviewResultsDialog->width() + 2, -250));
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \return
///
void WindowMain::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  if(mPreviewResultsDialog != nullptr) {
    QPoint topRight = this->geometry().topRight();
    mPreviewResultsDialog->move(topRight - QPoint(mPreviewResultsDialog->width() + 2, -250));
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
        "<u>Joachim Danmayr</u> : Creator, Programming, Documentation, Testing<br/><br/>"
        "<u>Melanie Schuerz</u> : Co-Creator, Conceptualization, Testing, AI-Training<br/><br/>"
        "<u>Tanja Plank</u> : Logo design, Testing, AI-Training<br/><br/>"
        "<u>Maria Jaritsch</u> : Testing, AI-Training<br/><br/>"
        "<u>Patricia Hrasnova</u> : Testing, AI-Training<br/><br/>"
        "<u>Anna Dlugosch</u> : Testing<br/><br/>"
        "<u>Heloisa Melo Benirschke</u> : AI-Training<br/><br/>"
        "<u>Manfred Seiwald</u> : Integration testing<br/><br/>");

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
                                  QString::number(static_cast<double>(joda::system::getTotalSystemMemory()) / 1000000.0, 'f', 2) +
                                  "MB <br/>"
                                  "RAM Available: " +
                                  QString::number(static_cast<double>(joda::system::getAvailableSystemMemory()) / 1000000.0, 'f', 2) + " MB <br/>");

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
    auto *labelAbout = new QTextBrowser();
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
