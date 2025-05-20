///
/// \file      dialog_open_template.cpp
/// \author    Joachim Danmayr
/// \date      2025-05-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_open_template.hpp"
#include <qevent.h>
#include <qheaderview.h>
#include <qlabel.h>
#include <QKeyEvent>
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"

namespace joda::ui::gui {

DialogOpenTemplate::DialogOpenTemplate(const std::set<std::string> &directories, const std::string &endian, QWidget *parent) :
    mDirectories(directories), mEndian(endian), QDialog(parent)
{
  setWindowTitle("New project");
  mSearch = new QLineEdit();
  mSearch->setPlaceholderText("Search...");
  connect(mSearch, &QLineEdit::textChanged, [this]() { filterCommands({mSearch->text()}); });

  mTableTemplates = new QTableWidget();
  mTableTemplates->setColumnCount(3);
  mTableTemplates->setColumnHidden(0, true);
  mTableTemplates->setHorizontalHeaderLabels({"", "", "Template"});
  mTableTemplates->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mTableTemplates->verticalHeader()->setVisible(false);
  mTableTemplates->horizontalHeader()->setVisible(false);
  mTableTemplates->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mTableTemplates->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mTableTemplates->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
  mTableTemplates->setColumnWidth(1, 16);
  mTableTemplates->setShowGrid(false);
  mTableTemplates->setStyleSheet("QTableView::item { border-top: 0px solid black; border-bottom: 1px solid gray; }");
  mTableTemplates->verticalHeader()->setMinimumSectionSize(36);
  mTableTemplates->verticalHeader()->setDefaultSectionSize(36);
  mTableTemplates->installEventFilter(this);

  // mTableTemplates.setsh

  connect(mTableTemplates, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    auto idx = mTableTemplates->item(row, 0)->text().toInt();
    // addNewCommand(idx);
    // TODO open template
    close();
  });

  //
  auto *layout = new QVBoxLayout();
  layout->addWidget(mSearch);
  layout->addWidget(mTableTemplates);
  setLayout(layout);
  setMinimumHeight(600);
  setMinimumWidth(500);

  loadTemplates();
}

///
/// \brief      Start search when typing within the table
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool DialogOpenTemplate::eventFilter(QObject *obj, QEvent *event)
{
  if(obj == mTableTemplates && event->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(event);
    mSearch->setText(keyEvent->text());
    mSearch->setFocus();
  }
  // Standard event processing
  return QObject::eventFilter(obj, event);
}

void DialogOpenTemplate::show()
{
  filterCommands({mSearch->text()});
  exec();
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void DialogOpenTemplate::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(mDirectories, mEndian);

  mTableTemplates->setRowCount(0);

  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          addTitleToTable(category, category);
        }
      }

      addTemplateToTable(data, category);
    }
    addedPerCategory = dataInCategory.size();
  }
}

void DialogOpenTemplate::addTitleToTable(const std::string &title, const std::string &group)
{
  int newRow = mTableTemplates->rowCount();
  if(newRow < 0) {
    newRow = 0;
  }
  mTitleINdex.emplace(group, newRow);
  mTableTemplates->insertRow(newRow);
  auto *iconItem = new QTableWidgetItem();
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsSelectable);
  QFont font;
  font.setBold(true);
  iconItem->setFont(font);    // Set the font to Arial, 12 points, and bold

  // iconItem->setIcon(cmd->getIcon());
  iconItem->setText(title.data());
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
  mTableTemplates->setItem(newRow, 1, iconItem);

  auto *vectorIndex = new QTableWidgetItem();
  vectorIndex->setText("");
  mTableTemplates->setItem(newRow, 0, vectorIndex);

  mTableTemplates->setSpan(newRow, 1, 1, 2);    // Starts at (row 0, column 0) and spans 1 row and 2 columns
}

int DialogOpenTemplate::addTemplateToTable(const joda::templates::TemplateParser::Data &data, const std::string &category)
{
  mTemplateList.emplace_back(data);
  int newRow = mTableTemplates->rowCount();
  mTemplateMap.emplace(mTemplateList.size() - 1, newRow);
  mTableTemplates->insertRow(newRow);

  QString text = QString(data.title.data()) + "<br/><span style='color:gray;'><i>" + QString(data.description.data()) + "</i></span>";

  // Set the icon in the first column
  auto *textIcon = new QLabel();
  textIcon->setText(text);
  textIcon->setTextFormat(Qt::RichText);
  mTableTemplates->setCellWidget(newRow, 2, textIcon);

  QIcon icon;
  if(!data.icon.isNull()) {
    icon = QIcon(data.icon.scaled(28, 28));

  } else {
    icon = generateSvgIcon("favorite");
  }
  auto *iconItem = new QTableWidgetItem();
  iconItem->setIcon(icon);
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
  mTableTemplates->setItem(newRow, 1, iconItem);

  auto *vectorIndex = new QTableWidgetItem();
  vectorIndex->setText(std::to_string(mTemplateList.size() - 1).data());
  mTableTemplates->setItem(newRow, 0, vectorIndex);
  return 1;
}

void DialogOpenTemplate::filterCommands(const TemplateTableFilter &filter)
{
  return;
  auto searchTexts = filter.searchText.toLower();
  std::set<std::string> categories;
  for(int32_t n = 0; n < mTemplateList.size(); n++) {
    const auto &command = mTemplateList.at(n);
    int32_t tableIndex  = mTemplateMap.at(n);
    auto filterCategory = filter.category.toStdString();
    if(command.group == filterCategory || filterCategory.empty()) {
      mTableTemplates->setRowHidden(tableIndex, true);
    } else if(QString(command.title.data()).contains(searchTexts) || QString(command.description.data()).contains(searchTexts)) {
      // Enable
      categories.emplace(command.group);
      mTableTemplates->setRowHidden(tableIndex, false);
    } else {
      bool found = false;
      for(const auto &tag : command.tags) {
        QString sTag(tag.data());
        if(sTag.contains(searchTexts)) {
          found = true;
          break;
        }
      }
      // Disable
      mTableTemplates->setRowHidden(tableIndex, !found);
    }
  }

  for(const auto &cat : categories) {
    int32_t tableIndex = mTitleINdex.at(cat);
    if(categories.contains(cat)) {
      mTableTemplates->setRowHidden(tableIndex, false);
    } else {
      mTableTemplates->setRowHidden(tableIndex, true);
    }
  }
}

}    // namespace joda::ui::gui
