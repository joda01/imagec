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
#include <qnamespace.h>
#include <qwidget.h>
#include "ui/gui/helper/word_wrap_header.hpp"

namespace joda::ui::gui {

DashboardElement::DashboardElement(QWidget *widget) : QMdiSubWindow(widget)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowFullscreenButtonHint | Qt::WindowMinimizeButtonHint |
                 Qt::WindowMaximizeButtonHint);    // No close button

  auto *centralWidget = new QWidget(this);
  auto *layout        = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  mTable = new PlaceholderTableWidget(centralWidget);
  mTable->setPlaceholderText("Click >Add column< to start.");
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
  mTable->verticalHeader()->setDefaultSectionSize(8);    // Set each row to 50 pixels height
  mTable->setHorizontalHeader(new WordWrapHeader(Qt::Horizontal));

  // connect(mTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
  //         [this](int logicalIndex) { openNextLevel({mActListData.data(logicalIndex, 0)}); });
  // connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) { openNextLevel({mActListData.data(row, 0)}); });
  connect(mTable, &QTableWidget::cellClicked, this, &DashboardElement::onCellClicked);
  connect(mTable, &QTableWidget::currentCellChanged, this, &DashboardElement::onTableCurrentCellChanged);

  layout->addWidget(mTable);
  setWidget(centralWidget);
}

void DashboardElement::setData(const QString &description, const std::map<uint32_t, const table::TableColumn *> &cols)
{
  setWindowTitle(description);
  mTable->setColumnCount(cols.size());
  mTable->setRowCount(0);

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
    widget->setStatusTip(data);
    return widget;
  };

  // Header
  int col = 0;
  for(const auto &colData : cols) {
    char txt           = col + 'A';
    auto colCount      = QString(std::string(1, txt).data());
    QString headerText = colData.second->colSettings.createHeader().data();
    mTable->setHorizontalHeaderItem(col, createTableWidget(headerText));

    int row = 0;
    for(const auto &[_, rowData] : colData.second->rows) {
      if(mTable->rowCount() < (row + 1)) {
        mTable->setRowCount(row + 1);
      }

      mTable->setVerticalHeaderItem(row, createTableWidget(std::to_string(row).data()));

      QTableWidgetItem *item = mTable->item(row, col);
      if(item == nullptr) {
        item = createTableWidget(" ");
        mTable->setItem(row, col, item);
      }
      if(item != nullptr) {
        if(rowData.isNAN()) {
          item->setText("-");
        } else {
          item->setText(QString::number((double) rowData.getVal()));
        }
        QFont font = item->font();
        font.setStrikeOut(!rowData.isValid());
        item->setFont(font);
      }
      row++;
    }
    col++;
  }

  adjustSize();
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
  for(int row = 0; row < mTable->rowCount(); ++row) {
    QStringList rowData;
    for(int col = 0; col < mTable->columnCount(); ++col) {
      if(row == 0) {
        header << mTable->horizontalHeaderItem(col)->text();
      }
      if(col == 0) {
        rowData << mTable->verticalHeaderItem(row)->text();
      }
      rowData << mTable->item(row, col)->text();
    }
    data << rowData.join("\t");    // Join row data with tabs for better readability
  }

  QString text = "\t" + header.join("\t") + "\n" + data.join("\n");    // Join rows with newlines

  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::onCellClicked(int rowSelected, int columnSelcted)
{
  /*
  table::TableCell selectedData;
  if(mActListData.empty()) {
    mSelectedTableColumnIdx = -1;
    mSelectedTableRow       = -1;
  } else {
    mSelectedTableColumnIdx = columnSelcted;
    mSelectedTableRow       = rowSelected;
    mSelection[mNavigation] = {rowSelected, columnSelcted};
    selectedData            = mActListData.data(rowSelected, columnSelcted);
  }*/
  // onElementSelected(mSelectedTableColumnIdx, mSelectedTableRow, selectedData);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::onTableCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  onCellClicked(currentRow, currentColumn);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DashboardElement::reset()
{
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
}

}    // namespace joda::ui::gui
