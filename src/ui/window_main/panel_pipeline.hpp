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

///

#pragma once

#include <qcombobox.h>
#include <qwidget.h>
#include <QtWidgets>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"

namespace joda::ui {

class WindowMain;
class PanelPipelineSettings;

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
  void addElement(PanelPipelineSettings *baseContainer, void *pointerToSettings);
  void erase(PanelPipelineSettings *toRemove);
  void clear();

  void addChannel(const joda::settings::Pipeline &settings);
  void addChannel(const QString &pathToSettings);
  void addChannel(const nlohmann::json &json);

private:
  /////////////////////////////////////////////////////
  QVBoxLayout *mVerticalLayout;
  std::map<PanelPipelineSettings *, void *>
      mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings
  WindowMain *mWindowMain;
  joda::settings::AnalyzeSettings &mAnalyzeSettings;
};

}    // namespace joda::ui
