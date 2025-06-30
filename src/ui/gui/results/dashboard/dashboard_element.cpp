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
#include "backend/helper/base32.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/html_header.hpp"

namespace joda::ui::gui {

DashboardElement::DashboardElement(QWidget *widget) : QMdiSubWindow(widget)
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowFullscreenButtonHint | Qt::WindowMinimizeButtonHint |
                 Qt::WindowMaximizeButtonHint);    // No close button

  auto *centralWidget = new QWidget(this);
  auto *layout        = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 4, 0, 0);

  // Header
  {
    mHeaderLabel = new QLabel();
    layout->addWidget(mHeaderLabel);
  }

  // Table
  {
    mTable = new PlaceholderTableWidget(centralWidget);
    mTable->setPlaceholderText("Click >Add column< to start.");
    mTable->setRowCount(0);
    mTable->setColumnCount(0);
    mTable->verticalHeader()->setDefaultSectionSize(8);    // Set each row to 50 pixels height
    mTable->setHorizontalHeader(new HtmlHeaderView(Qt::Horizontal));
    mTable->horizontalHeader()->setMinimumSectionSize(120);
    mTable->horizontalHeader()->setDefaultSectionSize(120);
    mTable->setItemDelegate(new HtmlDelegate(mTable));
    connect(mTable, &QTableWidget::cellDoubleClicked, [this](int row, int column) {
      if(mTableCells.contains(column) && mTableCells.at(column).contains(row)) {
        emit cellDoubleClicked(*mTableCells[column][row]);
      }
    });
    connect(mTable, &QTableWidget::cellClicked, [this](int row, int column) {
      if(mTableCells.contains(column) && mTableCells.at(column).contains(row)) {
        emit cellSelected(*mTableCells[column][row]);
      }
    });
    connect(mTable, &QTableWidget::currentCellChanged, [this](int row, int column) {
      if(mTableCells.contains(column) && mTableCells.at(column).contains(row)) {
        emit cellSelected(*mTableCells[column][row]);
      }
    });
    layout->addWidget(mTable);
  }
  setWidget(centralWidget);
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
  mHeaderLabel->setText("<b>" + text + "</b>");
}

void DashboardElement::setData(const QString &description, const std::vector<const table::TableColumn *> &cols, bool isImageView,
                               const table::TableColumn *intersectingColl)
{
  setWindowTitle(description);
  setHeader(description);

  int32_t colCount = cols.size();
  if(intersectingColl != nullptr && isImageView) {
    colCount++;
  }
  if(isImageView) {
    colCount++;    // +1 because we add the object ID at the first column
  }
  mTable->setColumnCount(colCount);
  mTable->setRowCount(0);

  auto createTableWidget = [](const QString &data) {
    auto *widget = new QTableWidgetItem(data);
    widget->setFlags(widget->flags() & ~Qt::ItemIsEditable);
    widget->setStatusTip(data);
    return widget;
  };

  mTableCells.clear();

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

  // We add a column with the object ID as first column
  const int32_t COL_IDX_OBJECT_ID    = 0;
  const int32_t COL_IDX_INTERSECTING = 1;

  // Data
  {
    int colTbl = intersectingColl == nullptr ? 1 : 2;    // We start with 2 because at 1 we put the intersecting objects
    if(!isImageView) {
      colTbl = 0;    // In plate and well view we do not show IDs because it does not make sense.
    }
    int32_t alternate = 0;
    QColor bgColor    = mTable->palette().color(QPalette::Base);
    for(const auto &colData : cols) {
      QString headerText =
          colData->colSettings.createHtmlHeader(settings::ResultsSettings::ColumnKey::HeaderStyle::ONLY_STATS_CONTAINS_INTERSECTING).data();
      mTable->setHorizontalHeaderItem(colTbl, createTableWidget(headerText));
      int row = 0;
      for(const auto &[_, rowData] : colData->rows) {
        if(rowData.getObjectId() == 0) {
          continue;
        }

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

        // =========================================
        // Parent object ID
        // =========================================
        if(isImageView) {
          if(intersectingColl != nullptr) {
            QTableWidgetItem *intersectingItem = mTable->item(row, COL_IDX_INTERSECTING);
            if(intersectingItem == nullptr) {
              intersectingItem = createTableWidget("");
              mTable->setItem(row, COL_IDX_INTERSECTING, intersectingItem);
            }
            if(intersectingItem != nullptr) {
              // QString::number((double) rowData.getVal())
              intersectingItem->setText(
                  QString(joda::helper::toBase32(rowData.getParentId()).data()) +
                  "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) + " ⬆ " +
                  QString(joda::helper::toBase32(rowData.getParentId()).data()) + "</i><span>");
              intersectingItem->setBackground(QBrush(QColor(bgColor)));
            }
          }
          // Table cell assignment below
        }
        // Vertical header
        if(!rowData.isNAN()) {
          mTable->setVerticalHeaderItem(row, createTableWidget(rowData.getRowName().data()));
        }

        // =========================================
        // Add object ID
        // =========================================
        if(isImageView) {
          mTable->setHorizontalHeaderItem(COL_IDX_OBJECT_ID, createTableWidget("Object ID 🗝"));
          QTableWidgetItem *itemObjId = mTable->item(row, COL_IDX_OBJECT_ID);
          if(itemObjId == nullptr) {
            itemObjId = createTableWidget("");
            mTable->setItem(row, COL_IDX_OBJECT_ID, itemObjId);
          }
          if(itemObjId != nullptr) {
            itemObjId->setText(
                QString(joda::helper::toBase32(rowData.getObjectId()).data()) +
                "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) + " ⬆ " +
                QString(joda::helper::toBase32(rowData.getParentId()).data()) + "</i><span>");
            itemObjId->setBackground(bgColor);
          }
          mTableCells[COL_IDX_OBJECT_ID][row] = &rowData;
        }
        // =========================================
        // Add data
        // =========================================
        QTableWidgetItem *item   = mTable->item(row, colTbl);
        mTableCells[colTbl][row] = &rowData;
        if(item == nullptr) {
          item = createTableWidget("");
          mTable->setItem(row, colTbl, item);
        }

        if(item != nullptr) {
          if(rowData.isNAN()) {
            item->setText("-<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) +
                          " ⬆ " + QString(joda::helper::toBase32(rowData.getParentId()).data()) + "</i><span>");
            item->setBackground(QBrush(QColor(bgColor)));
          } else {
            item->setText(QString::number((double) rowData.getVal()) +
                          "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) +
                          " ⬆ " + QString(joda::helper::toBase32(rowData.getParentId()).data()) + "</i><span>");
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

  if(nullptr != intersectingColl && isImageView) {
    QString headerText =
        intersectingColl->colSettings.createHtmlHeader(settings::ResultsSettings::ColumnKey::HeaderStyle::ONLY_STATS_IN_INTERSECTING).data();
    mTable->setHorizontalHeaderItem(COL_IDX_INTERSECTING, createTableWidget(headerText));

    for(const auto &[_, rowData] : intersectingColl->rows) {
      if(rowData.getObjectId() == 0 || !startOfNewParent.contains(rowData.getObjectId())) {
        continue;
      }

      auto [row, bgColor] = startOfNewParent.at(rowData.getObjectId());
      // We link to the parent. So if the users clicks on this cell, he gets the information about the parent object
      // rowData.getVal() contains the number of elements we have to fill
      for(int n = 0; n < rowData.getVal(); n++) {
        int32_t rowTemp                            = row + n;
        mTableCells[COL_IDX_INTERSECTING][rowTemp] = &rowData;
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
        header << mTable->horizontalHeaderItem(col)->text().replace("<br>", " ").replace("<b>", "");
      }
      if(col == 0) {
        rowData << mTable->verticalHeaderItem(row)->text().replace("<br>", " ").replace("<b>", "");
      }
      auto *tmp = mTable->item(row, col);
      if(tmp != nullptr) {
        auto txtTemp = tmp->text();
        auto index   = txtTemp.indexOf("<br>");
        if(index != -1) {
          txtTemp = txtTemp.left(index);
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
