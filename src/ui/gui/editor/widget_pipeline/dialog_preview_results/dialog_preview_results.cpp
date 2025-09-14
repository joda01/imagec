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
#include <qabstractitemmodel.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qwidget.h>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/editor/widget_pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "tabel_item_delegate_preview_Result.hpp"
#include "table_model_preview_result.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogPreviewResults::DialogPreviewResults(PanelImageView *panelImageView, const joda::settings::Classification &classes, WindowMain *windowMain) :
    QDialog(windowMain), mWindowMain(windowMain), mPanelImageView(panelImageView)
{
  setWindowTitle("Preview results");
  setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
  setWindowOpacity(0.5);    // 0.0 = fully transparent, 1.0 = fully opaque

  setAttribute(Qt::WA_TranslucentBackground);
  setStyleSheet("background-color: rgba(0, 0, 0, 255); border-radius: 8px;");
  setMaximumWidth(250);

  auto *container = new QWidget(this);
  auto *layoutMy  = new QVBoxLayout(container);
  layoutMy->setContentsMargins(8, 8, 8, 8);
  mResultsTable = new QTableView(this);
  mResultsTable->setFrameStyle(QFrame::NoFrame);
  mResultsTable->setShowGrid(false);
  mResultsTable->setItemDelegate(new ColorSquareDelegate(mResultsTable));
  mResultsTable->verticalHeader()->setVisible(false);
  mResultsTable->horizontalHeader()->setVisible(false);
  mResultsTable->setAlternatingRowColors(false);
  mResultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mResultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mTableModel = new TableModelPreviewResult(classes, mResultsTable);
  mResultsTable->setModel(mTableModel);
  layoutMy->addWidget(mResultsTable);

  setLayout(new QVBoxLayout);
  layout()->addWidget(container);

  connect(mResultsTable, &QTableView::doubleClicked, [this](const QModelIndex &index) {
    bool isHidden = mTableModel->data(index, Qt::CheckStateRole).toBool();
    mTableModel->setHiddenFlag(static_cast<enums::ClassId>(mTableModel->data(index, TableModelPreviewResult::CLASS_ROLE).toInt()), !isHidden);
    mPanelImageView->setRoisToHide(getClassesToHide());
  });
}

void DialogPreviewResults::enterEvent(QEnterEvent *event)
{
  setWindowOpacity(0.9);    // Fully opaque on hover
  QDialog::enterEvent(event);
}

void DialogPreviewResults::leaveEvent(QEvent *event)
{
  setWindowOpacity(0.5);    // Transparent again when mouse leaves
  mResultsTable->clearSelection();
  QDialog::leaveEvent(event);
}

void DialogPreviewResults::setResults(PanelPipelineSettings *pipelineSettings, joda::ctrl::Preview::PreviewResults *results)
{
  if(mTableModel != nullptr) {
    mTableModel->setData(results);
  }
  mPipelineSettings = pipelineSettings;
}

void DialogPreviewResults::refresh()
{
  if(mTableModel != nullptr) {
    mTableModel->refresh();
  }
  update();
}

auto DialogPreviewResults::getClassesToHide() const -> settings::ObjectInputClassesExp
{
  settings::ObjectInputClassesExp ret;
  for(int n = 0; n < mTableModel->rowCount(); n++) {
    auto idx = mTableModel->index(n, 0);

    if(mTableModel->data(idx, Qt::CheckStateRole).toBool()) {
      ret.emplace(static_cast<enums::ClassId>(mTableModel->data(idx, TableModelPreviewResult::CLASS_ROLE).toInt()));
    }
  }
  return ret;
}

void DialogPreviewResults::paintEvent(QPaintEvent *event)
{
  QDialog::paintEvent(event);
}

}    // namespace joda::ui::gui
