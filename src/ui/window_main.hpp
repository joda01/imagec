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
  void removeChannel(ContainerBase *toRemove);
  void removeAllChannels();
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
  [[nodiscard]] const QComboBox *getImageTilesCombo() const
  {
    return mImageTilesCombo;
  }

  [[nodiscard]] auto getJobName() const -> std::string
  {
    if(mJobName->text().isEmpty()) {
      return mJobName->placeholderText().toStdString();
    }
    return mJobName->text().toStdString();
  }

signals:
  void lookingForFilesFinished();
  void lookingForTemplateFinished(std::map<std::string, joda::settings::templates::TemplateParser::Data>);

private:
  void createToolbar();
  QWidget *createStackedWidget();
  QWidget *createGirafWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();
  void waitForFileSearchFinished();
  void setWorkingDirectory(const std::string &workingDir);
  ContainerBase *addChannel(joda::settings::ChannelSettings);
  ContainerBase *addVChannelVoronoi(joda::settings::VChannelVoronoi);
  ContainerBase *addVChannelIntersection(joda::settings::VChannelIntersection);
  ContainerBase *addChannelFromTemplate(const QString &pathToTemplate);

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  QWidget *mAddChannelPanel;
  QLabel *mLastElement;
  QAction *mBackButton;
  joda::ctrl::Controller *mController;
  QComboBox *mFoundFilesCombo;
  QComboBox *mImageSeriesCombo;
  QComboBox *mImageTilesCombo;
  QComboBox *mTemplateSelection;
  ClickableLabel *mFoundFilesHint;
  std::thread *mMainThread;
  bool mNewFolderSelected = false;

  ////Project settings/////////////////////////////////////////////////
  joda::settings::AnalyzeSettings mAnalyzeSettings;
  std::map<ContainerBase *, void *>
      mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings

  ////Toolbar/////////////////////////////////////////////////
  QLineEdit *mJobName;

  ////Made project settings/////////////////////////////////////////////////
  ContainerBase *mSelectedChannel = nullptr;
  QString mSelectedWorkingDirectory;
  std::mutex mLookingForFilesMutex;
  QWidget *mGirafWidget;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mFileSelectorComboBox = nullptr;
  QAction *mImageSeriesComboBox  = nullptr;
  QAction *mImageTilesComboBox   = nullptr;
  QAction *mFileSearchHintLabel  = nullptr;
  QAction *mSaveProject          = nullptr;
  QAction *mOPenProject          = nullptr;
  QAction *mStartAnalysis        = nullptr;
  QAction *mJobNameAction        = nullptr;
  QAction *mSettings             = nullptr;
  QAction *mDeleteChannel        = nullptr;
  QAction *mShowInfoDialog       = nullptr;
  QAction *mFirstSeparator       = nullptr;
  QAction *mSecondSeparator      = nullptr;

  ////Giraf/////////////////////////////////////////////////
  QPushButton *mUseImageC;
  QPushButton *mUseTheGiraf;
  QMovie *mGiraf;

private slots:
  void onOpenProjectClicked();
  void onSaveProjectClicked();
  void onStartClicked();
  void onAddChannelClicked();
  void onAddCellApproxClicked();
  void onAddIntersectionClicked();
  void onBackClicked();
  void onRemoveChannelClicked();
  void onShowInfoDialog();
  QWidget *createAddChannelPanel();
  void onLookingForFilesFinished();
  void onOpenAnalyzeSettingsClicked();
  void onOpenSettingsDialog();
  void onFindTemplatesFinished(std::map<std::string, joda::settings::templates::TemplateParser::Data>);
  void onTakeTheGirafClicked();
  void onUseImageCClicked();
};

}    // namespace joda::ui::qt
