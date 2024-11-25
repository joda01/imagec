///
/// \file      panel_image.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#include "panel_classification.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <exception>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/helper/color_combo/color_combo.hpp"
#include "ui/helper/colord_square_delegate.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/results/panel_results.hpp"
#include "ui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui {

PanelClassification::PanelClassification(joda::settings::ProjectSettings &settings, WindowMain *windowMain) :
    mWindowMain(windowMain), mSettings(settings)
{
  auto *layout = new QVBoxLayout();

  auto addSeparator = [&layout]() {
    QFrame *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
  };

  {
    auto *templateSelection = new QHBoxLayout();
    mTemplateSelection      = new QComboBox();
    templateSelection->addWidget(mTemplateSelection);

    auto *bookMarkMenu = new QMenu();
    auto *newTemplate  = bookMarkMenu->addAction(generateIcon("file"), "New from template");
    connect(newTemplate, &QAction::triggered, [this]() { this->newTemplate(); });
    auto *saveBookmark = bookMarkMenu->addAction(generateIcon("save"), "Save as new template");
    connect(saveBookmark, &QAction::triggered, [this]() { saveAsNewTemplate(); });

    mBookmarkButton = new QPushButton(generateIcon("bookmark"), "");
    mBookmarkButton->setMenu(bookMarkMenu);
    mBookmarkButton->setToolTip("Bookmark settings!");
    templateSelection->addWidget(mBookmarkButton);

    templateSelection->setStretch(0, 1);
    layout->addLayout(templateSelection);

    loadTemplates();
    connect(mTemplateSelection, &QComboBox::currentIndexChanged, this, &PanelClassification::onloadPreset);
  }

  {
    mClasses = new PlaceholderTableWidget(NR_OF_CLASSES, 5);
    mClasses->setPlaceholderText("Add a class");
    mClasses->verticalHeader()->setVisible(false);
    mClasses->setHorizontalHeaderLabels({"IdNr", "Id", "Class", "Color", "Notes"});
    mClasses->setAlternatingRowColors(true);
    mClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClasses->setColumnHidden(COL_ID, true);
    mClasses->setColumnHidden(COL_ID_ENUM, true);
    mClasses->setColumnHidden(COL_COLOR, true);
    mClasses->setColumnWidth(COL_ID_ENUM, 10);
    mClasses->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    auto *delegate = new ColoredSquareDelegate(mClasses);
    mClasses->setItemDelegateForColumn(COL_NAME, delegate);    // Set the delegate for the desired column

    layout->addWidget(mClasses);
  }
  addSeparator();
  setLayout(layout);

  initTable();
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
void PanelClassification::openEditDialog(int row, int column)
{
  auto *dialog = new QDialog(mWindowMain);
  dialog->setWindowTitle("Class editor");
  dialog->setMinimumWidth(300);
  auto *layout = new QVBoxLayout();
  auto *name   = new QLineEdit();
  name->setPlaceholderText("e.g. cy5 spot");
  name->setText(mClasses->item(row, COL_NAME)->text());
  if(mIsLocked) {
    name->setEnabled(false);
  }
  auto *colors = new ColorComboBox();
  auto *model  = qobject_cast<QStandardItemModel *>(colors->model());

  // Set the custom delegate
  for(const auto &color : settings::COLORS) {
    QString colorStr = color.data();
    auto *item       = new QStandardItem(colorStr);
    item->setBackground(QColor(colorStr));                  // Set the background color
    item->setData(QColor(colorStr), Qt::BackgroundRole);    // Store background color for use in the delegate
    model->appendRow(item);
  }

  auto colorIdx = colors->findData(QColor(mClasses->item(row, COL_COLOR)->text()), Qt::BackgroundRole);
  if(colorIdx >= 0) {
    colors->setCurrentIndex(colorIdx);
  }

  // Create buttons
  auto *okButton = new QPushButton("OK", this);
  okButton->setDefault(true);
  connect(okButton, &QPushButton::pressed, [this, row, dialog, name, colors]() {
    mClasses->item(row, COL_NAME)->setText(name->displayText());
    mClasses->item(row, COL_COLOR)->setText(colors->currentText());
    onSettingChanged();
    dialog->close();
  });
  auto *cancelButton = new QPushButton("Cancel", this);
  connect(cancelButton, &QPushButton::pressed, [&]() { dialog->close(); });

  // Create a horizontal layout for the buttons
  auto *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(okButton);

  layout->addWidget(name);
  layout->addWidget(colors);
  layout->addLayout(buttonLayout);

  dialog->setLayout(layout);

  dialog->exec();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::initTable()
{
  mClasses->blockSignals(true);

  //
  // Load classes
  //
  for(int32_t classId = 0; classId < NR_OF_CLASSES; classId++) {
    auto *index = new QTableWidgetItem(QString::number(classId));
    index->setFlags(index->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, COL_ID, index);

    nlohmann::json classIdStr = static_cast<enums::ClassId>(classId);
    auto *itemEnum            = new QTableWidgetItem(QString(std::string(classIdStr).data()));
    itemEnum->setFlags(itemEnum->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, COL_ID_ENUM, itemEnum);

    auto *item = new QTableWidgetItem(QString(""));
    item->setFlags(itemEnum->flags() & ~Qt::ItemIsEditable);
    mClasses->setItem(classId, COL_NAME, item);

    auto *itemColor = new QTableWidgetItem(QString(joda::settings::COLORS.at(classId % joda::settings::COLORS.size()).data()));
    mClasses->setItem(classId, COL_COLOR, itemColor);

    auto *itemNotes = new QTableWidgetItem(QString(""));
    mClasses->setItem(classId, COL_NOTES, itemNotes);
  }
  mClasses->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::fromSettings(const joda::settings::Classification &settings)
{
  mClasses->blockSignals(true);

  initTable();

  //
  // Load classes
  //
  for(const auto &classs : settings.classes) {
    auto classId = static_cast<int32_t>(classs.classId);

    nlohmann::json classIdStr = classs.classId;
    mClasses->item(classId, COL_ID_ENUM)->setText(QString(std::string(classIdStr).data()));
    mClasses->item(classId, COL_NAME)->setText(classs.name.data());
    mClasses->item(classId, COL_COLOR)->setText(classs.color.data());
    mClasses->item(classId, COL_NOTES)->setText(classs.notes.data());
  }

  mClasses->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::updateTableLock(bool lock)
{
  mIsLocked = lock;
  mClasses->blockSignals(true);

  if(lock) {
    mClasses->horizontalHeaderItem(COL_NAME)->setIcon(generateIcon("lock"));

  } else {
    mClasses->horizontalHeaderItem(COL_NAME)->setIcon({});
  }

  mClasses->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::toSettings()
{
  //
  // Save classes
  //
  mSettings.classification.classes.clear();
  for(int row = 0; row < mClasses->rowCount(); row++) {
    QTableWidgetItem *item = mClasses->item(row, COL_ID);
    if(item == nullptr) {
      continue;
    }
    auto classId = static_cast<joda::enums::ClassId>(item->text().toInt());

    QTableWidgetItem *itemName = mClasses->item(row, COL_NAME);
    if(itemName == nullptr || itemName->text().isEmpty()) {
      continue;
    }
    auto className = itemName->text();

    QTableWidgetItem *itemColor = mClasses->item(row, COL_COLOR);
    QString classColor;
    if(itemColor != nullptr && !itemColor->text().isEmpty()) {
      classColor = itemColor->text();
    }

    QTableWidgetItem *itemNotes = mClasses->item(row, COL_NOTES);
    QString classNotes;
    if(itemNotes != nullptr && !itemNotes->text().isEmpty()) {
      classNotes = itemNotes->text();
    }

    mSettings.classification.classes.emplace_back(joda::settings::Class{
        .classId = classId, .name = className.toStdString(), .color = classColor.toStdString(), .notes = classNotes.toStdString()});
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
[[nodiscard]] auto PanelClassification::getClasses() const -> std::map<enums::ClassIdIn, QString>
{
  std::map<enums::ClassIdIn, QString> classes;

  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::$), QString("Default"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::NONE), QString("None"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::UNDEFINED), QString("Undefined"));

  for(const auto &classs : mSettings.classification.classes) {
    classes.emplace(static_cast<enums::ClassIdIn>(classs.classId), QString(classs.name.data()));
  }

  return classes;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::onSettingChanged()
{
  toSettings();
  mWindowMain->checkForSettingsChanged();
  emit settingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {{"templates/classification", joda::templates::TemplateParser::Category::BASIC},
       {joda::templates::TemplateParser::getUsersTemplateDirectory().string(), joda::templates::TemplateParser::Category::USER}},
      joda::fs::EXT_CLASS_CLASS_TEMPLATE);

  mTemplateSelection->clear();
  mTemplateSelection->addItem("User defined", "");
  mTemplateSelection->insertSeparator(mTemplateSelection->count());

  joda::templates::TemplateParser::Category actCategory = joda::templates::TemplateParser::Category::BASIC;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        mTemplateSelection->insertSeparator(mTemplateSelection->count());
      }
      if(!data.icon.isNull()) {
        mTemplateSelection->addItem(QIcon(data.icon.scaled(28, 28)), data.title.data(), data.path.data());
      } else {
        mTemplateSelection->addItem(generateIcon("favorite"), data.title.data(), data.path.data());
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::saveAsNewTemplate()
{
  QString templatePath      = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString pathToStoreFileIn = QFileDialog::getSaveFileName(
      this, "Save File", templatePath, "ImageC classification template (*" + QString(joda::fs::EXT_CLASS_CLASS_TEMPLATE.data()) + ")");

  if(pathToStoreFileIn.isEmpty()) {
    return;
  }
  if(!pathToStoreFileIn.startsWith(templatePath)) {
    joda::log::logError("Templates must be stored in >" + templatePath.toStdString() + "< directory.");
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateIcon("warning-yellow").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Templates must be stored in >" + templatePath + "< directory.");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
    return;
  }

  nlohmann::json json = mWindowMain->getSettings().projectSettings.classification;
  auto storedFileName =
      joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(pathToStoreFileIn.toStdString()), joda::fs::EXT_CLASS_CLASS_TEMPLATE);
  loadTemplates();

  auto idx = mTemplateSelection->findData(QString(storedFileName.string().data()));
  if(idx > 0) {
    mDontAsk = true;
    mTemplateSelection->setCurrentIndex(idx);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool PanelClassification::askForChangeTemplateIndex()
{
  QMessageBox messageBox(mWindowMain);
  messageBox.setIconPixmap(generateIcon("info-blue").pixmap(48, 48));
  messageBox.setWindowTitle("Load preset?");
  messageBox.setText("Load new classification preset? Actual taken settings will get lost!");
  QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  auto reply = messageBox.exec();
  return messageBox.clickedButton() != noButton;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::newTemplate()
{
  mActSelectedIndex = 0;
  mTemplateSelection->setCurrentIndex(0);
  updateTableLock(false);
  mWindowMain->mutableSettings().projectSettings.classification.meta.revision = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.uid      = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.icon     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "User defined";
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::onloadPreset(int index)
{
  if(index == mActSelectedIndex) {
    return;
  }
  auto selection = mTemplateSelection->currentData().toString();

  if(selection == "") {
    newTemplate();
  } else {
    if(mDontAsk) {
      mDontAsk = false;
    } else {
      if(!askForChangeTemplateIndex()) {
        mTemplateSelection->setCurrentIndex(mActSelectedIndex);    // Revert to previous selection
        return;
      }
    }

    mActSelectedIndex = index;

    try {
      joda::settings::Classification settings =
          joda::templates::TemplateParser::loadTemplate(std::filesystem::path(mTemplateSelection->currentData().toString().toStdString()));
      mWindowMain->mutableSettings().projectSettings.classification = settings;
      fromSettings(settings);
      updateTableLock(true);
    } catch(const std::exception &ex) {
    }
  }
}

}    // namespace joda::ui
