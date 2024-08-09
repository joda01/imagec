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
/// \brief     A short description what happens here.
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
#include "backend/helper/template_parser/template_parser.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/experiment_settings.hpp"
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include "controller/controller.hpp"
#include "ui/container/container_base.hpp"
#include "ui/helper/clickablelabel.hpp"
#include "ui/window_main/panel_image.hpp"
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
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
  void showChannelEdit(ContainerBase *);
  bool showProjectOverview();

  joda::ctrl::Controller *getController()
  {
    return mController;
  }

  [[nodiscard]] auto getJobName() const -> QString
  {
    return mPanelProjectSettings->getJobName();
  }

  [[nodiscard]] auto getExperimentSettings() const -> const joda::settings::ExperimentSettings &
  {
    return mAnalyzeSettings.experimentSettings;
  }

  [[nodiscard]] const PanelImages *getImagePanel() const
  {
    return mPanelImages;
  }

  void setWindowTitlePrefix(const QString &txt);
  void checkForSettingsChanged();

public slots:
  void onBackClicked();
  void onRemoveChannelClicked();

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t OVERVIEW_COLS = 1;
  static constexpr int32_t V_CHANNEL_COL = OVERVIEW_COLS;

  /////////////////////////////////////////////////////
  enum class Navigation
  {
    PROJECT_OVERVIEW = 0,
    CHANNEL_EDIT     = 1,
    REPORTING        = 2
  };

  /////////////////////////////////////////////////////
  void createTopToolbar();
  void createLeftToolbar();
  void loadTemplates();

  void openProjectSettings(const QString &filePath);
  void openResultsSettings(const QString &filePath);

  QWidget *createStackedWidget();
  QWidget *createChannelWidget();
  QWidget *createReportingWidget();

  static QString bytesToString(int64_t bytes);

  ////Common/////////////////////////////////////////////////
  joda::ctrl::Controller *mController;

  ////Project settings/////////////////////////////////////////////////
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;
  std::filesystem::path mSelectedProjectSettingsFilePath;

  ////Left Toolbar/////////////////////////////////////////////////
  QToolBar *mSidebar;
  PanelProjectSettings *mPanelProjectSettings;
  PanelPipeline *mPanelPipeline;
  PanelImages *mPanelImages;
  QPushButton *mStartAnalysis = nullptr;

  ////Stacked widget/////////////////////////////////////////////////
  QStackedWidget *mStackedWidget;
  Navigation mNavigation          = Navigation::PROJECT_OVERVIEW;
  ContainerBase *mSelectedChannel = nullptr;
  PanelResults *mPanelReporting   = nullptr;

  ////Sidebar panels/////////////////////////////////////////////////
  QComboBox *mTemplateSelection;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mNewProjectButton  = nullptr;
  QAction *mOpenProjectButton = nullptr;
  QAction *mSaveProject       = nullptr;
  QAction *mShowInfoDialog    = nullptr;

private slots:
  void onAddChannel();
  void onSaveProject();
  void onSaveProjectAsClicked();
  void onStartClicked();
  void onShowInfoDialog();
  void onOpenClicked();
};

}    // namespace joda::ui::qt
