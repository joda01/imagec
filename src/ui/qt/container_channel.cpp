///
/// \file      container_channel.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A container which holds all functions of a channel
///            The channel container has two possible views:
///              The overview view and the edit view
///

#include "container_channel.hpp"
#include <qwidget.h>
#include <memory>
#include "ui/qt/panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ContainerChannel::ContainerChannel(WindowMain *windowMain)
{
  mChannelName = std::shared_ptr<ContainerFunction<QString>>(
      new ContainerFunction<QString>("icons8-text-50.png", "Name", "Channel Name", ""));

  mChannelSelector = std::shared_ptr<ContainerFunction<int>>(
      new ContainerFunction<int>("icons8-layers-50.png", "Index", "Channel index", "",
                                 {{0, "Channel 1"}, {1, "Channel 2"}, {2, "Channel 3"}, {3, "Channel 4"}}));

  mPanelOverview = std::shared_ptr<PanelChannelOverview>(new PanelChannelOverview(windowMain, this));
  mPanelEdit     = std::shared_ptr<PanelChannelEdit>(new PanelChannelEdit(windowMain, this));
}

}    // namespace joda::ui::qt
