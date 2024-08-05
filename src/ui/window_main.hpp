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
#include "backend/helper/template_parser/template_parser.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/vchannel/vchannel_intersection.hpp"
#include "container/container_base.hpp"
#include "controller/controller.hpp"
#include "ui/helper/clickablelabel.hpp"
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
  bool showStartScreen(bool warnBeforeSwitch);
  void showProjectOverview();
  void removeChannel(ContainerBase *toRemove);
  void removeAllChannels();
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
    if(mJobName->text().isEmpty()) {
      return mJobName->placeholderText().toStdString();
    }
    return mJobName->text().toStdString();
  }

  void setMiddelLabelText(const QString &txt)
  {
    if(mMiddle != nullptr) {
      mMiddle->setText(txt);
    }
  }

signals:
  void lookingForFilesFinished();
  void lookingForTemplateFinished(std::map<std::string, helper::templates::TemplateParser::Data>);

private:
  /////////////////////////////////////////////////////
  static constexpr int32_t OVERVIEW_COLS = 3;
  static constexpr int32_t V_CHANNEL_COL = OVERVIEW_COLS;

  /////////////////////////////////////////////////////
  enum class Navigation
  {
    START_SCREEN     = 0,
    PROJECT_OVERVIEW = 1,
    CHANNEL_EDIT     = 2,
    REPORTING        = 3
  };

  /////////////////////////////////////////////////////
  void createTopToolbar();
  void createBottomToolbar();
  void createRightToolbar();
  void checkForSettingsChanged();
  void resetImageInfo();

  QWidget *createStackedWidget();
  QWidget *createStartWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();
  QWidget *createReportingWidget();

  void waitForFileSearchFinished();
  ContainerBase *addChannel(joda::settings::ChannelSettings);
  ContainerBase *addVChannelVoronoi(joda::settings::VChannelVoronoi);
  ContainerBase *addVChannelIntersection(joda::settings::VChannelIntersection);
  ContainerBase *addChannelFromTemplate(const QString &pathToTemplate);

  static QString bytesToString(int64_t bytes);

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  QWidget *mAddChannelPanel;
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
  QLabel *mMiddle         = nullptr;

  ////Navifation/////////////////////////////////////////////////
  Navigation mNavigation = Navigation::START_SCREEN;

  ////Project settings/////////////////////////////////////////////////
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  joda::settings::AnalyzeSettings mAnalyzeSettingsOld;
  std::map<ContainerBase *, void *>
      mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings

  ////Toolbar/////////////////////////////////////////////////
  QToolBar *mButtomToolbar;
  QToolBar *mRightToolBar;
  QLineEdit *mJobName;

  ////Right Toolbar/////////////////////////////////////////////////
  QTextEdit *mLabelImageInfo;

  ////Made project settings/////////////////////////////////////////////////
  ContainerBase *mSelectedChannel = nullptr;
  std::filesystem::path mSelectedImagesDirectory;
  std::filesystem::path mSelectedProjectSettingsFilePath;
  std::mutex mLookingForFilesMutex;
  PanelReporting *mPanelReporting = nullptr;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mFileSelectorComboBox    = nullptr;
  QAction *mImageSeriesComboBox     = nullptr;
  QAction *mImageResolutionComboBox = nullptr;
  QAction *mFileSearchHintLabel     = nullptr;
  QAction *mSaveProject             = nullptr;
  QAction *mStartAnalysis           = nullptr;
  QAction *mOpenReportingArea       = nullptr;
  QAction *mJobNameAction           = nullptr;
  QAction *mProjectSettings         = nullptr;
  QAction *mDeleteChannel           = nullptr;
  QAction *mShowInfoDialog          = nullptr;
  QAction *mFirstSeparator          = nullptr;
  QAction *mSecondSeparator         = nullptr;

  ////Giraf/////////////////////////////////////////////////
  QPushButton *mUseImageC;
  QPushButton *mUseTheGiraf;
  QMovie *mGiraf;

private slots:
  void onSaveProject();
  void onSaveProjectAsClicked();
  void onStartClicked();
  void onOpenReportingAreaClicked();
  void onAddChannelClicked();
  void onAddCellApproxClicked();
  void onAddIntersectionClicked();
  void onBackClicked();
  void onRemoveChannelClicked();
  void onShowInfoDialog();
  QWidget *createAddChannelPanel();
  void onLookingForFilesFinished();
  void onOpenAnalyzeSettingsClicked();
  void onAddGirafClicked();
  void onImageSelectionChanged();
  void onResolutionChanged();

  void onOpenSettingsDialog();
  void onFindTemplatesFinished(std::map<std::string, helper::templates::TemplateParser::Data>);
};

}    // namespace joda::ui::qt
