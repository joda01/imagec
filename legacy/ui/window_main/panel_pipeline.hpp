///
/// \file      panel_pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
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
#include "backend/helper/template_parser/template_parser.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/vchannel/vchannel_voronoi_settings.hpp"
#include "ui/container/channel/container_channel.hpp"
#include "ui/container/container_base.hpp"
#include "ui/container/intersection/container_intersection.hpp"
#include "ui/container/voronoi/container_voronoi.hpp"

namespace joda::ui::qt {

class WindowMain;

///
/// \class
/// \author
/// \brief
///
class PanelPipeline : public QScrollArea
{
public:
  /////////////////////////////////////////////////////
  explicit PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings);
  void addElement(ContainerBase *baseContainer, void *pointerToSettings);
  void erase(ContainerBase *toRemove);
  void clear();

  void addChannel(const joda::settings::ChannelSettings &settings);
  void addChannel(const joda::settings::VChannelIntersection &settings);
  void addChannel(const joda::settings::VChannelVoronoi &settings);
  void addChannel(const QString &pathToSettings);
  void addChannel(const nlohmann::json &json);

private:
  /////////////////////////////////////////////////////
  void addChannel(const joda::helper::templates::TemplateParser::LoadedChannel &loaded);

  /////////////////////////////////////////////////////
  QVBoxLayout *mVerticalLayout;
  std::map<ContainerBase *, void *>
      mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings
  WindowMain *mWindowMain;
  joda::settings::AnalyzeSettings &mAnalyzeSettings;
};

}    // namespace joda::ui::qt
