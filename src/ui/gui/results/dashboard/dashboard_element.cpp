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
#include <qnamespace.h>
#include <qwidget.h>
#include "ui/gui/helper/html_delegate.hpp"
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
  mTable->setItemDelegate(new HtmlDelegate(mTable));

  // connect(mTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
  //         [this](int logicalIndex) { openNextLevel({mActListData.data(logicalIndex, 0)}); });
  // connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) { openNextLevel({mActListData.data(row, 0)}); });
  connect(mTable, &QTableWidget::cellClicked, this, &DashboardElement::onCellClicked);
  connect(mTable, &QTableWidget::currentCellChanged, this, &DashboardElement::onTableCurrentCellChanged);

  layout->addWidget(mTable);
  setWidget(centralWidget);
}

void DashboardElement::setData(const QString &description, const std::vector<const table::TableColumn *> &cols,
                               const table::TableColumn *intersectingColl)
{
  setWindowTitle(description);
  int32_t colCount = cols.size();
  if(intersectingColl != nullptr) {
    colCount++;
  }
  mTable->setColumnCount(colCount);
  mTable->setRowCount(0);

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
    widget->setStatusTip(data);
    return widget;
  };

  //
  // We can assume that the data are ordered by image_id, parent_object_id, objects_id, t_stack.
  // We want to align first by parent_object_id and afterwards by object_id.
  // Imagine that the parent_object_id is not unique!
  //
  struct RowInfo
  {
    int32_t startingRow = 0;
    QColor bgColor;
  };
  std::map<uint64_t, RowInfo> startOfNewParent;    // Key is the parent_id and value the row where this parent started

  // Header
  {
    int colTbl        = intersectingColl == nullptr ? 0 : 1;    // We start with 1 because at 0 we pout the intersecting objects
    int32_t alternate = 0;
    QColor bgColor    = mTable->palette().color(QPalette::Base);
    for(const auto &colData : cols) {
      char txt           = colTbl + 'A';
      auto colCount      = QString(std::string(1, txt).data());
      QString headerText = colData->colSettings.createHeader().data();
      mTable->setHorizontalHeaderItem(colTbl, createTableWidget(headerText));
      int row = 0;
      for(const auto &[_, rowData] : colData->rows) {
        if(mTable->rowCount() < (row + 1)) {
          mTable->setRowCount(row + 1);
        }
        auto key = rowData.getParentId();
        if(key == 0) {
          key = rowData.getObjectId();
        }
        if(!startOfNewParent.contains(key)) {
          if(alternate % 2 != 0) {
            bgColor = mTable->palette().color(QPalette::AlternateBase);
          } else {
            bgColor = mTable->palette().color(QPalette::Base);
          }
          alternate++;
          startOfNewParent[key] = {row, bgColor};
        } else {
          bgColor = startOfNewParent.at(key).bgColor;
        }

        // Cleanup possible old data
        if(intersectingColl != nullptr && mTable->item(row, 0) != nullptr) {
          mTable->item(row, 0)->setText("");
          mTable->item(row, 0)->setBackground(QBrush(QColor(Qt::white)));
        }

        mTable->setVerticalHeaderItem(row, createTableWidget(std::to_string(row).data()));
        QTableWidgetItem *item = mTable->item(row, colTbl);
        if(item == nullptr) {
          item = createTableWidget(" ");
          mTable->setItem(row, colTbl, item);
        }

        if(item != nullptr) {
          if(rowData.isNAN()) {
            item->setText("-");
            item->setBackground(QBrush(QColor(bgColor)));
          } else {
            item->setText(QString::number((double) rowData.getVal()) + "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " +
                          QString::number(rowData.getObjectId()) + " ⬆ " + QString::number(rowData.getParentId()) + "</i><span>");
            item->setBackground(QBrush(QColor(bgColor)));
          }
          QFont font = item->font();
          font.setStrikeOut(!rowData.isValid());
          item->setFont(font);
        }
        row++;
      }
      colTbl++;
    }
  }
  //
  // Now put intersecting class the resulting table looks like that
  //
  //  Intersect   |  Data
  // -------------|------------
  // obj=1        |par=1
  //              |par=1
  //              |par=1
  // obj=2        |par=2
  //              |par=2
  //
  //
  if(nullptr != intersectingColl) {
    QString headerText = intersectingColl->colSettings.createHeader().data();
    mTable->setHorizontalHeaderItem(0, createTableWidget(headerText));
    for(const auto &[_, rowData] : intersectingColl->rows) {
      if(!startOfNewParent.contains(rowData.getObjectId())) {
        continue;
      }

      auto [row, bgColor] = startOfNewParent.at(rowData.getObjectId());
      mTable->setVerticalHeaderItem(row, createTableWidget(std::to_string(row).data()));
      QTableWidgetItem *item = mTable->item(row, 0);
      if(item == nullptr) {
        item = createTableWidget(" ");
        mTable->setItem(row, 0, item);
      }
      if(item != nullptr) {
        if(rowData.isNAN()) {
          item->setText("-");
          item->setBackground(QBrush(QColor(bgColor)));

        } else {
          item->setText(QString::number((double) rowData.getVal()) + "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " +
                        QString::number(rowData.getObjectId()) + " ⬆ " + QString::number(rowData.getParentId()) + "</i><span>");
        }
        QFont font = item->font();
        font.setStrikeOut(!rowData.isValid());
        item->setFont(font);
        item->setBackground(QBrush(QColor(bgColor)));
      }
    }
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
