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

#include "panel_pipeline.hpp"

namespace joda::ui::qt {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipeline::PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings) :
    mWindowMain(windowMain), mAnalyzeSettings(settings)
{
  setFrameStyle(0);
  setFrameShape(QFrame::NoFrame);
  viewport()->setStyleSheet("background-color: transparent;");
  setObjectName("scrollAreaOverview");
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Create a widget to hold the panels
  auto *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  setWidget(contentWidget);
  setWidgetResizable(true);

  // Create a horizontal layout for the panels
  mVerticalLayout = new QVBoxLayout(contentWidget);
  mVerticalLayout->setContentsMargins(8, 0, 8, 0);
  mVerticalLayout->setSpacing(16);    // Adjust this value as needed
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
void PanelPipeline::addElement(ContainerBase *baseContainer, void *pointerToSettings)
{
  mVerticalLayout->addWidget(baseContainer->getOverviewPanel());
  mChannels.emplace(baseContainer, pointerToSettings);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::erase(ContainerBase *toRemove)
{
  if(toRemove != nullptr) {
    toRemove->setActive(false);
    void *elementInSettings = mChannels.at(toRemove);
    mChannels.erase(toRemove);

    mAnalyzeSettings.channels.remove_if(
        [&elementInSettings](const joda::settings::ChannelSettings &item) { return &item == elementInSettings; });

    mAnalyzeSettings.vChannels.remove_if(
        [&elementInSettings](const joda::settings::VChannelSettings &item) { return &item == elementInSettings; });

    mVerticalLayout->removeWidget(toRemove->getOverviewPanel());
    toRemove->getOverviewPanel()->setParent(nullptr);
    delete toRemove;
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
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const joda::settings::ChannelSettings &settings)
{
  mAnalyzeSettings.channels.push_back(settings);
  auto &newlyAdded = mAnalyzeSettings.channels.back();
  auto *panel1     = new ContainerChannel(mWindowMain, newlyAdded);
  panel1->fromSettings();
  panel1->toSettings();
  addElement(panel1, &newlyAdded);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const joda::settings::VChannelIntersection &settings)
{
  mAnalyzeSettings.vChannels.push_back(joda::settings::VChannelSettings{.$intersection = settings});
  auto &newlyAdded = mAnalyzeSettings.vChannels.back();
  auto *panel1     = new ContainerIntersection(mWindowMain, newlyAdded.$intersection.value());
  panel1->fromSettings();
  panel1->toSettings();
  addElement(panel1, &newlyAdded);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const joda::settings::VChannelVoronoi &settings)
{
  mAnalyzeSettings.vChannels.push_back(joda::settings::VChannelSettings{.$voronoi = settings});
  auto &newlyAdded = mAnalyzeSettings.vChannels.back();
  auto *panel1     = new ContainerVoronoi(mWindowMain, newlyAdded.$voronoi.value());
  panel1->fromSettings();
  panel1->toSettings();
  addElement(panel1, &newlyAdded);
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
    std::ifstream ifs(pathToSettings.toStdString());
    settings::ChannelSettings settings = nlohmann::json::parse(ifs);
    addChannel(settings);
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    auto *icon = new QIcon(":/icons/outlined/icons8-warning-50.png");
    messageBox.setIconPixmap(icon->pixmap(42, 42));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

}    // namespace joda::ui::qt
