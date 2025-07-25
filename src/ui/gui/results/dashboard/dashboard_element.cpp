///
/// \file      dashboard_element.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dashboard_element.hpp"
#include <qcolor.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <memory>
#include <string>
#include <thread>
#include "backend/helper/base32.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/html_header.hpp"
#include "ui/gui/helper/table_model.hpp"

namespace joda::ui::gui {

DashboardElement::DashboardElement() /*: QMdiSubWindow(widget)*/
{
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 4, 0, 0);
  // setMinimumWidth(500);
  //  Header
  {
    mHeaderLabel = new QLabel();
    layout->addWidget(mHeaderLabel);
  }

  // Table
  {
    mTableView = new QTableView();
    mTableView->setHorizontalHeader(new HtmlHeaderView(Qt::Horizontal));
    mTableView->setItemDelegate(new HtmlDelegate(mTableView));
    mTableView->horizontalHeader()->setMinimumSectionSize(120);
    mTableView->horizontalHeader()->setDefaultSectionSize(120);

    mTableModel = new TableModel(mTableView);
    mTableView->setModel(mTableModel);

    connect(mTableView, &QTableView::doubleClicked, [this](const QModelIndex &index) {
      int row = index.row();
      int col = index.column();
      emit cellDoubleClicked(*mTable->data(row, col));
    });

    connect(mTableView->selectionModel(), &QItemSelectionModel::currentChanged, [this](const QModelIndex &index, const QModelIndex & /*previous*/) {
      int row = index.row();
      int col = index.column();
      emit cellSelected(*mTable->data(row, col));
    });
    layout->addWidget(mTableView);
  }
  setLayout(layout);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
const std::string &DashboardElement::getTitle() const
{
  return mTitle;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::setHeader(const QString &text)
{
  setWindowTitle(text);
  mTitle = text.toStdString();
  mHeaderLabel->setText("<b>" + text + "</b>");
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::setData(const std::shared_ptr<joda::table::Table> table)
{
  using namespace std::chrono_literals;
  // Store actual selection
  saveSelection();
  mTable = table;
  setHeader(mTable->getTitle().data());
  mTableModel->setData(mTable);
  // Restore selected rows
  restoreSelection();
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::saveSelection()
{
  QModelIndex currentIndex = mTableView->currentIndex();
  if(currentIndex.isValid()) {
    savedRow    = currentIndex.row();
    savedColumn = currentIndex.column();
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::restoreSelection()
{
  if(savedRow >= 0 && savedColumn >= 0) {
    QModelIndex index = mTableModel->index(savedRow, savedColumn);
    if(index.isValid()) {
      mTableView->setCurrentIndex(index);
      mTableView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto DashboardElement::getTable() const -> const joda::table::Table &
{
  return *mTable;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto DashboardElement::getSelectedRows() const -> std::vector<joda::table::TableCell>
{
  std::vector<joda::table::TableCell> selectedCells;

  QItemSelectionModel *selectionModel = mTableView->selectionModel();
  if(selectionModel == nullptr) {
    return {};
  }
  QModelIndexList selectedIndex = selectionModel->selectedIndexes();    // one QModelIndex per selected row
  int columnCount               = mTableModel->columnCount();
  foreach(const QModelIndex &index, selectedIndex) {
    selectedCells.emplace_back(*mTable->data(index.row(), index.column()));
  }

  return selectedCells;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::copyTableToClipboard() const
{
  QStringList data;
  QStringList header;
  for(int row = 0; row < mTable->getNrOfRows(); ++row) {
    QStringList rowData;
    for(int col = 0; col < mTable->getNrOfCols(); ++col) {
      if(row == 0) {
        header << mTable->getColHeader(col).createHeader().data();
      }
      if(col == 0) {
        rowData << mTable->getRowHeader(row).data();
      }
      const auto tmp = mTable->data(row, col);
      if(tmp != nullptr) {
        auto val = tmp->getValAsVariant(mTable->getColHeader(col).measureChannel);
        QString txtTemp;
        if(std::holds_alternative<std::string>(val)) {
          txtTemp = std::get<std::string>(val).data();
        } else {
          txtTemp = QString::number(std::get<double>(val));
        }
        rowData << txtTemp;
      } else {
        rowData << "";
      }
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = "\t" + header.join("\t") + "\n" + data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}

}    // namespace joda::ui::gui
