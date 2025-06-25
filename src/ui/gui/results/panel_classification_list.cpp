///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_classification_list.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qwidget.h>
#include <exception>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/analze_settings.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/colord_square_delegate.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

PanelClassificationList::PanelClassificationList(WindowMain *windowMain) : mWindowMain(windowMain)
{
  setWindowTitle("Column settings");
  mClassSettingsDialog = new DialogClassSettings(windowMain);
  auto *centralWidget  = new QWidget();
  auto *layout         = new QVBoxLayout();
  centralWidget->setLayout(layout);

  {
    auto *toolbar = new QToolBar();

    layout->addWidget(toolbar);
  }

  {
    mClasses = new PlaceholderTableWidget(0, 5);
    mClasses->setPlaceholderText("No classes found!");
    mClasses->verticalHeader()->setVisible(false);
    mClasses->horizontalHeader()->setVisible(false);
    mClasses->setHorizontalHeaderLabels({"IdNr", "Id", "Class", "Color", "Notes"});
    mClasses->setAlternatingRowColors(true);
    mClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClasses->setColumnHidden(COL_ID, true);
    mClasses->setColumnHidden(COL_ID_ENUM, true);
    mClasses->setColumnHidden(COL_COLOR, true);
    mClasses->setColumnHidden(COL_NOTES, true);
    mClasses->setColumnWidth(COL_ID_ENUM, 10);
    mClasses->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    auto *delegate = new ColoredSquareDelegate(mClasses);
    mClasses->setItemDelegateForColumn(COL_NAME, delegate);    // Set the delegate for the desired column

    layout->addWidget(mClasses);
  }

  setWidget(centralWidget);
  // connect(mClasses, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
  connect(mClasses, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    if(column == COL_NAME) {
      openEditDialog(row, column);
    }
  });
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::openEditDialog(int row, int column)
{
  auto it = mClassesList.begin();
  std::advance(it, row);
  if(mClassSettingsDialog->exec(*it) == 0) {
    onSettingChanged();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::createTableItem(int32_t rowIdx, enums::ClassId classId, const std::string &name, const std::string &color,
                                              const std::string &notes)
{
  auto *index = new QTableWidgetItem(QString::number(static_cast<uint16_t>(classId)));
  index->setFlags(index->flags() & ~Qt::ItemIsEditable);
  mClasses->setItem(rowIdx, COL_ID, index);

  nlohmann::json classIdStr = classId;
  auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
  itemEnum->setFlags(itemEnum->flags() & ~Qt::ItemIsEditable);
  mClasses->setItem(rowIdx, COL_ID_ENUM, itemEnum);

  auto *item = new QTableWidgetItem(QString(name.data()));
  item->setFlags(itemEnum->flags() & ~Qt::ItemIsEditable);
  mClasses->setItem(rowIdx, COL_NAME, item);

  auto calculatedColor = QString(color.data());
  if(calculatedColor.isEmpty()) {
    calculatedColor = QString(joda::settings::COLORS.at(rowIdx % joda::settings::COLORS.size()).data());
  }
  auto *itemColor = new QTableWidgetItem(calculatedColor);
  mClasses->setItem(rowIdx, COL_COLOR, itemColor);

  auto *itemNotes = new QTableWidgetItem(QString(notes.data()));
  mClasses->setItem(rowIdx, COL_NOTES, itemNotes);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::setDatabase(joda::db::Database *database)
{
  mDatabase = database;
  loadClasses();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::loadClasses()
{
  if(mDatabase == nullptr) {
    mClasses->setRowCount(0);
    return;
  }
  auto classes = mDatabase->selectClasses();
  mClasses->setRowCount(classes.size());
  int32_t rowIdx = 0;
  for(const auto &[id, classs] : classes) {
    createTableItem(rowIdx, id, classs.name, classs.color, classs.notes);
    mClassesList.emplace_back(classs);
    rowIdx++;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassificationList::onSettingChanged()
{
}

}    // namespace joda::ui::gui
