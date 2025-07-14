///
/// \file      window_main.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qcombobox.h>
#include <qdockwidget.h>
#include <qwidget.h>
#include <QtWidgets>
#include <filesystem>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/setting.hpp"
#include "controller/controller.hpp"
#include "ui/gui/container/container_base.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/helper/clickablelabel.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/pipeline_compile_log/panel_pipeline_compile_log.hpp"
#include "ui/gui/window_main/panel_classification.hpp"
#include "ui/gui/window_main/panel_image.hpp"
#include "ui/gui/window_main/panel_pipeline.hpp"
#include "ui/gui/window_main/panel_project_settings.hpp"
#include <duckdb/function/table_function.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::updater {
class Updater;
}

namespace joda::ui::gui {

class ContainerChannel;
class PanelResults;
class PanelResultsTableGenerator;
class DialogOpenTemplate;

///
/// \class
/// \author
/// \brief
///
class WindowMain : public QMainWindow
{
  Q_OBJECT

public:
  WindowMain(joda::ctrl::Controller *controller, joda::updater::Updater *updater);
  ~WindowMain();
  bool showPanelStartPage();
  void openProjectSettings(const QString &filePath, bool openFromTemplate);
  void openResultsSettings(const QString &filePath);
  void openImage(const std::filesystem::path &imagePath, const ome::OmeInfo *omeInfo);

  joda::ctrl::Controller *getController()
  {
    return mController;
  }

  [[nodiscard]] auto getJobName() const -> QString
  {
    return mPanelProjectSettings->getJobName();
  }

  [[nodiscard]] const PanelImages *getImagePanel() const
  {
    return mPanelImages;
  }

  [[nodiscard]] PanelPipeline *getPanelPipeline()
  {
    return mPanelPipeline;
  }

  [[nodiscard]] PanelClassification *getPanelClassification()
  {
    return mPanelClassification;
  }

  [[nodiscard]] PanelProjectSettings *getPanelProjectSettings()
  {
    return mPanelProjectSettings;
  }

  [[nodiscard]] PanelResults *getPanelResults()
  {
    return mPanelReporting;
  }

  const settings::AnalyzeSettings &getSettings()
  {
    return mAnalyzeSettings;
  }

  settings::AnalyzeSettings &mutableSettings()
  {
    return mAnalyzeSettings;
  }

  auto getPreviewDock() -> DialogImageViewer *
  {
    return mPreviewImage;
  }

  void addToLastLoadedResults(const QString &path, const QString &jobName);
  void setSideBarVisible(bool visible);
  void setWindowTitlePrefix(const QString &txt);
  void checkForSettingsChanged();
  auto getOutputClasses() -> std::set<joda::enums::ClassId>;

public slots:
  void onBackClicked();

signals:
  void onOutputClassifierChanges();

private:
  enum class AskEnum
  {
    yes,
    no,
    cancel
  };

  /////////////////////////////////////////////////////
  static constexpr int32_t LEFT_TOOLBAR_WIDTH = 350;    // 365

  /////////////////////////////////////////////////////
  enum class Navigation
  {
    START_PAGE = 0,
    REPORTING  = 1
  };

  enum class Tabs
  {
    PROJECT_SETTINGS = 0,
    CLASSIFICATION   = 1,
    PIPELINES        = 2,
    IMAGES           = 3,
  };

  /////////////////////////////////////////////////////
  void createTopToolbar();
  void createLeftToolbar();
  void loadLastOpened();
  void clearSettings();
  bool saveProject(std::filesystem::path filename, bool saveAs = false, bool createHistoryEntry = true);
  void closeEvent(QCloseEvent *event) override;
  AskEnum askForNewProject();

  QWidget *createStackedWidget();
  QWidget *createStartPageWidget();
  QWidget *createChannelWidget();
  QWidget *createReportingWidget();

  static QString bytesToString(int64_t bytes);
  ////Common/////////////////////////////////////////////////
  QToolBar *mTopToolBar;
  joda::ctrl::Controller *mController;
  QFileSystemWatcher mTemplateDirWatcher;
  PanelCompilerLog *mCompilerLog;
  DialogOpenTemplate *mDialogOpenProjectTemplates;

  ////Project settings/////////////////////////////////////////////////
  joda ::settings::AnalyzeSettings *mActAnalyzeSettings = nullptr;
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;
  std::filesystem::path mSelectedProjectSettingsFilePath;
  std::set<joda::enums::ClassId> mOutPutClassesOld;

  ////Left Toolbar/////////////////////////////////////////////////
  std::vector<QDockWidget *> mDockWidgets;

  PanelProjectSettings *mPanelProjectSettings = nullptr;
  PanelClassification *mPanelClassification   = nullptr;
  PanelPipeline *mPanelPipeline               = nullptr;
  PanelImages *mPanelImages                   = nullptr;

  ////Right Dock/////////////////////////////////////////////////
  DialogImageViewer *mPreviewImage = nullptr;

  ////Stacked widget/////////////////////////////////////////////////
  QStackedWidget *mStackedWidget = nullptr;
  Navigation mNavigation         = Navigation::START_PAGE;
  PanelResults *mPanelReporting  = nullptr;

  ////ToolbarIcons/////////////////////////////////////////////////
  QMenu *mOpenProjectMenu           = nullptr;
  QAction *mOpenProjectButton       = nullptr;
  QAction *mSaveProject             = nullptr;
  QAction *mSaveProjectAs           = nullptr;
  QAction *mShowInfoDialog          = nullptr;
  QAction *mStartAnalysisToolButton = nullptr;
  QAction *mShowCompilerLog         = nullptr;

  ////Mutexes/////////////////////////////////////////////////
  std::mutex mCheckForSettingsChangedMutex;

public slots:
  void onNewProjectClicked();
  void onSaveProject();
  void onSaveProjectAs();
  void onStartClicked();
  void onShowInfoDialog();
  void onShowHelpClicked();
  void onOpenClicked();
};

}    // namespace joda::ui::gui
