///
/// \file      panel_channel.cpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "panel_reporting.hpp"
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include "../window_main.hpp"
#include "backend/pipelines/reporting/reporting_generator.hpp"
#include "backend/pipelines/reporting/reporting_heatmap.hpp"
#include "backend/results/results.hpp"

namespace joda::ui::qt {

using namespace std::chrono_literals;
using namespace std::filesystem;

PanelReporting::PanelReporting(WindowMain *wm) : mWindowMain(wm)
{
  // setStyleSheet("border: 1px solid black; padding: 10px;");
  setObjectName("PanelReporting");

  auto *horizontalLayout             = createLayout();
  auto [verticalLayoutContainer, _1] = addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 16);
  auto [verticalLayoutXlsx, _2]      = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");

  //
  // Excel export
  //
  verticalLayoutXlsx->addWidget(createTitle("Excel report"));
  {
    QPushButton *exportXlsx = new QPushButton("Create XLSX report");
    const QIcon icon(":/icons/outlined/icons8-export-excel-50.png");
    exportXlsx->setIconSize({16, 16});
    exportXlsx->setIcon(icon);
    connect(exportXlsx, &QPushButton::pressed, this, &PanelReporting::onExportToXlsxClicked);
    verticalLayoutXlsx->addWidget(exportXlsx);
  }
  _2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  //
  // Heatmap
  //
  auto [verticalLayoutHeatmap, _11] = addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");
  verticalLayoutHeatmap->addWidget(createTitle("Heatmap"));
  _11->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  verticalLayoutContainer->addStretch(0);

  //
  // Add layout
  //
  setLayout(horizontalLayout);
  horizontalLayout->addStretch();
}

PanelReporting::~PanelReporting()
{
}

void PanelReporting::setActualSelectedResultsFolder(const QString &folder)
{
  mWindowMain->setMiddelLabelText(folder);
  mActualSelectedResultsFolder = folder;
}

///
/// \brief      Export to xlsx
/// \author     Joachim Danmayr
///
void PanelReporting::onExportToXlsxClicked()
{
  // Write summary
  /*
joda::pipeline::reporting::ReportGenerator::flushReportToFile(
    mAnalyzeSettings, alloverReport, resultsFile + ".xlsx",
    {.jobName = mJobName, .timeStarted = timeStarted, .timeFinished = timeStopped},
    joda::pipeline::reporting::ReportGenerator::OutputFormat::HORIZONTAL, true);
if(mAnalyzeSettings.experimentSettings.generateHeatmapForPlate) {
  auto wellOrder = mAnalyzeSettings.experimentSettings.generateHeatmapForWell
                       ? mAnalyzeSettings.experimentSettings.wellImageOrder
                       : std::vector<std::vector<int32_t>>();
  resultsFile    = mOutputFolder + separator + "heatmap_summary_" + mJobName + ".xlsx";
  joda::pipeline::reporting::Heatmap::createAllOverHeatMap(mAnalyzeSettings, alloverReport, mOutputFolder,
                                                           resultsFile, mJobName, wellOrder);
}*/
  std::vector<std::filesystem::path> imageResults;
  std::vector<std::filesystem::path> summaryResults;

  for(recursive_directory_iterator i(mActualSelectedResultsFolder.toStdString()), end; i != end; ++i) {
    try {
      if(!is_directory(i->path())) {
        auto ext = i->path().extension().string();
        if(ext == ".json") {
          std::filesystem::path path = i->path();
          if(path.filename().string().starts_with("results_summary")) {
            summaryResults.push_back(path);
          } else {
            imageResults.push_back(path);
          }
        }
      }
    } catch(const std::exception &ex) {
      std::cout << ex.what() << std::endl;
    }
  }

  // Details summary
  for(const auto &detailResultPath : imageResults) {
    joda::results::WorkSheet details;
    details.loadFromFile(detailResultPath.string());

    std::string outputFolder =
        detailResultPath.parent_path().string() + "/../reports/" + detailResultPath.filename().string() + ".xlsx";
    joda::pipeline::reporting::ReportGenerator::flushReportToFile(
        details, {}, outputFolder, joda::pipeline::reporting::ReportGenerator::OutputFormat::VERTICAL, false);
  }
  /* if(mAnalyzeSettings.experimentSettings.generateHeatmapForImage) {
     joda::pipeline::reporting::Heatmap::createHeatMapForImage(
         mAnalyzeSettings, detailReport, propsOut.props.width, propsOut.props.height,
         detailOutputFolder + separator + "heatmap_image_" + mJobName + ".xlsx");
   }*/
}

/////////////////////////////////////////////////////////////////////////////
//
QLabel *PanelReporting::createTitle(const QString &title)
{
  auto *label = new QLabel();
  QFont font;
  font.setPixelSize(16);
  font.setBold(true);
  label->setFont(font);
  label->setText(title);

  return label;
}

QHBoxLayout *PanelReporting::createLayout()
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setObjectName("scrollArea");
  scrollArea->setStyleSheet("QScrollArea#scrollArea { background-color: rgba(0, 0, 0, 0);}");
  scrollArea->setFrameStyle(0);
  scrollArea->setContentsMargins(0, 0, 0, 0);
  scrollArea->verticalScrollBar()->setStyleSheet(
      "QScrollBar:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    width: 6px;"
      "    margin: 0px 0px 0px 0px;"
      "}"
      "QScrollBar::handle:vertical {"
      "    background: rgba(32, 27, 23, 0.6);"
      "    min-height: 20px;"
      "    border-radius: 12px;"
      "}"
      "QScrollBar::add-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: bottom;"
      "    subcontrol-origin: margin;"
      "}"
      "QScrollBar::sub-line:vertical {"
      "    border: none;"
      "    background: rgba(0, 0, 0, 0);"
      "    height: 20px;"
      "    subcontrol-position: top;"
      "    subcontrol-origin: margin;"
      "}");

  // Create a widget to hold the panels
  QWidget *contentWidget = new QWidget;
  contentWidget->setObjectName("contentOverview");
  contentWidget->setStyleSheet("QWidget#contentOverview { background-color: rgb(251, 252, 253);}");

  scrollArea->setWidget(contentWidget);
  scrollArea->setWidgetResizable(true);

  // Create a horizontal layout for the panels
  QHBoxLayout *horizontalLayout = new QHBoxLayout(contentWidget);
  horizontalLayout->setContentsMargins(16, 16, 16, 16);
  horizontalLayout->setSpacing(16);    // Adjust this value as needed
  contentWidget->setLayout(horizontalLayout);
  return horizontalLayout;
}

std::tuple<QVBoxLayout *, QWidget *> PanelReporting::addVerticalPanel(QLayout *horizontalLayout, const QString &bgColor,
                                                                      int margin, bool enableScrolling, int maxWidth,
                                                                      int spacing) const
{
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setSpacing(spacing);
  QWidget *contentWidget = new QWidget();

  layout->setContentsMargins(margin, margin, margin, margin);
  layout->setAlignment(Qt::AlignTop);

  contentWidget->setObjectName("verticalContentChannel");
  contentWidget->setLayout(layout);
  contentWidget->setStyleSheet(
      "QWidget#verticalContentChannel { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: " +
      bgColor + ";}");

  if(enableScrolling) {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scrollArea->setObjectName("scrollArea");
    scrollArea->setStyleSheet("QScrollArea#scrollArea { background-color: rgba(0, 0, 0, 0);}");
    scrollArea->setFrameStyle(0);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    width: 6px;"
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgba(32, 27, 23, 0.6);"
        "    min-height: 20px;"
        "    border-radius: 12px;"
        "}"
        "QScrollBar::add-line:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    height: 20px;"
        "    subcontrol-position: bottom;"
        "    subcontrol-origin: margin;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: rgba(0, 0, 0, 0);"
        "    height: 20px;"
        "    subcontrol-position: top;"
        "    subcontrol-origin: margin;"
        "}");

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(maxWidth);
    scrollArea->setMaximumWidth(maxWidth);

    horizontalLayout->addWidget(scrollArea);
    return {layout, scrollArea};
  }
  contentWidget->setMinimumWidth(maxWidth);
  contentWidget->setMaximumWidth(maxWidth);
  horizontalLayout->addWidget(contentWidget);

  return {layout, contentWidget};
}

}    // namespace joda::ui::qt
