///
/// \file      container_channel.hpp
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

#pragma once
#include <QtWidgets>
#include <memory>
#include "ui/qt/container_function.hpp"
#include "ui/qt/panel_channel_edit.hpp"
#include "ui/qt/panel_channel_overview.hpp"

namespace joda::ui::qt {

///
/// \class      ContainerChannel
/// \author     Joachim Danmayr
/// \brief
///
class ContainerChannel : std::enable_shared_from_this<ContainerChannel>
{
  friend class PanelChannelOverview;
  friend class PanelChannelEdit;

public:
  /////////////////////////////////////////////////////
  ContainerChannel(WindowMain *windowMain);
  std::shared_ptr<PanelChannelOverview> getOverviewPanel()
  {
    return mPanelOverview;
  }
  std::shared_ptr<PanelChannelEdit> getEditPanel()
  {
    return mPanelEdit;
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<ContainerFunction<QString>> mChannelName;
  std::shared_ptr<ContainerFunction<int>> mChannelSelector;
  std::shared_ptr<PanelChannelOverview> mPanelOverview;
  std::shared_ptr<PanelChannelEdit> mPanelEdit;
};

}    // namespace joda::ui::qt
