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
#include <qsplitter.h>
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
#include "ui/gui/dialogs/dialog_image_view/dialog_image_view.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/dialog_roi_manager.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_model_roi.hpp"
#include "ui/gui/editor/widget_project_tabs/panel_image.hpp"
#include "ui/gui/editor/widget_project_tabs/table_model_classes.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/color_combo/color_combo.hpp"
#include "ui/gui/helper/colord_square_delegate.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/table_view.hpp"
#include "ui/gui/results/dialog_class_settings.hpp"
#include "ui/gui/results/window_results.hpp"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::gui {

PanelClassification::PanelClassification(const std::shared_ptr<atom::ObjectList> &objectMap, joda::settings::Classification *settings,
                                         WindowMain *windowMain, DialogImageViewer *imageView) :
    mWindowMain(windowMain),
    mSettings(settings), mObjectMap(objectMap), mDialogImageView(imageView)
{
  mClassSettingsDialog = new DialogClassSettings(windowMain);
  auto *layout         = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);

  {
    auto *toolbar = new QToolBar();
    toolbar->setObjectName("SubToolBar");
    toolbar->setIconSize(QSize(16, 16));

    auto *submenu = new QMenu();

    //
    // Add class
    //
    mTemplateMenu  = new QMenu();
    auto *newClass = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("plus"), "Add object class");
    connect(newClass, &QAction::triggered, [this]() { addClass(); });
    newClass->setStatusTip("Add object class or load from template");
    toolbar->addAction(newClass);

    //
    // Populate from image
    // object-ungroup
    //
    auto *populateFromImage = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("target"), "Populate from image channels");
    populateFromImage->setStatusTip("Automatically populate classes from image channels");
    toolbar->addAction(populateFromImage);
    connect(populateFromImage, &QAction::triggered, [this]() { this->populateClassesFromImage(); });

    //
    // Hide class
    //
    mActionHideClass = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("eye-slash"), "Hide class");
    mActionHideClass->setCheckable(true);
    connect(mActionHideClass, &QAction::triggered, [this](bool checked) {
      auto indexes = mTableClasses->selectionModel()->selectedIndexes();
      if(!indexes.isEmpty()) {
        int selectedRow = indexes.first().row();
        if(selectedRow >= 0) {
          mTableModelClasses->hideElement(selectedRow, checked);
        }
      }
      mDialogImageView->getImagePanel()->setRegionsOfInterestFromObjectList();
    });
    mActionHideClass->setStatusTip("Hide class in the preview");
    toolbar->addAction(mActionHideClass);

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
    submenu->addAction(openTemplate);

    //
    // Save as template
    //
    auto *saveAsTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("floppy-disk"), "Save classification settings as template");
    saveAsTemplate->setStatusTip("Save classification settings as template");
    connect(saveAsTemplate, &QAction::triggered, [this]() { saveAsNewTemplate(); });
    submenu->addAction(saveAsTemplate);

    submenu->addSeparator();

    //
    // Move down
    //
    auto *moveDown = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-down"), "Move down");
    moveDown->setStatusTip("Move selected pipeline down");
    connect(moveDown, &QAction::triggered, this, &PanelClassification::moveDown);
    submenu->addAction(moveDown);

    //
    // Move up
    //
    auto *moveUp = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-up"), "Move up");
    moveUp->setStatusTip("Move selected pipeline up");
    connect(moveUp, &QAction::triggered, this, &PanelClassification::moveUp);
    submenu->addAction(moveUp);

    submenu->addSeparator();

    //
    // Copy selection
    //
    auto *copy = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("copy"), "Copy selected class");
    copy->setStatusTip("Copy selected class");
    connect(copy, &QAction::triggered, [this]() {
      auto indexes = mTableClasses->selectionModel()->selectedIndexes();
      if(!indexes.isEmpty()) {
        int selectedRow = indexes.first().row();
        if(selectedRow > 0) {
          selectedRow--;    // Row zero is None
          auto actClass = mSettings->classes.begin();
          std::advance(actClass, selectedRow);

          mTableModelClasses->beginChange();
          joda::settings::Class newCreatedClass;
          newCreatedClass.classId             = findNextFreeClassId();
          newCreatedClass.color               = actClass->color;
          newCreatedClass.defaultMeasurements = actClass->defaultMeasurements;
          newCreatedClass.name                = actClass->name + " (copy)";
          newCreatedClass.notes               = actClass->notes;
          mSettings->classes.emplace_back(newCreatedClass);
          mTableModelClasses->endChange();

          onSettingChanged();
          QModelIndex indexToSelect = mTableModelClasses->index(
              static_cast<int>(mSettings->classes.size()), 0);    // We can use size since the table has always one element more, the none element
          mTableClasses->selectionModel()->setCurrentIndex(indexToSelect,
                                                           QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Rows);
        }
      }
    });
    submenu->addAction(copy);
    submenu->addSeparator();

    //
    // Delete column
    //
    auto *deleteColumn = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("trash-simple"), "Delete selected class", this);
    deleteColumn->setStatusTip("Delete selected class");
    submenu->addAction(deleteColumn);
    connect(deleteColumn, &QAction::triggered, [this]() {
      auto indexes = mTableClasses->selectionModel()->selectedIndexes();
      if(!indexes.isEmpty()) {
        int selectedRow = indexes.first().row();
        if(selectedRow > 0) {
          if(askForDeleteClass()) {
            auto it = mSettings->classes.begin();
            std::advance(it, selectedRow - 1);
            mSettings->classes.erase(it);
            onSettingChanged();
          }
        }
      }
    });

    // Submenu
    auto *submenuAction = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("dots-three-vertical"), "");
    submenuAction->setMenu(submenu);
    toolbar->addAction(submenuAction);
    auto *btn = qobject_cast<QToolButton *>(toolbar->widgetForAction(submenuAction));
    btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    btn->setStyleSheet("QToolButton::menu-indicator { image: none; }");

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
    auto *splitPane = new QSplitter();
    {
      /////////
      mTableClasses = new PlaceholderTableView(this);
      mTableClasses->setPlaceholderText("Select an annotation ...");
      mTableClasses->setFrameStyle(QFrame::NoFrame);
      mTableClasses->verticalHeader()->setVisible(false);
      mTableClasses->horizontalHeader()->setVisible(true);
      mTableClasses->setAlternatingRowColors(true);
      mTableClasses->setSelectionBehavior(QAbstractItemView::SelectRows);
      mTableClasses->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      mTableClasses->setItemDelegateForColumn(0, new ColoredSquareDelegate(mTableClasses));
      mTableModelClasses = new TableModelClasses(settings, objectMap, mTableClasses);
      mTableClasses->setModel(mTableModelClasses);

      splitPane->addWidget(mTableClasses);
    }

    // ROI details
    {
      mTableRoiDetails = new PlaceholderTableView(this);
      mTableRoiDetails->setPlaceholderText("Select an annotation ...");
      mTableRoiDetails->setFrameStyle(QFrame::NoFrame);
      mTableRoiDetails->verticalHeader()->setVisible(false);
      mTableRoiDetails->horizontalHeader()->setVisible(true);
      mTableRoiDetails->setAlternatingRowColors(true);
      mTableRoiDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
      mTableRoiDetails->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      mTableModelRoi = new TableModelRoi(imageView->getImagePanel(), settings, mTableRoiDetails);
      mTableRoiDetails->setModel(mTableModelRoi);
    }

    {
      splitPane->addWidget(new DialogRoiManager(objectMap, settings, imageView->getImagePanel(), mTableModelRoi, mWindowMain));
    }

    layout->addWidget(splitPane, 2);
    layout->addWidget(mTableRoiDetails, 1);
  }
  setLayout(layout);

  connect(mTableClasses, &QTableView::doubleClicked, [&](const QModelIndex &index) {
    int32_t row = index.row();
    if(row > 0) {
      int32_t tmpRow = row - 1;
      auto it        = std::next(mSettings->classes.begin(), tmpRow);
      openEditDialog(&*it, row);
    }
  });

  //
  // On selected class change set the class id to draw in the image panel
  //
  connect(mTableClasses->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &, const QItemSelection &) {
    auto *imgPanel = mWindowMain->mutableImagePreview()->getImagePanel();
    if(!mTableClasses->selectionModel()->hasSelection()) {
      imgPanel->setClassIdToUseForDrawing(enums::ClassId::NONE, QColor(TableModelClasses::NONE_COLOR.data()));
      mActionHideClass->setChecked(false);
      mActionHideClass->setEnabled(false);
    } else {
      mActionHideClass->setEnabled(true);
      auto indexes     = mTableClasses->selectionModel()->selectedIndexes();
      auto selectedRow = indexes.begin()->row();
      mActionHideClass->setChecked(mTableModelClasses->isHidden(selectedRow));
      if(selectedRow == 0) {
        imgPanel->setClassIdToUseForDrawing(enums::ClassId::NONE, QColor(TableModelClasses::NONE_COLOR.data()));
      } else {
        selectedRow--;
        auto it = mSettings->classes.begin();
        std::advance(it, selectedRow);
        imgPanel->setClassIdToUseForDrawing(it->classId, QColor(it->color.data()));
      }
    }
  });

  // connect(mClasses, &QTableWidget::currentCellChanged, [&](int currentRow, int currentColumn, int previousRow, int previousColumn) {});
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelClassification::openEditDialog(joda::settings::Class *classToModify, int32_t row)
{
  if(mClassSettingsDialog->exec(*classToModify) == 0) {
    QModelIndex indexToUpdt = mTableModelClasses->index(row, 0);
    mTableModelClasses->dataChanged(indexToUpdt, indexToUpdt);
    mSettings->triggerSettingsChanged();
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
  for(const auto &actualClass : mSettings->classes) {
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
    mSettings->classes.emplace_back(newClass);
  }
  if(withUpdate) {
    onSettingChanged();
    mSettings->triggerSettingsChanged();
  }
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
  mTableModelClasses->beginChange();
  *mSettings = settings;
  mTableModelClasses->endChange();
  mSettings->triggerSettingsChanged();
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
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto PanelClassification::getSelectedClass() const -> enums::ClassId
{
  auto ranges = mTableClasses->selectionModel()->selectedIndexes();
  if(!ranges.isEmpty()) {
    int selectedRow = ranges.first().row();
    if(selectedRow == 0) {
      return enums::ClassId::NONE;
    }
    auto it = mSettings->classes.begin();
    std::advance(it, selectedRow - 1);
    return it->classId;
  }
  return enums::ClassId::NONE;
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

  for(const auto &classs : mSettings->classes) {
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
  fromSettings(*mSettings);
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
  mSettings->classes.clear();
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
  if(mSettings->classes.empty() || askForChangeTemplateIndex()) {
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
  QItemSelectionModel *selectionModel = mTableClasses->selectionModel();
  if(selectionModel->hasSelection()) {
    QModelIndex index = selectionModel->selectedRows().first();
    auto rowAct       = index.row();
    auto newPos       = rowAct - 1;
    if(newPos <= 0) {    // First row is reserved for None therefore <=
      return;
    }
    moveClassToPosition(static_cast<uint32_t>(rowAct), static_cast<uint32_t>(newPos));
  }
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
  QItemSelectionModel *selectionModel = mTableClasses->selectionModel();
  if(selectionModel->hasSelection()) {
    QModelIndex index = selectionModel->selectedRows().first();
    auto rowAct       = index.row();
    auto newPos       = rowAct + 1;
    if(newPos >= mTableModelClasses->rowCount()) {
      return;
    }
    moveClassToPosition(static_cast<uint32_t>(rowAct), static_cast<uint32_t>(newPos));
  }
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

  moveElementToListPosition(mSettings->classes, fromPos - 1, newPosIn - 1);    // -1 because the first element in the table is none

  QModelIndex indexToUpdtFrom = mTableModelClasses->index(fromPos, 0);
  mTableModelClasses->dataChanged(indexToUpdtFrom, indexToUpdtFrom);

  QModelIndex indexToUpddTo = mTableModelClasses->index(newPosIn, 0);
  mTableModelClasses->dataChanged(indexToUpddTo, indexToUpddTo);

  mTableClasses->selectionModel()->setCurrentIndex(indexToUpdtFrom,
                                                   QItemSelectionModel::SelectionFlag::Deselect | QItemSelectionModel::SelectionFlag::Rows);
  mTableClasses->selectionModel()->setCurrentIndex(indexToUpddTo,
                                                   QItemSelectionModel::SelectionFlag::Select | QItemSelectionModel::SelectionFlag::Rows);
  mWindowMain->checkForSettingsChanged();
  std::cout << "triggered 01" << std::endl;

  mSettings->triggerSettingsChanged();
}

}    // namespace joda::ui::gui
