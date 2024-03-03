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
#include <QtWidgets>
#include <memory>
#include <mutex>
#include <set>
#include "backend/settings/analze_settings_parser.hpp"
#include "controller/controller.hpp"
#include "ui/qt/helper/clickablelabel.hpp"

namespace joda::ui::qt {

class ContainerChannel;

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
  void showChannelEdit(ContainerChannel *);
  void removeChannel(ContainerChannel *toRemove);
  int getSelectedSeries() const
  {
    return mImageSeriesCombo->currentData().toInt();
  }
  int getSelectedFileIndex() const
  {
    if(mFoundFilesCombo->count() > 0) {
      return mFoundFilesCombo->currentData().toInt();
    } else {
      return -1;
    }
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

  nlohmann::json toJson();
  void fromJson(const settings::json::AnalyzeSettings &);
signals:
  void lookingForFilesFinished();

private:
  void createToolbar();
  QWidget *createStackedWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();
  void waitForFileSearchFinished();
  void setWorkingDirectory(const std::string &workingDir);
  ContainerChannel *addChannel();
  void syncColocSettings();

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  QWidget *mAddChannelPanel;
  QLabel *mLastElement;
  QAction *mBackButton;
  joda::ctrl::Controller *mController;
  QComboBox *mFoundFilesCombo;
  QComboBox *mImageSeriesCombo;
  ClickableLabel *mFoundFilesHint;
  std::thread *mMainThread;
  bool mNewFolderSelected = false;

  ////Made project settings/////////////////////////////////////////////////
  ContainerChannel *mSelectedChannel = nullptr;
  std::set<ContainerChannel *> mChannels;
  QString mSelectedWorkingDirectory;
  std::mutex mLookingForFilesMutex;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mFileSelectorComboBox = nullptr;
  QAction *mImageSeriesComboBox  = nullptr;
  QAction *mFileSearchHintLabel  = nullptr;
  QAction *mSaveProject          = nullptr;
  QAction *mOPenProject          = nullptr;
  QAction *mStartAnalysis        = nullptr;
  QAction *mSettings             = nullptr;
  QAction *mDeleteChannel        = nullptr;
  QAction *mShowInfoDialog       = nullptr;
  QAction *mFirstSeparator       = nullptr;
  QAction *mSecondSeparator      = nullptr;

private slots:
  void onOpenProjectClicked();
  void onSaveProjectClicked();
  void onStartClicked();
  void onAddChannelClicked();
  void onBackClicked();
  void onRemoveChannelClicked();
  void onShowInfoDialog();
  QWidget *createAddChannelPanel();
  void onLookingForFilesFinished();
  void onOpenSettingsClicked();
};

}    // namespace joda::ui::qt
