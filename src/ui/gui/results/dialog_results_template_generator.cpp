///
/// \file      dialog_results_template_generator.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "dialog_results_template_generator.hpp"
#include <qdialog.h>
#include <qtablewidget.h>
#include "backend/helper/database/plugins/filter.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/results/dialog_column_settings.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogResultsTemplateGenerator::DialogResultsTemplateGenerator(WindowMain *mainWindow, joda::settings::AnalyzeSettings *analyzeSettings) :
    QDialog(mainWindow), mMainWindow(mainWindow), mAnalyzeSettings(analyzeSettings), mLayout(this, false, true, false)
{
  setWindowTitle("Results template");
  setMinimumWidth(350);
  setMinimumHeight(450);

  //
  // Table
  //
  mCommands = new PlaceholderTableWidget();
  mCommands->setColumnCount(1);
  mCommands->setHorizontalHeaderLabels({"Column"});
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(true);
  mCommands->horizontalHeader()->setVisible(true);
  mCommands->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mCommands->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mCommands->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  connect(mCommands, &PlaceholderTableWidget::currentCellChanged,
          [this](int currentRow, int currentColumn, int previousRow, int previousColumn) { mSelectedTableRow = currentRow; });

  // Middle layout
  auto *tab = mLayout.addTab(
      "", [] {}, false);
  auto *col = tab->addVerticalPanel();
  col->setContentsMargins(0, 0, 0, 0);
  col->setSpacing(0);
  col->addWidget(mCommands);

  //
  // Action button
  //
  auto *addColumn = new QAction(generateIcon("add-column"), "");
  addColumn->setToolTip("Add column");
  connect(addColumn, &QAction::triggered, [this]() {
    mColumnEditDialog->exec(mCommands->rowCount());
    refreshView();
  });
  mLayout.addItemToTopToolbar(addColumn);

  auto *editColumn = new QAction(generateIcon("edit-column"), "");
  editColumn->setToolTip("Edit column");
  mLayout.addItemToTopToolbar(editColumn);
  connect(editColumn, &QAction::triggered, [this]() {
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->exec(mSelectedTableRow);
      refreshView();
    }
  });

  auto *deleteColumn = new QAction(generateIcon("delete-column"), "");
  deleteColumn->setToolTip("Delete column");
  mLayout.addItemToTopToolbar(deleteColumn);
  connect(deleteColumn, &QAction::triggered, [this]() {
    if(mSelectedTableRow >= 0) {
      mAnalyzeSettings->resultsSettings.resultsTableTemplate.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableRow});
      refreshView();
    }
  });

  //
  // Add command dialog
  //
  mColumnEditDialog = new DialogColumnSettings(&mAnalyzeSettings->resultsSettings.resultsTableTemplate, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogResultsTemplateGenerator::exec()
{
  refreshView();
  mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
  return QDialog::exec();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogResultsTemplateGenerator::refreshView()
{
  const auto &columns = mAnalyzeSettings->resultsSettings.resultsTableTemplate.getColumns();
  mCommands->setRowCount(columns.size());
  for(const auto &[index, key] : columns) {
    auto *item = mCommands->item(index.colIdx, 0);
    if(item == nullptr) {
      item = new QTableWidgetItem();
      item->setText(key.createHeader().data());
      mCommands->setItem(index.colIdx, 0, item);
    } else {
      item->setText(key.createHeader().data());
    }
  }
}

}    // namespace joda::ui::gui
