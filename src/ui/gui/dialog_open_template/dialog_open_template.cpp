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
#include <qmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <QKeyEvent>
#include "backend/user_settings/user_settings.hpp"
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

  connect(mTableTemplates, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    auto idx              = mTableTemplates->item(row, 0)->text().toInt();
    mSelectedTemplatePath = mTemplateList.at(idx).path.data();
    mReturnCode           = ReturnCode::OPEN_TEMPLATE;
    close();
  });

  // Create buttons
  auto *okButton = new QPushButton("OK", this);
  okButton->setDefault(true);
  connect(okButton, &QPushButton::pressed, [&]() {
    QList<QTableWidgetItem *> selectedItems = mTableTemplates->selectedItems();
    if(!selectedItems.isEmpty()) {
      auto row              = selectedItems.first()->row();
      auto idx              = mTableTemplates->item(row, 0)->text().toInt();
      mSelectedTemplatePath = mTemplateList.at(idx).path.data();
      mReturnCode           = ReturnCode::OPEN_TEMPLATE;
      close();
    }
  });
  auto *cancelButton = new QPushButton("Cancel", this);
  connect(cancelButton, &QPushButton::pressed, [&]() {
    mReturnCode = ReturnCode::CANCEL;
    mSelectedTemplatePath.clear();
    close();
  });

  // Open button
  mOpenProjectMenu = new QMenu();
  auto *openButton = new QToolButton(this);
  connect(openButton, &QToolButton::clicked, [&]() {
    mReturnCode = ReturnCode::OPEN_FILE_DIALOG;
    close();
  });
  openButton->setText("Open file");
  openButton->setPopupMode(QToolButton::MenuButtonPopup);    // <- Shows a split button with an arrow
  openButton->setMenu(mOpenProjectMenu);
  openButton->setSizePolicy(okButton->sizePolicy());
  openButton->setFont(okButton->font());
  openButton->setMinimumHeight(okButton->sizeHint().height());

  auto *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(openButton);
  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(okButton);

  //
  auto *layout = new QVBoxLayout();
  layout->addWidget(mSearch);
  layout->addWidget(mTableTemplates);
  layout->addLayout(buttonLayout);

  setLayout(layout);
  setMinimumHeight(500);
  setMinimumWidth(500);

  loadTemplates();
}

///
/// \brief      Load last opened files
/// \author     Joachim Danmayr
///
void DialogOpenTemplate::loadLastOpened()
{
  mOpenProjectMenu->clear();
  mOpenProjectMenu->addSection("Projects");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedProject()) {
    auto *action = mOpenProjectMenu->addAction(path.path.data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() {
      mSelectedTemplatePath = path.data();
      mReturnCode           = ReturnCode::OPEN_PROJECT;
      close();
    });
  }
  mOpenProjectMenu->addSection("Results");
  for(const auto &path : joda::user_settings::UserSettings::getLastOpenedResult()) {
    auto *action = mOpenProjectMenu->addAction((path.path + " (" + path.title + ")").data());
    connect(action, &QAction::triggered, this, [this, path = path.path]() {
      mSelectedTemplatePath = path.data();
      mReturnCode           = ReturnCode::OPEN_RESULTS;
      close();
    });
  }
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

std::pair<DialogOpenTemplate::ReturnCode, QString> DialogOpenTemplate::show()
{
  loadLastOpened();
  mSelectedTemplatePath.clear();
  filterCommands({mSearch->text()});
  exec();

  if(mReturnCode != ReturnCode::CANCEL) {
    if(mSelectedTemplatePath.endsWith(joda::fs::EXT_PROJECT.data())) {
      mReturnCode = ReturnCode::OPEN_PROJECT;
    }
    if(mSelectedTemplatePath.endsWith(joda::fs::EXT_PROJECT_TEMPLATE.data())) {
      mReturnCode = ReturnCode::OPEN_TEMPLATE;
    }
    if(mSelectedTemplatePath.endsWith(joda::fs::EXT_DATABASE.data())) {
      mReturnCode = ReturnCode::OPEN_RESULTS;
    }
    if(mSelectedTemplatePath == "empty") {
      mReturnCode = ReturnCode::EMPTY_PROJECT;
    }
  }

  return {mReturnCode, mSelectedTemplatePath};
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void DialogOpenTemplate::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(mDirectories, mEndian);

  mTableTemplates->setRowCount(0);

  addTemplateToTable({.group       = "",
                      .title       = "Empty project",
                      .description = "Create a new, empty project.",
                      .path        = "empty",
                      .icon        = generateSvgIcon("document-new").pixmap(16, 16)},
                     "");

  std::string actGroup = "basic";
  for(const auto &[_, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(actGroup != data.group) {
        actGroup = data.group;
        addTitleToTable(data.group, data.group);
      }
      addTemplateToTable(data, data.group);
    }
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

  QString text;
  if(!data.description.empty()) {
    text = QString(data.title.data()) + "<br/><span style='color:gray;'><i>" + QString(data.description.data()) + "</i></span>";
  } else {
    text = QString(data.title.data());
  }

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
  auto searchTexts = filter.searchText.toLower();
  std::set<std::string> groups;
  for(int32_t n = 0; n < mTemplateList.size(); n++) {
    const auto &command = mTemplateList.at(n);
    int32_t tableIndex  = mTemplateMap.at(n);
    auto filterCategory = filter.category.toStdString();
    if(command.group == filterCategory && !filterCategory.empty()) {
      mTableTemplates->setRowHidden(tableIndex, true);
    } else if(QString(command.title.data()).contains(searchTexts) || QString(command.description.data()).contains(searchTexts) ||
              command.path == "empty") {
      // Enable
      groups.emplace(command.group);
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
      if(found) {
        groups.emplace(command.group);
      }
      mTableTemplates->setRowHidden(tableIndex, !found);
    }
  }

  for(const auto &[groupToShow, idx] : mTitleINdex) {
    if(groups.contains(groupToShow)) {
      mTableTemplates->setRowHidden(idx, false);
    } else {
      mTableTemplates->setRowHidden(idx, true);
    }
  }
}

}    // namespace joda::ui::gui
