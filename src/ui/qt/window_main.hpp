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
#include <set>
#include "ui/qt/panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \class
/// \author
/// \brief
///
class WindowMain : public QMainWindow
{
public:
  WindowMain();
  void showOverview();
  void showChannelEdit(PanelChannelOverview *);
  void removeChannel();

private:
  void createToolbar();
  QWidget *createStackedWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  std::set<PanelChannelOverview *> mChannels;
  QPushButton *mAddChannelButton;
  QLabel *mLastElement;

  PanelChannelOverview *mSelectedChannel = nullptr;

private slots:
  void onOpenFolderClicked();
  void onSaveProjectClicked();
  void onStartClicked();
  void onAddChannelClicked();
};

}    // namespace joda::ui::qt
