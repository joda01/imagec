///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_results_info.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <string>
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/processor/initializer/pipeline_initializer.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "panel_results_template_generator.hpp"

namespace joda::ui::gui {

PanelResultsInfo::PanelResultsInfo(WindowMain *windowMain) : mWindowMain(windowMain)
{
  auto *layout = new QVBoxLayout();

  auto addSeparator = [&layout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };

  // layout->setContentsMargins(0, 0, 0, 0);
  {
      // auto *mSearchField = new QLineEdit();
      // mSearchField->setPlaceholderText("Search ...");
      // layout->addWidget(mSearchField);
      // connect(mSearchField, &QLineEdit::editingFinished, this, &PanelResultsInfo::filterResults);
  }

  {
    mLastLoadedResults = new PlaceholderTableWidget(0, 2);
    mLastLoadedResults->setPlaceholderText("Select a working directory");
    mLastLoadedResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mLastLoadedResults->verticalHeader()->setVisible(false);
    mLastLoadedResults->setHorizontalHeaderLabels({"Path", "Results"});
    mLastLoadedResults->setAlternatingRowColors(true);
    mLastLoadedResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    mLastLoadedResults->setColumnHidden(0, true);
    mLastLoadedResults->setMaximumHeight(150);

    connect(mLastLoadedResults, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
      // Open results
      mWindowMain->openResultsSettings(mLastLoadedResults->item(row, 0)->text());
    });

    layout->addWidget(mLastLoadedResults);
  }
  addSeparator();
  {
    mResultsTemplate = new PanelResultsTemplateGenerator(mWindowMain, &mWindowMain->mutableSettings());
    layout->addWidget(mResultsTemplate);
  }

  setLayout(layout);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::filterResults()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::fromSettings(const joda::settings::AnalyzeSettings &settings)
{
  mWindowMain->mutableSettings().resultsSettings = settings.resultsSettings;
  mResultsTemplate->refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::refreshTableView()
{
  mResultsTemplate->refreshView();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::clearHistory()
{
  mLastLoadedResults->setRowCount(0);
  mAddedPaths.clear();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelResultsInfo::addResultsFileToHistory(const std::filesystem::path &dbFile, const std::string &jobName,
                                               const std::chrono::system_clock::time_point &time)
{
  if(mAddedPaths.contains(dbFile.string())) {
    return;
  }
  mAddedPaths.emplace(dbFile.string());
  mLastLoadedResults->insertRow(0);
  auto *index = new QTableWidgetItem(dbFile.string().data());
  index->setFlags(index->flags() & ~Qt::ItemIsEditable);
  mLastLoadedResults->setItem(0, 0, index);

  auto *item = new QTableWidgetItem(QString((jobName + " (" + joda::helper::timepointToIsoString(time) + ")").data()));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  mLastLoadedResults->setItem(0, 1, item);
}

}    // namespace joda::ui::gui
