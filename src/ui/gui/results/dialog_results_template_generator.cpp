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
#include <qboxlayout.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
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
    QWidget(mainWindow), mMainWindow(mainWindow), mAnalyzeSettings(analyzeSettings)
{
  auto *mainLayout = new QVBoxLayout();
  mainLayout->setContentsMargins(0, 0, 0, 0);
  setContentsMargins(0, 0, 0, 0);
  this->setLayout(mainLayout);
  //
  // Action button
  //
  auto *toolBar = new QHBoxLayout();
  toolBar->setContentsMargins(0, 0, 0, 0);
  auto *addColumn = new QPushButton(generateIcon("add-column"), "");
  addColumn->setToolTip("Add column");
  connect(addColumn, &QPushButton::pressed, [this]() {
    mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
    mColumnEditDialog->exec(mCommands->rowCount());
    refreshView();
  });
  toolBar->addWidget(addColumn);

  auto *editColumn = new QPushButton(generateIcon("edit-column"), "");
  editColumn->setToolTip("Edit column");
  toolBar->addWidget(editColumn);
  connect(editColumn, &QPushButton::pressed, [this]() {
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
      mColumnEditDialog->exec(mSelectedTableRow);
      refreshView();
    }
  });

  auto *deleteColumn = new QPushButton(generateIcon("delete-column"), "");
  deleteColumn->setToolTip("Delete column");
  toolBar->addWidget(deleteColumn);
  connect(deleteColumn, &QPushButton::pressed, [this]() {
    if(mSelectedTableRow >= 0) {
      mAnalyzeSettings->resultsSettings.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableRow});
      refreshView();
    }
  });
  mainLayout->addLayout(toolBar);

  //
  // Table
  //
  mCommands = new PlaceholderTableWidget();
  mCommands->setColumnCount(1);
  mCommands->setPlaceholderText("Add a column");
  mCommands->setHorizontalHeaderLabels({"Column"});
  mCommands->setAlternatingRowColors(true);
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(true);
  mCommands->horizontalHeader()->setVisible(true);
  mCommands->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mCommands->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mCommands->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  connect(mCommands, &PlaceholderTableWidget::currentCellChanged,
          [this](int currentRow, int currentColumn, int previousRow, int previousColumn) { mSelectedTableRow = currentRow; });
  mainLayout->addWidget(mCommands);

  //
  // Add command dialog
  //
  mColumnEditDialog = new DialogColumnSettings(&mAnalyzeSettings->resultsSettings, this);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
// int32_t DialogResultsTemplateGenerator::exec()
//{
//   refreshView();
//   mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
//   return QDialog::exec();
// }

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogResultsTemplateGenerator::refreshView()
{
  const auto &columns = mAnalyzeSettings->resultsSettings.getColumns();
  mCommands->setRowCount(columns.size());
  for(const auto &[index, key] : columns) {
    auto *item = mCommands->item(index.colIdx, 0);
    if(item == nullptr) {
      item = new QTableWidgetItem();
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      item->setText(key.createHeader().data());
      mCommands->setItem(index.colIdx, 0, item);
    } else {
      item->setText(key.createHeader().data());
    }
  }
}

}    // namespace joda::ui::gui
