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
#include <exception>
#include <string>
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_file_endians.hpp"
#include "backend/helper/file_parser/directory_iterator.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/project_settings/project_class.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "backend/settings/project_settings/project_plates.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/colord_square_delegate.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/results/panel_results.hpp"
#include "ui/gui/window_main/window_main.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

PanelClassification::PanelClassification(joda::settings::ProjectSettings &settings, WindowMain *windowMain) :
    mWindowMain(windowMain), mSettings(settings)
{
  createDialog();
  auto *layout = new QVBoxLayout();

  {
    auto *toolbar = new QToolBar();

    //
    // New from template
    //
    mTemplateMenu     = new QMenu();
    auto *newPipeline = new QAction(generateSvgIcon("document-new"), "Add object class");
    connect(newPipeline, &QAction::triggered, [this]() { addClass(); });
    newPipeline->setStatusTip("Add object class or load from template");
    newPipeline->setMenu(mTemplateMenu);
    toolbar->addAction(newPipeline);

    //
    // Open template
    //
    auto *openTemplate = new QAction(generateSvgIcon("folder-stash"), "Open object class template");
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
    // Populate from image
    // object-ungroup
    //
    auto *populateFromImage = new QAction(generateSvgIcon("quickopen-file"), "Populate from image channels");
    populateFromImage->setStatusTip("Populate classes from image channels");
    toolbar->addAction(populateFromImage);
    connect(populateFromImage, &QAction::triggered, [this]() { this->populateClassesFromImage(); });

    //
    // Save as template
    //
    auto *saveAsTemplate = new QAction(generateSvgIcon("document-save-as-template"), "Save classification settings as template");
    saveAsTemplate->setStatusTip("Save classification settings as template");
    connect(saveAsTemplate, &QAction::triggered, [this]() { saveAsNewTemplate(); });
    toolbar->addAction(saveAsTemplate);
    toolbar->addSeparator();

    //
    // Delete column
    //
    auto *deleteColumn = new QAction(generateSvgIcon("edit-table-delete-column"), "Delete selected class", this);
    deleteColumn->setStatusTip("Delete selected class");
    toolbar->addAction(deleteColumn);
    connect(deleteColumn, &QAction::triggered, [this]() {
      QList<QTableWidgetSelectionRange> ranges = mClasses->selectedRanges();
      if(!ranges.isEmpty()) {
        int selectedRow = ranges.first().topRow();
        if(selectedRow >= 0) {
          mClasses->removeRow(selectedRow);
          onSettingChanged();
        }
      }
    });

    //
    // Clear
    //
    auto *clearList = new QAction(generateSvgIcon("edit-delete"), "Clear");
    clearList->setStatusTip("Clear classification list");
    toolbar->addAction(clearList);
    connect(clearList, &QAction::triggered, [this]() {
      if(this->askForChangeTemplateIndex()) {
        this->newTemplate();
      }
    });
    layout->addWidget(toolbar);

    loadTemplates();
  }

  {
    mClasses = new PlaceholderTableWidget(0, 5);
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
  setLayout(layout);

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
void PanelClassification::createDialog()
{
  mEditDialog = new QDialog(mWindowMain);
  mEditDialog->setWindowTitle("Class editor");
  mEditDialog->setMinimumWidth(300);
  auto *layout     = new QVBoxLayout();
  mDialogClassName = new QComboBox();
  mDialogClassName->setEditable(true);
  mDialogClassName->setPlaceholderText("e.g. cy5@spot");
  //
  // Predefined selections
  //
  mDialogClassName->addItem("cy3@spot", "cy3@spot");
  mDialogClassName->addItem("cy3@none", "cy3@none");
  mDialogClassName->addItem("cy3@background", "cy3@background");
  mDialogClassName->addItem("cy5@spot", "cy5@spot");
  mDialogClassName->addItem("cy5@none", "cy5@none");
  mDialogClassName->addItem("cy5@background", "cy5@background");
  mDialogClassName->addItem("cy7@spot", "cy7@spot");
  mDialogClassName->addItem("cy7@none", "cy7@none");
  mDialogClassName->addItem("cy7@background", "cy7@background");
  mDialogClassName->addItem("gfp@spot", "gfp@spot");
  mDialogClassName->addItem("gfp@none", "gfp@none");
  mDialogClassName->addItem("gfp@background", "gfp@background");
  mDialogClassName->addItem("fitc@spot", "fitc@spot");
  mDialogClassName->addItem("fitc@none", "fitc@none");
  mDialogClassName->addItem("fitc@background", "fitc@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("tetraspeck@spot", "tetraspeck@spot");
  mDialogClassName->addItem("tetraspeck@none", "tetraspeck@none");
  mDialogClassName->addItem("tetraspeck@background", "tetraspeck@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("dapi@nucleus", "dapi@nucleus");
  mDialogClassName->addItem("dapi@nucleus-none", "dapi@nucleus-none");
  mDialogClassName->addItem("dapi@background", "dapi@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("brightfield@cell-area", "brightfield@cell-area");
  mDialogClassName->addItem("brightfield@cell-area-none", "brightfield@cell-area-none");
  mDialogClassName->addItem("brightfield@cell", "brightfield@cell");
  mDialogClassName->addItem("brightfield@cell-none", "brightfield@cell-none");
  mDialogClassName->addItem("brightfield@background", "brightfield@background");
  mDialogClassName->insertSeparator(mDialogClassName->count());
  mDialogClassName->addItem("coloc@cy3cy7", "coloc@cy5cy7");
  mDialogClassName->addItem("coloc@cy3cy7-none", "coloc@cy5cy7-none");
  mDialogClassName->addItem("coloc@cy5cy7", "coloc@c5c7");
  mDialogClassName->addItem("coloc@cy5cy7-none", "coloc@cy5cy7-none");

  mDialogColorCombo = new ColorComboBox();
  auto *model       = qobject_cast<QStandardItemModel *>(mDialogColorCombo->model());

  // Set the custom delegate
  for(const auto &color : settings::COLORS) {
    QString colorStr = color.data();
    auto *item       = new QStandardItem(colorStr);
    item->setBackground(QColor(colorStr));                  // Set the background color
    item->setData(QColor(colorStr), Qt::BackgroundRole);    // Store background color for use in the delegate
    model->appendRow(item);
  }

  // Create buttons
  auto *okButton = new QPushButton("OK", this);
  okButton->setDefault(true);
  connect(okButton, &QPushButton::pressed, this, &PanelClassification::onOkayPressed);
  auto *cancelButton = new QPushButton("Cancel", this);
  connect(cancelButton, &QPushButton::pressed, [&]() { mEditDialog->close(); });

  //
  // Grid Layout for the measurements
  //
  auto *measureLayout = new QGridLayout();
  auto addMeasure     = [&](const QString &text, int32_t row, int32_t col) {
    auto *button = new QPushButton(text);
    button->setCheckable(true);
    button->setMaximumWidth(150);
    button->setMinimumWidth(150);
    measureLayout->addWidget(button, row, col + 1);
  };

  auto addIcon = [&](const QString &icon, int32_t row) {
    auto *label = new QLabel();
    label->setPixmap(generateSvgIcon(icon).pixmap(16, 16));
    label->setMaximumWidth(22);
    label->setMinimumWidth(22);
    measureLayout->addWidget(label, row, 0);

    auto *separator = new QFrame;
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    measureLayout->addWidget(separator, row, 1, 1, 3);
  };

  addIcon("format-precision-more", 0);
  addMeasure("Count", 1, 0);
  addMeasure("Intersection count", 1, 1);

  addIcon("insert-horizontal-rule", 2);
  addMeasure("Area size", 3, 0);
  addMeasure("Perimeter", 3, 1);
  addMeasure("Circularity", 3, 2);

  addIcon("brightness-high", 4);
  addMeasure("Intensity min", 5, 0);
  addMeasure("Intensity max", 5, 1);
  addMeasure("Intensity avg", 5, 2);
  addMeasure("Intensity sum", 6, 0);

  addIcon("coordinate", 7);
  addMeasure("Position", 8, 0);
  addMeasure("Distance to surface min", 9, 0);
  addMeasure("Distance to surface max", 9, 1);
  addMeasure("Distance to center min", 10, 0);
  addMeasure("Distance to center max", 10, 1);

  addIcon("irc-operator", 11);
  addMeasure("Object ID", 12, 0);
  addMeasure("Parent object ID", 12, 1);
  addMeasure("Origin object ID", 12, 2);
  addMeasure("Tracking ID", 13, 0);

  measureLayout->setColumnMinimumWidth(0, 24);
  measureLayout->setColumnStretch(0, 0);    // prevent stretching

  //
  // Create a horizontal layout for the buttons
  //
  auto *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(cancelButton);
  buttonLayout->addWidget(okButton);

  //
  // Add to final layout
  //
  layout->addWidget(mDialogClassName);
  layout->addWidget(mDialogColorCombo);
  layout->addLayout(measureLayout);
  layout->addLayout(buttonLayout);

  mEditDialog->setLayout(layout);
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
  mSelectedRow = row;
  mDialogClassName->setCurrentText(mClasses->item(row, COL_NAME)->text());
  auto colorIdx = mDialogColorCombo->findData(QColor(mClasses->item(row, COL_COLOR)->text()), Qt::BackgroundRole);
  if(colorIdx >= 0) {
    mDialogColorCombo->setCurrentIndex(colorIdx);
  }
  mDialogClassName->setEnabled(true);
  mEditDialog->exec();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::onOkayPressed()
{
  auto findNextFreeClassId = [this]() -> enums::ClassId {
    std::set<enums::ClassId> classIds;
    // Sort class IDs
    for(const auto &actualClass : mSettings.classification.classes) {
      classIds.emplace(actualClass.classId);
    }

    // Iterate over all classIds and find the first not used
    enums::ClassId idx = enums::ClassId::C0;
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
  };

  if(mSelectedRow < 0) {
    mClasses->setRowCount(mClasses->rowCount() + 1);
    mSelectedRow         = mClasses->rowCount() - 1;
    auto nextFreeClassId = findNextFreeClassId();
    createTableItem(mSelectedRow, nextFreeClassId, mDialogClassName->currentText().toStdString(), mDialogColorCombo->currentText().toStdString(), "");
  } else {
    mClasses->item(mSelectedRow, COL_NAME)->setText(mDialogClassName->currentText());
    mClasses->item(mSelectedRow, COL_COLOR)->setText(mDialogColorCombo->currentText());
  }
  onSettingChanged();
  mEditDialog->close();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::addClass()
{
  mSelectedRow = -1;
  mDialogClassName->setCurrentText("");
  int32_t newRowIdx = mClasses->rowCount() + 1;
  auto colorIdx =
      mDialogColorCombo->findData(QColor(QString(joda::settings::COLORS.at(newRowIdx % joda::settings::COLORS.size()).data())), Qt::BackgroundRole);
  mDialogColorCombo->setCurrentIndex(colorIdx);
  mEditDialog->exec();
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
void PanelClassification::fromSettings(const joda::settings::Classification &settings)
{
  mClasses->blockSignals(true);
  mClasses->setRowCount(settings.classes.size());

  //
  // Load classes
  //
  int rowIdx = 0;
  for(const auto &classs : settings.classes) {
    createTableItem(rowIdx, classs.classId, classs.name, classs.color, classs.notes);
    rowIdx++;
  }

  toSettings();
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
    auto classId                = static_cast<joda::enums::ClassId>(item->text().toInt());
    QTableWidgetItem *itemName  = mClasses->item(row, COL_NAME);
    auto className              = itemName->text();
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
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_01), QString("Memory 01"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_02), QString("Memory 02"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_03), QString("Memory 03"));
  classes.emplace(static_cast<enums::ClassIdIn>(enums::ClassIdIn::TEMP_04), QString("Memory 04"));

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
      {"templates/classification", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_CLASS_CLASS_TEMPLATE);

  mTemplateMenu->clear();
  std::string actCategory = "basic";
  size_t addedPerCategory = 0;
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplateMenu->addSeparator();
        }
      }
      QAction *action;
      if(!data.icon.isNull()) {
        action = mTemplateMenu->addAction(QIcon(data.icon.scaled(28, 28)), data.title.data());
      } else {
        action = mTemplateMenu->addAction(generateSvgIcon("favorite"), data.title.data());
      }
      connect(action, &QAction::triggered, [this, path = data.path]() { openTemplate(path.data()); });
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
  if(!pathToStoreFileIn.startsWith(templatePath)) {
    joda::log::logError("Templates must be stored in >" + templatePath.toStdString() + "< directory.");
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
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
  messageBox.setIconPixmap(generateSvgIcon("data-information").pixmap(48, 48));
  messageBox.setWindowTitle("Proceed?");
  messageBox.setText("Actual taken settings will get lost!");
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
  mWindowMain->mutableSettings().projectSettings.classification.meta.revision = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.uid      = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.icon     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "";
  mWindowMain->mutableSettings().projectSettings.classification.meta.name     = "User defined";
  mClasses->setRowCount(0);
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
  auto [path, series, omeInfo] = mWindowMain->getImagePanel()->getSelectedImageOrFirst();
  if(path.empty()) {
    joda::log::logError("No images found! Please select a image directory first!");
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not find any images!");
    messageBox.setText("No images found! Please select a image directory first!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
    return;
  }
  auto classes = mWindowMain->getController()->populateClassesFromImage(omeInfo, series);
  if(askForChangeTemplateIndex()) {
    fromSettings(classes);
  }
}

}    // namespace joda::ui::gui
