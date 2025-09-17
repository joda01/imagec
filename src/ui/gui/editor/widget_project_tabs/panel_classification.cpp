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

#include "panel_classification.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgridlayout.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <exception>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/colord_square_delegate.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"
#include "ui/gui/results/window_results.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

PanelClassification::PanelClassification(joda::settings::Classification &settings, WindowMain *windowMain) :
    mWindowMain(windowMain), mSettings(settings)
{
  mClassSettingsDialog = new DialogClassSettings(windowMain);
  auto *layout         = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  {
    auto *toolbar = new QToolBar();
    toolbar->setObjectName("SubToolBar");
    toolbar->setIconSize(QSize(16, 16));

    //
    // Add class
    //
    mTemplateMenu  = new QMenu();
    auto *newClass = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("plus"), "Add object class");
    connect(newClass, &QAction::triggered, [this]() { addClass(); });
    newClass->setStatusTip("Add object class or load from template");
    newClass->setMenu(mTemplateMenu);
    toolbar->addAction(newClass);

    //
    // Populate from image
    // object-ungroup
    //
    auto *populateFromImage = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("magic-wand"), "Populate from image channels");
    populateFromImage->setStatusTip("Automatically populate classes from image channels");
    toolbar->addAction(populateFromImage);
    connect(populateFromImage, &QAction::triggered, [this]() { this->populateClassesFromImage(); });

    toolbar->addSeparator();

    //
    // Open template
    //
    auto *openTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("folder-open"), "Open object class template");
    openTemplate->setStatusTip("Open object class template");
    connect(openTemplate, &QAction::triggered, [this]() {
      QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
      QString filePathOfSettingsFile = QFileDialog::getOpenFileName(
          this, "Open template", folderToOpen, "ImageC classification templates (*" + QString(joda::fs::EXT_CLASS_CLASS_TEMPLATE.data()) + ")");
      if(filePathOfSettingsFile.isEmpty()) {
        return;
      }
      this->openTemplate(filePathOfSettingsFile);
    });
    toolbar->addAction(openTemplate);

    //
    // Save as template
    //
    auto *saveAsTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("floppy-disk"), "Save classification settings as template");
    saveAsTemplate->setStatusTip("Save classification settings as template");
    connect(saveAsTemplate, &QAction::triggered, [this]() { saveAsNewTemplate(); });
    toolbar->addAction(saveAsTemplate);

    toolbar->addSeparator();

    //
    // Move down
    //
    auto *moveDown = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-down"), "Move down");
    moveDown->setStatusTip("Move selected pipeline down");
    connect(moveDown, &QAction::triggered, this, &PanelClassification::moveDown);
    toolbar->addAction(moveDown);

    //
    // Move up
    //
    auto *moveUp = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-up"), "Move up");
    moveUp->setStatusTip("Move selected pipeline up");
    connect(moveUp, &QAction::triggered, this, &PanelClassification::moveUp);
    toolbar->addAction(moveUp);

    toolbar->addSeparator();

    //
    // Copy selection
    //
    auto *copy = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("copy"), "Copy selected class");
    copy->setStatusTip("Copy selected class");
    connect(copy, &QAction::triggered, [this]() {
      QList<QTableWidgetSelectionRange> ranges = mClasses->selectedRanges();
      if(!ranges.isEmpty()) {
        int selectedRow = ranges.first().topRow();
        if(selectedRow > 0) {
          selectedRow--;    // Row zero is None
          auto actClass = mSettings.classes.begin();
          std::advance(actClass, selectedRow);

          joda::settings::Class newCreatedClass;
          newCreatedClass.classId             = findNextFreeClassId();
          newCreatedClass.color               = actClass->color;
          newCreatedClass.defaultMeasurements = actClass->defaultMeasurements;
          newCreatedClass.name                = actClass->name + " (copy)";
          newCreatedClass.notes               = actClass->notes;
          mSettings.classes.emplace_back(newCreatedClass);

          onSettingChanged();
        }
      }
    });
    toolbar->addAction(copy);
    toolbar->addSeparator();

    //
    // Delete column
    //
    auto *deleteColumn = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("trash-simple"), "Delete selected class", this);
    deleteColumn->setStatusTip("Delete selected class");
    toolbar->addAction(deleteColumn);
    connect(deleteColumn, &QAction::triggered, [this]() {
      QList<QTableWidgetSelectionRange> ranges = mClasses->selectedRanges();
      if(!ranges.isEmpty()) {
        int selectedRow = ranges.first().topRow();
        if(selectedRow > 0) {
          if(askForDeleteClass()) {
            mClasses->removeRow(selectedRow);
            auto it = mSettings.classes.begin();
            std::advance(it, selectedRow - 1);
            mSettings.classes.erase(it);
            onSettingChanged();
          }
        }
      }
    });

    // toolbar->addSeparator();

    //
    // Clear
    //
    // auto *clearList = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("trash-simple"), "Clear");
    // clearList->setStatusTip("Clear classification list");
    // toolbar->addAction(clearList);
    // connect(clearList, &QAction::triggered, [this]() {
    //  if(this->askForChangeTemplateIndex()) {
    //    this->newTemplate();
    //  }
    //});
    layout->addWidget(toolbar);

    loadTemplates();
  }

  {
    mClasses = new PlaceholderTableWidget(0, 6);
    mClasses->setFrameStyle(QFrame::NoFrame);
    mClasses->setShowGrid(false);
    mClasses->setPlaceholderText("Press the + button to add a class or use the wizard.");
    mClasses->verticalHeader()->setVisible(false);
    mClasses->horizontalHeader()->setVisible(true);
    mClasses->setHorizontalHeaderLabels({"IdNr", "Id", "Classes", "Color", "Notes", "Hidden"});
    mClasses->setAlternatingRowColors(true);
    mClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
    mClasses->setColumnHidden(COL_ID, true);
    mClasses->setColumnHidden(COL_ID_ENUM, true);
    mClasses->setColumnHidden(COL_COLOR, true);
    mClasses->setColumnHidden(COL_NOTES, true);
    mClasses->setColumnHidden(COL_HIDDEN, true);
    mClasses->setColumnWidth(COL_ID_ENUM, 10);
    mClasses->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    mClasses->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    auto *delegate = new ColoredSquareDelegate(mClasses);
    mClasses->setItemDelegateForColumn(COL_NAME, delegate);    // Set the delegate for the desired column

    layout->addWidget(mClasses);
  }
  setLayout(layout);

  // connect(mClasses, &QTableWidget::itemChanged, [&](QTableWidgetItem *item) { onSettingChanged(); });
  connect(mClasses, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    if(row > 0) {
      row--;
      if(column == COL_NAME) {
        openEditDialog(row, column);
      }
    }
  });

  // The non class should always be present
  addNoneClass();

  // connect(mClasses, &QTableWidget::currentCellChanged, [&](int currentRow, int currentColumn, int previousRow, int previousColumn) {});
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::addNoneClass()
{
  mClasses->setRowCount(1);
  createTableItem(0, enums::ClassId::NONE, "None", "#565656", "");
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::openEditDialog(int row, int /*column*/)
{
  auto it = mSettings.classes.begin();
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
auto PanelClassification::findNextFreeClassId() -> enums::ClassId
{
  std::set<enums::ClassId> classIds;
  // Sort class IDs
  for(const auto &actualClass : mSettings.classes) {
    classIds.emplace(actualClass.classId);
  }
  // Iterate over all classIds and find the first not used
  enums::ClassId idx = enums::ClassId::C1;
  for(const auto &classId : classIds) {
    if(idx != classId) {
      return idx;
    }
    if(static_cast<uint16_t>(idx) >= static_cast<uint16_t>(enums::ClassId::CMAX)) {
      break;
    }
    idx = static_cast<enums::ClassId>(static_cast<uint16_t>(idx) + 1);
  }
  return idx;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::addClass(bool withUpdate)
{
  joda::settings::Class newClass;
  newClass.classId = findNextFreeClassId();
  if(mClassSettingsDialog->exec(newClass) == 0) {
    mSettings.classes.emplace_back(newClass);
  }
  if(withUpdate) {
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
void PanelClassification::createTableItem(int32_t rowIdx, enums::ClassId classId, const std::string &name, const std::string &color,
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
    calculatedColor = QString(joda::settings::COLORS.at(static_cast<uint64_t>(rowIdx) % joda::settings::COLORS.size()).data());
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
void PanelClassification::fromSettings(const joda::settings::Classification &settings)
{
  mClasses->blockSignals(true);
  mClasses->setRowCount(static_cast<int32_t>(settings.classes.size()) + 1);    // +1 because none is always shown
  mSettings = settings;

  //
  // Load classes
  //
  int rowIdx = 1;    // We start at 1 because 0 is reserved for none
  for(const auto &classs : settings.classes) {
    createTableItem(rowIdx, classs.classId, classs.name, classs.color, classs.notes);
    rowIdx++;
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
  int32_t row = 1;    // we start at 1 because 0 is none.
  for(auto &classs : mSettings.classes) {
    QTableWidgetItem *itemNotes = mClasses->item(row, COL_NOTES);
    QString classNotes;
    if(itemNotes != nullptr && !itemNotes->text().isEmpty()) {
      classNotes = itemNotes->text();
    }
    classs.notes = classNotes.toStdString();
    row++;
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
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::UNDEFINED), QString("Off"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_01), QString("Memory 01"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_02), QString("Memory 02"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_03), QString("Memory 03"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_04), QString("Memory 04"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_05), QString("Memory 05"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_06), QString("Memory 06"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_07), QString("Memory 07"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_08), QString("Memory 08"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_09), QString("Memory 09"));

  for(const auto &classs : mSettings.classes) {
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
  fromSettings(mSettings);
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
  auto foundTemplates =
      joda::templates::TemplateParser::findTemplates({joda::templates::TemplateParser::getGlobalTemplateDirectory("classification").string(),
                                                      joda::templates::TemplateParser::getUsersTemplateDirectory().string()},
                                                     joda::fs::EXT_CLASS_CLASS_TEMPLATE);

  mTemplateMenu->clear();
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, dataIn] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateMenu->addSeparator();
        }
      }
      QAction *action;
      if(!dataIn.icon.isNull()) {
        action = mTemplateMenu->addAction(QIcon(dataIn.icon.scaled(28, 28)), dataIn.title.data());
      } else {
        action = mTemplateMenu->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("star"), dataIn.title.data());
      }
      connect(action, &QAction::triggered, [this, path = dataIn.path]() { openTemplate(path.data()); });
    }
    addedPerCategory = dataInCategory.size();
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

  nlohmann::json json = mWindowMain->getSettings().projectSettings.classification;
  auto storedFileName =
      joda::templates::TemplateParser::saveTemplate(json, std::filesystem::path(pathToStoreFileIn.toStdString()), joda::fs::EXT_CLASS_CLASS_TEMPLATE);
  loadTemplates();
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
  messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::BLUE>("warning-circle").pixmap(48, 48));
  messageBox.setWindowTitle("Proceed?");
  messageBox.setText("Actual taken settings will get lost!");
  QPushButton *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  messageBox.exec();
  return messageBox.clickedButton() != noButton;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool PanelClassification::askForDeleteClass()
{
  QMessageBox messageBox(mWindowMain);
  messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
  messageBox.setWindowTitle("Proceed?");
  messageBox.setText("Remove selected class?");
  QPushButton *noButton = messageBox.addButton(tr("No"), QMessageBox::NoRole);
  messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
  messageBox.setDefaultButton(noButton);
  messageBox.exec();
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
  mWindowMain->mutableSettings().projectSettings.classification.meta.revision = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.uid      = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.icon     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "User defined";
  mSettings.classes.clear();
  onSettingChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::openTemplate(const QString &path)
{
  try {
    joda::settings::Classification settings = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(path.toStdString()));
    mWindowMain->mutableSettings().projectSettings.classification = settings;
    fromSettings(settings);
    onSettingChanged();
  } catch(const std::exception &ex) {
    joda::log::logWarning("Could not load template >" + path.toStdString() + "<. What: " + std::string(ex.what()));
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::populateClassesFromImage()
{
  if(mSettings.classes.empty() || askForChangeTemplateIndex()) {
    auto [path, series, omeInfo] = mWindowMain->getImagePanel()->getSelectedImageOrFirst();
    if(path.empty()) {
      joda::log::logError("No images found! Please select an image directory first!");
      QMessageBox messageBox(this);
      messageBox.setIconPixmap(generateSvgIcon<Style::REGULAR, Color::YELLOW>("warning").pixmap(48, 48));
      messageBox.setWindowTitle("Could not find any images!");
      messageBox.setText("No images found! Please select a image directory first!");
      messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
      messageBox.exec();
      return;
    }

    auto classes = mWindowMain->getController()->populateClassesFromImage(omeInfo, series);
    fromSettings(classes);
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
void PanelClassification::moveUp()
{
  mClasses->blockSignals(true);
  auto rowAct = mClasses->currentRow();
  auto newPos = rowAct - 1;
  if(newPos <= 0) {    // Position 0 is reserved for None class
    return;
  }
  moveClassToPosition(rowAct, newPos);
  mClasses->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::moveDown()
{
  mClasses->blockSignals(true);
  auto rowAct = mClasses->currentRow();
  auto newPos = rowAct + 1;
  if(newPos >= mClasses->rowCount()) {
    return;
  }
  moveClassToPosition(rowAct, newPos);
  mClasses->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::moveClassToPosition(int32_t fromPos, int32_t newPosIn)
{
  auto moveElementToListPosition = [](std::list<joda::settings::Class> &myList, int32_t oldPos, int32_t newPos) {
    // Get iterators to the old and new positions
    if(newPos > oldPos) {
      auto oldIt = std::next(myList.begin(), newPos);
      auto newIt = std::next(myList.begin(), oldPos);
      // Splice the element at oldIt to before newIt
      myList.splice(newIt, myList, oldIt);
    } else {
      auto oldIt = std::next(myList.begin(), oldPos);
      auto newIt = std::next(myList.begin(), newPos);
      // Splice the element at oldIt to before newIt
      myList.splice(newIt, myList, oldIt);
    }
  };

  auto moveRow = [&](int32_t fromRow, int32_t toRow) {
    if(fromRow == toRow || fromRow < 0 || toRow < 0 || fromRow >= mClasses->rowCount() || toRow > mClasses->rowCount()) {
      return;    // invalid input
    }
    mClasses->setUpdatesEnabled(false);

    int32_t columnCount = mClasses->columnCount();
    for(int32_t col = 0; col < columnCount; ++col) {
      QTableWidgetItem *fromItem = mClasses->takeItem(fromRow, col);
      QTableWidgetItem *toItem   = mClasses->takeItem(toRow, col);

      mClasses->setItem(fromRow, col, toItem);
      mClasses->setItem(toRow, col, fromItem);
    }

    mClasses->setUpdatesEnabled(true);
    mClasses->selectRow(toRow);
  };

  moveElementToListPosition(mSettings.classes, fromPos - 1, newPosIn - 1);    // -1 because the first element in the table is none
  moveRow(fromPos, newPosIn);

  mWindowMain->checkForSettingsChanged();
}

}    // namespace joda::ui::gui
