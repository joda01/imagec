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

#include "panel_pipeline.hpp"
#include <qwidget.h>
#include <cmath>
#include <filesystem>
#include <iterator>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "ui/gui/container/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/gui/container/pipeline/panel_channel_overview.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/helper/droppable_widget/droppable_widget.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipeline::PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings) : mWindowMain(windowMain), mAnalyzeSettings(settings)
{
  setFrameStyle(0);
  setFrameShape(QFrame::NoFrame);
  viewport()->setStyleSheet("background-color: transparent;");
  setObjectName("scrollAreaOverview");
  // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Create a widget to hold the panels
  mContentWidget = new DroppableWidget();
  mContentWidget->setObjectName("contentOverview");
  setWidget(mContentWidget);
  setWidgetResizable(true);

  connect(mContentWidget, &DroppableWidget::dropFinished, this, &PanelPipeline::dropFinishedEvent);

  mCommandSelectionDialog = std::make_shared<DialogCommandSelection>(mWindowMain);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///

void PanelPipeline::addElement(std::unique_ptr<PanelPipelineSettings> baseContainer, void *pointerToSettings)
{
  mContentWidget->getLayout()->addWidget(baseContainer->getOverviewPanel());
  mChannels.emplace(std::move(baseContainer), pointerToSettings);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::erase(PanelPipelineSettings *toRemove)
{
  if(toRemove != nullptr) {
    toRemove->setActive(false);
    // Find the iterator to the element using the pointer
    auto it = std::find_if(mChannels.begin(), mChannels.end(), [&toRemove](std::pair<const std::unique_ptr<PanelPipelineSettings>, void *> &entry) {
      return entry.first.get() == toRemove;
    });

    if(it != mChannels.end()) {
      void *elementInSettings = it->second;

      mAnalyzeSettings.pipelines.remove_if([&elementInSettings](const joda::settings::Pipeline &item) { return &item == elementInSettings; });

      mContentWidget->getLayout()->removeWidget(toRemove->getOverviewPanel());
      toRemove->getOverviewPanel()->setParent(nullptr);
      mChannels.erase(it);
      mWindowMain->checkForSettingsChanged();
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::clear()
{
  // Iterate through all items in the layout
  while(QLayoutItem *item = mContentWidget->getLayout()->takeAt(0)) {
    // Check if the item contains a widget
    if(QWidget *widget = item->widget()) {
      // Optionally, remove the widget from the layout and delete it
      widget->setParent(nullptr);    // Remove widget from layout without deleting it
      delete widget;                 // Delete the widget if you don't need it anymore
    }
    delete item;    // Delete the layout item
  }
  mChannels.clear();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const joda::settings::Pipeline &settings)
{
  mAnalyzeSettings.pipelines.push_back(joda::settings::Pipeline{});
  auto &newlyAdded = mAnalyzeSettings.pipelines.back();
  auto panel1      = std::make_unique<PanelPipelineSettings>(mWindowMain, newlyAdded, mCommandSelectionDialog);
  panel1->fromSettings(settings);
  panel1->toSettings();
  addElement(std::move(panel1), &newlyAdded);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const QString &pathToSettings)
{
  try {
    addChannel(joda::templates::TemplateParser::loadChannelFromTemplate(std::filesystem::path(pathToSettings.toStdString())));
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const nlohmann::json &json)
{
  try {
    addChannel(joda::templates::TemplateParser::loadChannelFromTemplate(json));
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::dropFinishedEvent()
{
  auto moveElementToListPosition = [](std::list<joda::settings::Pipeline> &myList, void *elementToMove, size_t newPos) {
    size_t oldPos = 0;
    for(const auto &pip : myList) {
      if(&pip == elementToMove) {
        break;
      }
      oldPos++;
    }
    if(oldPos >= myList.size()) {
      throw std::runtime_error("Cannot mve");
    }
    // Get iterators to the old and new positions
    auto oldIt = std::next(myList.begin(), oldPos);
    auto newIt = std::next(myList.begin(), newPos);
    // Splice the element at oldIt to before newIt
    myList.splice(newIt, myList, oldIt);
  };

  for(size_t i = 0; i < mContentWidget->getLayout()->count(); ++i) {
    auto *toMove = mContentWidget->getLayout()->itemAt(i)->widget();
    auto it = std::find_if(mChannels.begin(), mChannels.end(), [&toMove](std::pair<const std::unique_ptr<PanelPipelineSettings>, void *> &entry) {
      return entry.first.get()->getOverviewPanel() == toMove;
    });

    if(it != mChannels.end()) {
      void *elementToMove = it->second;
      moveElementToListPosition(mAnalyzeSettings.pipelines, elementToMove, i);
    }
  }
  mWindowMain->checkForSettingsChanged();
}

}    // namespace joda::ui::gui
