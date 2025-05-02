///
/// \file      panel_pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
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
class PlaceholderTableWidget;

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;
class DialogCommandSelection;

///
/// \class
/// \author
/// \brief
///
class PanelPipeline : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  explicit PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings);
  void addElement(std::unique_ptr<PanelPipelineSettings> baseContainer, void *pointerToSettings);
  void erase(PanelPipelineSettings *toRemove);
  void clear();
  void loadTemplates();
  void setActionStartEnabled(bool);

  void addChannel(const joda::settings::Pipeline &settings);
  void addChannel(const QString &pathToSettings);
  void addChannel(const nlohmann::json &json);
  auto getPipelineWidgets() -> std::map<std::unique_ptr<PanelPipelineSettings>, void *> &
  {
    return mChannels;
  }

private:
  /////////////////////////////////////////////////////
  void onAddChannel(const QString &path);

  /////////////////////////////////////////////////////
  PlaceholderTableWidget *mPipelineTable;
  std::map<std::unique_ptr<PanelPipelineSettings>, void *> mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings
  WindowMain *mWindowMain;
  joda::settings::AnalyzeSettings &mAnalyzeSettings;
  std::shared_ptr<DialogCommandSelection> mCommandSelectionDialog;

  // ACTIONS///////////////////////////////////////////////////
  QAction *mActionStart;
  QMenu *mTemplatesMenu;

private slots:
  void dropFinishedEvent();
};

}    // namespace joda::ui::gui
