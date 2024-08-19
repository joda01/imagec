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
#include "controller/controller.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/helper/clickablelabel.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/window_main/panel_classification.hpp"
#include "ui/window_main/panel_image.hpp"
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
#include "ui/window_main/panel_results_info.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

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

  [[nodiscard]] PanelResults *getPanelResults()
  {
    return mPanelReporting;
  }

  void setWindowTitlePrefix(const QString &txt);
  void checkForSettingsChanged();

public slots:
  void onBackClicked();
  void onRemoveChannelClicked();

private:
  /////////////////////////////////////////////////////
  enum class Navigation
  {
    START_PAGE   = 0,
    CHANNEL_EDIT = 1,
    REPORTING    = 2
  };

  /////////////////////////////////////////////////////
  void createTopToolbar();
  void createLeftToolbar();
  void loadTemplates();

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

  ////Project settings/////////////////////////////////////////////////
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;
  std::filesystem::path mSelectedProjectSettingsFilePath;

  ////Left Toolbar/////////////////////////////////////////////////
  QToolBar *mSidebar;
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
  QAction *mNewProjectButton  = nullptr;
  QAction *mOpenProjectButton = nullptr;
  QAction *mSaveProject       = nullptr;
  QAction *mShowInfoDialog    = nullptr;

private slots:
  void onNewProjectClicked();
  void onAddChannel();
  void onSaveProject();
  void onSaveProjectAsClicked();
  void onStartClicked();
  void onShowInfoDialog();
  void onShowHelpClicked();
  void onOpenClicked();
};

}    // namespace joda::ui::qt
