///
/// \file      dialog_results_template_generator.cpp
/// \author    Joachim Danmayr
/// \date      2025-03-04
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "panel_results_template_generator.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include "backend/enums/enums_classes.hpp"
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
PanelResultsTemplateGenerator::PanelResultsTemplateGenerator(WindowMain *mainWindow, joda::settings::AnalyzeSettings *analyzeSettings) :
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
  auto *addColumn = new QPushButton(generateIcon("add-column"), "", this);
  addColumn->setToolTip("Add column");
  connect(addColumn, &QPushButton::pressed, [this]() {
    mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
    mColumnEditDialog->exec(mColumns->rowCount());
    refreshView();
    mMainWindow->checkForSettingsChanged();
  });
  toolBar->addWidget(addColumn);

  auto *editColumn = new QPushButton(generateIcon("edit-column"), "", this);
  editColumn->setToolTip("Edit column");
  toolBar->addWidget(editColumn);
  connect(editColumn, &QPushButton::pressed, [this]() {
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
      mColumnEditDialog->exec(mSelectedTableRow);
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  auto *deleteColumn = new QPushButton(generateIcon("delete-column"), "", this);
  deleteColumn->setToolTip("Delete column");
  toolBar->addWidget(deleteColumn);
  connect(deleteColumn, &QPushButton::pressed, [this]() {
    if(mSelectedTableRow >= 0) {
      mAnalyzeSettings->resultsSettings.eraseColumn({.tabIdx = 0, .colIdx = mSelectedTableRow});
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  //
  // Table
  //
  mColumns = new PlaceholderTableWidget();
  mColumns->setColumnCount(1);
  mColumns->setPlaceholderText("Add a column");
  mColumns->setHorizontalHeaderLabels({"Results column template"});
  mColumns->setAlternatingRowColors(true);
  mColumns->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mColumns->verticalHeader()->setVisible(true);
  mColumns->horizontalHeader()->setVisible(true);
  mColumns->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mColumns->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mColumns->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  connect(mColumns, &PlaceholderTableWidget::currentCellChanged,
          [this](int currentRow, int currentColumn, int previousRow, int previousColumn) { mSelectedTableRow = currentRow; });

  connect(mColumns, &PlaceholderTableWidget::cellDoubleClicked, [this](int row, int column) {
    mSelectedTableRow = row;
    if(mSelectedTableRow >= 0) {
      mColumnEditDialog->updateClassesAndClasses(*mAnalyzeSettings);
      mColumnEditDialog->exec(mSelectedTableRow);
      refreshView();
      mMainWindow->checkForSettingsChanged();
    }
  });

  mainLayout->addWidget(mColumns);
  mainLayout->addLayout(toolBar);

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
// int32_t PanelResultsTemplateGenerator::exec()
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
void PanelResultsTemplateGenerator::refreshView()
{
  auto getNameForClass = [this](const enums::ClassId &classId) -> std::string {
    for(const auto &classs : mAnalyzeSettings->projectSettings.classification.classes) {
      if(classs.classId == classId) {
        return classs.name;
      }
    }
    return "";
  };

  auto &columns = mAnalyzeSettings->resultsSettings.mutableColumns();
  mColumns->setRowCount(columns.size());
  std::set<int32_t> toRemove;
  for(auto &[index, key] : columns) {
    auto *item          = mColumns->item(index.colIdx, 0);
    std::string nameTmp = getNameForClass(key.classId);
    if(nameTmp.empty()) {
      toRemove.emplace(index.colIdx);
    }
    key.names.className        = nameTmp;
    key.names.intersectingName = getNameForClass(key.intersectingChannel);

    if(item == nullptr) {
      item = new QTableWidgetItem();
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      item->setText(key.createHeader().data());
      mColumns->setItem(index.colIdx, 0, item);
    } else {
      item->setText(key.createHeader().data());
    }
  }

  // Remove not existing classes
  if(!toRemove.empty()) {
    for(const auto idx : toRemove) {
      mAnalyzeSettings->resultsSettings.eraseColumn({.tabIdx = 0, .colIdx = idx});
    }
    refreshView();
    mMainWindow->checkForSettingsChanged();
  }
}

}    // namespace joda::ui::gui
