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
#include "backend/helper/database/plugins/filter.hpp"
#include "ui/gui/helper/table_widget.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogResultsTemplateGenerator::DialogResultsTemplateGenerator(WindowMain *mainWindow, db::QueryFilter *filter) :
    QDialog(mainWindow), mMainWindow(mainWindow), mFilter(filter), mLayout(this, true)
{
  //
  // Action button
  //
  auto *mSaveAsTemplate = new QAction(generateIcon("add-to-favorites"), "Save as template");
  mLayout.addItemToTopToolbar(mSaveAsTemplate);
  // connect(mSaveAsTemplate, &QAction::triggered, this, &PanelEdit::onSaveAsTemplate);

  auto *copyChannel = new QAction(generateIcon("copy"), "Copy channel");
  mLayout.addItemToTopToolbar(copyChannel);
  // connect(copyChannel, &QAction::triggered, this, &PanelEdit::onCopyChannel);

  //
  // Table
  //
  mCommands = new PlaceholderTableWidget();
  mCommands->setColumnCount(3);
  mCommands->setColumnHidden(0, true);
  mCommands->setHorizontalHeaderLabels({"", "", "Command"});
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(false);
  mCommands->horizontalHeader()->setVisible(false);
  mCommands->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mCommands->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mCommands->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
  mCommands->setColumnWidth(1, 16);
  mCommands->setShowGrid(false);
  mCommands->setStyleSheet("QTableView::item { border-top: 0px solid black; border-bottom: 1px solid gray; }");
  mCommands->verticalHeader()->setMinimumSectionSize(36);
  mCommands->verticalHeader()->setDefaultSectionSize(36);
  mCommands->installEventFilter(this);

  // Middle layout
  auto *tab = mLayout.addTab(
      "", [] {}, false);
  auto *col = tab->addVerticalPanel();
  col->setContentsMargins(0, 0, 0, 0);
  col->setSpacing(0);
  col->addWidget(mCommands);
}

}    // namespace joda::ui::gui
