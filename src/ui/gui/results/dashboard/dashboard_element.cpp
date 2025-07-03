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
#include <string>
#include "backend/helper/base32.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/html_header.hpp"

namespace joda::ui::gui {

DashboardElement::DashboardElement(QWidget *widget) : QMdiSubWindow(widget)
{
  // setAttribute(Qt::WA_DeleteOnClose);
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
    // connect(mTable, &QTableWidget::cellClicked, [this](int row, int column) {
    //   if(mTableCells.contains(column) && mTableCells.at(column).contains(row)) {
    //     emit cellSelected(*mTableCells[column][row]);
    //   }
    // });
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

void DashboardElement::setData(const QString &description, const std::vector<const table::TableColumn *> &cols, bool isImageView, bool isColoc,
                               const table::TableColumn *intersectingColl)
{
  setWindowTitle(description);
  setHeader(description);

  mTable->setRowCount(0);
  mTableCells.clear();

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
  std::map<uint64_t, RowInfo> startOfNewParent;     // Key is the parent_id and value the row where this parent started
  std::map<uint64_t, RowInfo> trackingIdMapping;    // Key is the tracking_id and value the row where this tracking_id was first placed
  int32_t highestRow = 0;

  // We add a column with the object ID as first column
  int32_t COL_IDX_OBJECT_ID          = 0;
  const int32_t COL_IDX_INTERSECTING = 1;

  auto generateMetaFooter = [](const table::TableCell &rowData) {
    return "<br><span style=\"color:rgb(155, 153, 153);\"><i>🗝: " + QString(joda::helper::toBase32(rowData.getObjectId()).data()) + " ⬆ " +
           QString(joda::helper::toBase32(rowData.getParentId()).data()) + "<br> ↔ " +
           QString(joda::helper::toBase32(rowData.getDistanceToObjectId()).data()) + " ⚯ " +
           QString(joda::helper::toBase32(rowData.getTrackingId()).data()) + "</i><span>";
  };

  auto headerStyle = isColoc ? settings::ResultsSettings::ColumnKey::HeaderStyle::FULL
                             : settings::ResultsSettings::ColumnKey::HeaderStyle::ONLY_STATS_CONTAINS_INTERSECTING;

  const auto LIGHT_BLUE = QColor("#ADD8E6");
  const auto DARK_BLUE  = QColor("#87CEEB");

  // Data
  {
    int32_t alternate = 0;
    QColor bgColor    = mTable->palette().color(QPalette::Base);

    settings::ResultsSettings::ColumnKey actColumnKey;
    bool addObjectId = false;
    auto colTableTmp = 0;
    for(const auto &colData : cols) {
      if(actColumnKey.classId != colData->colSettings.classId && isImageView) {
        // Add the object ID again
        actColumnKey      = colData->colSettings;
        COL_IDX_OBJECT_ID = colTableTmp;
        addObjectId       = true;
        if(intersectingColl != nullptr && colTableTmp == 0) {
          colTableTmp += 2;    // We put the parent object id in the second column
        } else {
          colTableTmp++;
        }
      }
      mTable->setColumnCount(colTableTmp + 1);

      QString headerText = colData->colSettings.createHtmlHeader(headerStyle).data();
      mTable->setHorizontalHeaderItem(colTableTmp, createTableWidget(headerText));
      int row = 0;
      for(const auto &[_, rowData] : colData->rows) {
        if(rowData.getObjectId() == 0) {
          continue;
        }
        if(mTable->rowCount() < (row + 1)) {
          mTable->setRowCount(row + 1);
        }

        // =====================================
        // Row to place in case of coloc
        // =====================================
        int32_t rowToPlace = row;
        if(isColoc && isImageView) {
          if(rowData.getTrackingId() == 0) {
            // This should not happen, but if we continue
            continue;
          }

          if(trackingIdMapping.contains(rowData.getTrackingId())) {
            rowToPlace = trackingIdMapping.at(rowData.getTrackingId()).startingRow;
            bgColor    = trackingIdMapping.at(rowData.getTrackingId()).bgColor;
          } else {
            if(alternate % 2 != 0) {
              bgColor = mTable->palette().color(QPalette::AlternateBase);
            } else {
              bgColor = mTable->palette().color(QPalette::Base);
            }
            trackingIdMapping.emplace(rowData.getTrackingId(), RowInfo{highestRow, bgColor});
            highestRow++;
            alternate++;
          }
        }

        // =====================================
        // Alternating row color
        // =====================================
        if(!isColoc) {
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
            startOfNewParent[key] = {rowToPlace, bgColor};
          } else {
            bgColor = startOfNewParent.at(key).bgColor;
          }
        }

        // =========================================
        // Add object ID
        // =========================================
        if(isImageView) {
          QString headerNameObj = "<b>" + QString(actColumnKey.names.className.data()) + "</b><br>";
          if(headerStyle != settings::ResultsSettings::ColumnKey::HeaderStyle::FULL) {
            headerNameObj.clear();
          }
          mTable->setHorizontalHeaderItem(COL_IDX_OBJECT_ID, createTableWidget(headerNameObj + "Object ID 🗝"));
          QTableWidgetItem *itemObjId = mTable->item(rowToPlace, COL_IDX_OBJECT_ID);
          if(itemObjId == nullptr) {
            itemObjId = createTableWidget("");
            mTable->setItem(rowToPlace, COL_IDX_OBJECT_ID, itemObjId);
          }
          if(itemObjId != nullptr) {
            itemObjId->setText(QString(joda::helper::toBase32(rowData.getObjectId()).data()) + generateMetaFooter(rowData));
            if(bgColor == mTable->palette().color(QPalette::Base)) {
              itemObjId->setBackground(LIGHT_BLUE);    // We use a bluish value to better find the borders of a class

            } else {
              itemObjId->setBackground(DARK_BLUE);
            }
          }
          mTableCells[COL_IDX_OBJECT_ID][rowToPlace] = &rowData;
        }

        // =========================================
        // Parent object ID
        // =========================================
        if(isImageView) {
          if(intersectingColl != nullptr && !isColoc) {
            QTableWidgetItem *intersectingItem = mTable->item(rowToPlace, COL_IDX_INTERSECTING);
            if(intersectingItem == nullptr) {
              intersectingItem = createTableWidget("");
              mTable->setItem(row, COL_IDX_INTERSECTING, intersectingItem);
            }
            if(intersectingItem != nullptr) {
              // QString::number((double) rowData.getVal())
              intersectingItem->setText(QString(joda::helper::toBase32(rowData.getParentId()).data()) + generateMetaFooter(rowData));
              intersectingItem->setBackground(QBrush(QColor(bgColor)));
            }
          }
          // Table cell assignment below
        }
        // Vertical header
        if(!rowData.isNAN()) {
          mTable->setVerticalHeaderItem(rowToPlace, createTableWidget(rowData.getRowName().data()));
        }

        // =========================================
        // Add data
        // =========================================
        QTableWidgetItem *item               = mTable->item(rowToPlace, colTableTmp);
        mTableCells[colTableTmp][rowToPlace] = &rowData;
        if(item == nullptr) {
          item = createTableWidget("");
          mTable->setItem(rowToPlace, colTableTmp, item);
        }

        if(item != nullptr) {
          if(rowData.isNAN()) {
            item->setText("-" + generateMetaFooter(rowData));
            item->setBackground(QBrush(QColor(bgColor)));
          } else {
            item->setText(QString::number((double) rowData.getVal()) + generateMetaFooter(rowData));
            item->setBackground(QBrush(QColor(bgColor)));
          }
          QFont font = item->font();
          font.setStrikeOut(!rowData.isValid());
          item->setFont(font);
        }
        row++;
      }
      //  addObjectId = false;
      colTableTmp++;
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

  if(nullptr != intersectingColl && isImageView && !isColoc) {
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

  // adjustSize();
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
void DashboardElement::resetData()
{
  mTable->setRowCount(0);
  mTable->setColumnCount(0);
  mTable->clear();
}

}    // namespace joda::ui::gui
