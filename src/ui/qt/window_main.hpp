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

#include <QtWidgets>
#include <memory>
#include <set>

namespace joda::ui::qt {

class ContainerChannel;

///
/// \class
/// \author
/// \brief
///
class WindowMain : public QMainWindow
{
public:
  WindowMain();
  void showChannelEdit(ContainerChannel *);
  void removeChannel(ContainerChannel *toRemove);

private:
  void createToolbar();
  QWidget *createStackedWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  std::set<ContainerChannel *> mChannels;
  QWidget *mAddChannelPanel;
  QLabel *mLastElement;
  QAction *mBackButton;

  ContainerChannel *mSelectedChannel = nullptr;

  ////ToolbarIcons/////////////////////////////////////////////////
  QAction *mSaveProject     = nullptr;
  QAction *mOPenProject     = nullptr;
  QAction *mStartAnalysis   = nullptr;
  QAction *mSettings        = nullptr;
  QAction *mDeleteChannel   = nullptr;
  QAction *mFirstSeparator  = nullptr;
  QAction *mSecondSeparator = nullptr;

private slots:
  void onOpenFolderClicked();
  void onSaveProjectClicked();
  void onStartClicked();
  void onAddChannelClicked();
  void onBackClicked();
  void onRemoveChannelClicked();
  QWidget *createAddChannelPanel();
};

}    // namespace joda::ui::qt
