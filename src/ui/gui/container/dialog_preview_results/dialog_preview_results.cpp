///
/// \file      dialog_pipeline_settings.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_preview_results.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qformlayout.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/container/setting/setting_base.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include "table_model_preview_result.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogPreviewResults::DialogPreviewResults(const joda::settings::Classification &classes, const joda::ctrl::Preview::PreviewResults *results,
                                           WindowMain *windowMain) :
    QDialog(windowMain)
{
  setWindowTitle("Preview results");
  setMinimumSize(100, 400);
  auto *layout = new QVBoxLayout();

  mResultsTable = new QTableView(this);
  mResultsTable->setFrameStyle(QFrame::NoFrame);
  mResultsTable->setShowGrid(false);
  mResultsTable->setItemDelegate(new HtmlDelegate(mResultsTable));
  mResultsTable->verticalHeader()->setVisible(false);
  mResultsTable->horizontalHeader()->setVisible(false);
  mResultsTable->setAlternatingRowColors(true);
  mResultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mResultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mTableModel = new TableModelPreviewResult(classes, mResultsTable);
  mTableModel->setData(results);
  mResultsTable->setModel(mTableModel);

  layout->addWidget(mResultsTable);
  setLayout(layout);
}

void DialogPreviewResults::refresh()
{
  if(mTableModel != nullptr) {
    mTableModel->refresh();
  }
}

}    // namespace joda::ui::gui
