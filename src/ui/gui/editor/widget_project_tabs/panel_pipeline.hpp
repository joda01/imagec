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
#include <qtableview.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QtWidgets>
#include <memory>
#include <mutex>
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/pipeline/pipeline.hpp"

class DroppableWidget;
class PlaceholderTableView;

namespace joda::ui::gui {

class WindowMain;
class PanelPipelineSettings;
class DialogCommandSelection;
class TableModelPipeline;

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
  void addElement(std::unique_ptr<PanelPipelineSettings> baseContainer);
  void erase(PanelPipelineSettings *toRemove);
  void clear();
  void saveAsTemplate();
  void loadTemplates();
  void toSettings();
  void fromSettings(const joda::settings::AnalyzeSettings &settings);

  void addChannelFromSettings(joda::settings::Pipeline settings);
  void addChannelFromPath(const QString &pathToSettings);
  void addChannelFromJson(const nlohmann::json &json);
  auto getPipelineWidgets() -> std::set<std::unique_ptr<PanelPipelineSettings>> &
  {
    return mChannels;
  }
  void unselectPipeline();
  auto getSelectedPipeline() -> PanelPipelineSettings *;

private:
  /////////////////////////////////////////////////////
  void onAddChannel(const QString &path);
  void movePipelineToPosition(size_t fromPos, size_t newPos);
  void openSelectedPipelineSettings(int32_t selectedRow);
  void removePipelineWidget();
  void updatePipelineCommandUnits();

  /////////////////////////////////////////////////////
  PlaceholderTableView *mPipelineTable;
  TableModelPipeline *mTableModel;
  std::set<std::unique_ptr<PanelPipelineSettings>> mChannels;    // The second value is the pointer to the array entry in the AnalyzeSettings
  WindowMain *mWindowMain;
  joda::settings::AnalyzeSettings &mAnalyzeSettings;
  std::shared_ptr<DialogCommandSelection> mCommandSelectionDialog;
  PanelPipelineSettings *mActivePipeline;

  // Stack options ///////////////////////////////////////////
  QDialog *mStackOptionsDialog;
  QComboBox *mStackHandlingZ;
  QComboBox *mStackHandlingT;
  QComboBox *mMeasureUnit;
  QLineEdit *mTStackFrameStart;
  QLineEdit *mTStackFrameEnd;

  // LAYOUT //////////////////////////////////////////////////
  QVBoxLayout *mMainLayout;

  // ACTIONS///////////////////////////////////////////////////
  QMenu *mTemplatesMenu;

  std::mutex mClosePipelineMutex;

private slots:
  void openSelectedPipeline(const QModelIndex &current, const QModelIndex &previous);
  void openSelectedPipelineSettings(const QModelIndex &current);
  void moveUp();
  void moveDown();
};

}    // namespace joda::ui::gui
