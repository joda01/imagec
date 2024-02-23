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
  void showOverview();
  void showChannelEdit(ContainerChannel *);
  void removeChannel();

private:
  void createToolbar();
  QWidget *createStackedWidget();
  QWidget *createOverviewWidget();
  QWidget *createChannelWidget();

  QStackedWidget *mStackedWidget;
  QGridLayout *mLayoutChannelOverview;
  std::set<std::shared_ptr<ContainerChannel>> mChannels;
  QPushButton *mAddChannelButton;
  QLabel *mLastElement;

private slots:
  void onOpenFolderClicked();
  void onSaveProjectClicked();
  void onStartClicked();
  void onAddChannelClicked();
};

}    // namespace joda::ui::qt
