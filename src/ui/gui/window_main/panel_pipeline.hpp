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
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"

class DroppableWidget;

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;
class DialogCommandSelection;

///
/// \class
/// \author
/// \brief
///
class PanelPipeline : public QScrollArea
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings);
  void addElement(std::unique_ptr<PanelPipelineSettings> baseContainer, void *pointerToSettings);
  void erase(PanelPipelineSettings *toRemove);
  void clear();

  void addChannel(const joda::settings::Pipeline &settings);
  void addChannel(const QString &pathToSettings);
  void addChannel(const nlohmann::json &json);

private:
  /////////////////////////////////////////////////////
  DroppableWidget *mContentWidget;
  std::map<std::unique_ptr<PanelPipelineSettings>, void *> mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings
  WindowMain *mWindowMain;
  joda::settings::AnalyzeSettings &mAnalyzeSettings;
  std::shared_ptr<DialogCommandSelection> mCommandSelectionDialog;

private slots:
  void dropFinishedEvent();
};

}    // namespace joda::ui::gui
