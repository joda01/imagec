///
/// \file      window_main.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qcombobox.h>
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
#include "ui/container/container_base.hpp"
#include "ui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/helper/clickablelabel.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/pipeline_compile_log/panel_pipeline_compile_log.hpp"
#include "ui/window_main/panel_classification.hpp"
#include "ui/window_main/panel_image.hpp"
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
#include "ui/window_main/panel_results_info.hpp"
#include <duckdb/function/table_function.hpp>
#include <nlohmann/json_fwd.hpp>

namespace joda::ui {

class ContainerChannel;
class PanelResults;

///
/// \class
/// \author
/// \brief
///
class WindowMain : public QMainWindow
{
  Q_OBJECT

public:
  WindowMain(joda::ctrl::Controller *controller);
  ~WindowMain()
  {
  }
  bool showPanelStartPage();
  void showPanelPipelineSettingsEdit(PanelPipelineSettings *);
  void showPanelResults();

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

  [[nodiscard]] PanelResultsInfo *getPanelResultsInfo()
  {
    return mPanelResultsInfo;
  }

  [[nodiscard]] PanelPipeline *getPanelPipeline()
  {
    return mPanelPipeline;
  }

  [[nodiscard]] PanelClassification *getPanelClassification()
  {
    return mPanelClassification;
  }

  [[nodiscard]] PanelResults *getPanelResults()
  {
    return mPanelReporting;
  }

  const settings::AnalyzeSettings &getSettings()
  {
    return mAnalyzeSettings;
  }

  void setWindowTitlePrefix(const QString &txt);
  void checkForSettingsChanged();
  auto getOutputClasses() -> std::set<settings::ClassificatorSettingOut>;

public slots:
  void onBackClicked();
  void onRemoveChannelClicked();

signals:
  void onOutputClassifierChanges();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t LEFT_TOOLBAR_WIDTH = 400;    // 365

  /////////////////////////////////////////////////////
  enum class Navigation
  {
    START_PAGE   = 0,
    CHANNEL_EDIT = 1,
    REPORTING    = 2
  };

  enum class Tabs
  {
    PROJECT_SETTINGS = 0,
    CLASSIFICATION   = 1,
    PIPELINES        = 2,
    IMAGES           = 3,
    RESULTS          = 4
  };

  /////////////////////////////////////////////////////
  void createTopToolbar();
  void createLeftToolbar();
  void loadTemplates();
  void clearSettings();
  void saveProject(std::filesystem::path filename);

  void openProjectSettings(const QString &filePath);
  void openResultsSettings(const QString &filePath);

  QWidget *createStackedWidget();
  QWidget *createStartPageWidget();
  QWidget *createChannelWidget();
  QWidget *createReportingWidget();

  static QString bytesToString(int64_t bytes);

  ////Common/////////////////////////////////////////////////
  joda::ctrl::Controller *mController;
  QFileSystemWatcher mTemplateDirWatcher;
  PanelCompilerLog *mCompilerLog;

  ////Project settings/////////////////////////////////////////////////
  joda ::settings::AnalyzeSettings *mActAnalyzeSettings = nullptr;
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;
  std::filesystem::path mSelectedProjectSettingsFilePath;

  ////Left Toolbar/////////////////////////////////////////////////
  QToolBar *mSidebar;
  QTabWidget *mTabWidget;
  PanelProjectSettings *mPanelProjectSettings;
  PanelClassification *mPanelClassification;
  PanelPipeline *mPanelPipeline;
  PanelImages *mPanelImages;
  PanelResultsInfo *mPanelResultsInfo;
  QPushButton *mStartAnalysis = nullptr;

  ////Stacked widget/////////////////////////////////////////////////
  QStackedWidget *mStackedWidget;
  Navigation mNavigation                  = Navigation::START_PAGE;
  PanelPipelineSettings *mSelectedChannel = nullptr;
  PanelResults *mPanelReporting           = nullptr;

  ////Sidebar panels/////////////////////////////////////////////////
  QComboBox *mTemplateSelection;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mNewProjectButton        = nullptr;
  QAction *mOpenProjectButton       = nullptr;
  QAction *mSaveProject             = nullptr;
  QAction *mSaveProjectAs           = nullptr;
  QAction *mShowInfoDialog          = nullptr;
  QAction *mStartAnalysisToolButton = nullptr;

  ////Mutexes/////////////////////////////////////////////////
  std::mutex mCheckForSettingsChangedMutex;

private slots:
  void onNewProjectClicked();
  void onAddChannel();
  void onSaveProject();
  void onSaveProjectAs();
  void onSaveProjectAsClicked();
  void onStartClicked();
  void onShowInfoDialog();
  void onShowHelpClicked();
  void onOpenClicked();
};

}    // namespace joda::ui
