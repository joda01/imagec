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
#include <filesystem>
#include <memory>
#include "backend/settings/pipeline/pipeline.hpp"
#include "ui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/helper/template_parser/template_parser.hpp"
#include "ui/window_main/window_main.hpp"

namespace joda::ui {

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
  auto *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  setWidget(contentWidget);
  setWidgetResizable(true);

  // Create a horizontal layout for the panels
  mVerticalLayout = new QVBoxLayout(contentWidget);
  mVerticalLayout->setContentsMargins(0, 0, 0, 0);
  mVerticalLayout->setSpacing(8);    // Adjust this value as needed
  mVerticalLayout->setAlignment(Qt::AlignTop);
  contentWidget->setLayout(mVerticalLayout);
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
  mVerticalLayout->addWidget(baseContainer->getOverviewPanel());
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

      mVerticalLayout->removeWidget(toRemove->getOverviewPanel());
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
  while(QLayoutItem *item = mVerticalLayout->takeAt(0)) {
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
  auto panel1      = std::make_unique<PanelPipelineSettings>(mWindowMain, newlyAdded);
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
    auto *icon = new QIcon(":/icons/icons/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
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
    auto *icon = new QIcon(":/icons/icons/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

}    // namespace joda::ui
