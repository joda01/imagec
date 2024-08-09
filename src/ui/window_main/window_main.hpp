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
#include "ui/window_main/panel_pipeline.hpp"
#include "ui/window_main/panel_project_settings.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class ContainerChannel;
class PanelReporting;

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
  void setWorkingDirectory(const std::string &workingDir);
  int getSelectedSeries() const
  {
    return mImageSeriesCombo->currentData().toInt();
  }
  int getSelectedFileIndex() const
  {
    if(mFoundFilesCombo->count() > 0) {
      return mFoundFilesCombo->currentData().toInt();
    }
    return -1;
  }
  joda::ctrl::Controller *getController()
  {
    return mController;
  }

  [[nodiscard]] const QComboBox *getFoundFilesCombo() const
  {
    return mFoundFilesCombo;
  }
  [[nodiscard]] const QComboBox *getImageSeriesCombo() const
  {
    return mImageSeriesCombo;
  }

  [[nodiscard]] const QComboBox *getImageResolutionCombo() const
  {
    return mImageResolutionCombo;
  }

  [[nodiscard]] auto getJobName() const -> std::string
  {
    return mJobName.toStdString();
  }

  [[nodiscard]] auto getExperimentSettings() const -> const joda::settings::ExperimentSettings &
  {
    return mAnalyzeSettings.experimentSettings;
  }

  void setWindowTitlePrefix(const QString &txt);
  void checkForSettingsChanged();

signals:
  void lookingForFilesFinished();
  void lookingForTemplateFinished(std::map<std::string, joda::helper::templates::TemplateParser::Data>);

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
  void createBottomToolbar();
  void createLeftToolbar();
  void resetImageInfo();

  QWidget *createStackedWidget();
  QWidget *createChannelWidget();
  QWidget *createReportingWidget();

  void waitForFileSearchFinished();

  static QString bytesToString(int64_t bytes);

  QStackedWidget *mStackedWidget;
  QLabel *mLastElement;
  QAction *mBackButton;
  joda::ctrl::Controller *mController;
  QComboBox *mFoundFilesCombo;
  QComboBox *mImageSeriesCombo;
  QComboBox *mImageResolutionCombo;

  QComboBox *mTemplateSelection;
  ClickableLabel *mFoundFilesHint;
  std::thread *mMainThread;
  bool mNewFolderSelected = false;

  ////Navigation/////////////////////////////////////////////////
  Navigation mNavigation = Navigation::PROJECT_OVERVIEW;

  ////Project settings/////////////////////////////////////////////////
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;

  ////Toolbar/////////////////////////////////////////////////
  QToolBar *mButtomToolbar;
  QToolBar *mSidebar;
  QString mJobName;

  ////Left Toolbar/////////////////////////////////////////////////
  QTableWidget *mLabelImageInfo;
  PanelProjectSettings *mPanelProjectSettings;
  PanelPipeline *mPanelPipeline;

  ////Made project settings/////////////////////////////////////////////////
  ContainerBase *mSelectedChannel = nullptr;
  std::filesystem::path mSelectedImagesDirectory;
  std::filesystem::path mSelectedProjectSettingsFilePath;
  std::mutex mLookingForFilesMutex;
  PanelReporting *mPanelReporting = nullptr;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mNewProjectButton  = nullptr;
  QAction *mOpenProjectButton = nullptr;

  QAction *mFileSelectorComboBox    = nullptr;
  QAction *mImageSeriesComboBox     = nullptr;
  QAction *mImageResolutionComboBox = nullptr;
  QAction *mFileSearchHintLabel     = nullptr;
  QAction *mSaveProject             = nullptr;
  QAction *mStartAnalysis           = nullptr;
  QAction *mDeleteChannel           = nullptr;
  QAction *mShowInfoDialog          = nullptr;
  QAction *mFirstSeparator          = nullptr;
  QAction *mSecondSeparator         = nullptr;

  ////Giraf/////////////////////////////////////////////////
  QPushButton *mUseImageC;
  QPushButton *mUseTheGiraf;
  QMovie *mGiraf;

private slots:
  void onAddChannel();
  void onSaveProject();
  void onSaveProjectAsClicked();
  void onStartClicked();
  void onOpenReportingAreaClicked();
  void onBackClicked();
  void onRemoveChannelClicked();
  void onShowInfoDialog();
  void onLookingForFilesFinished();
  void onOpenAnalyzeSettingsClicked();
  void onAddGirafClicked();
  void onImageSelectionChanged();
  void onFindTemplatesFinished(std::map<std::string, joda::helper::templates::TemplateParser::Data>);
};

}    // namespace joda::ui::qt
