///
/// \file      panel_pipeline.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-09
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "panel_pipeline.hpp"
#include <qaction.h>
#include <qcombobox.h>
#include <qmenu.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <cmath>
#include <filesystem>
#include <iterator>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/helper/logger/console_logger.hpp"
#include "backend/settings/pipeline/pipeline.hpp"
#include "ui/gui/container/dialog_command_selection/dialog_command_selection.hpp"
#include "ui/gui/container/dialog_pipeline_settings/dialog_pipeline_settings.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/container/pipeline/table_item_delegate_pipeline.hpp"
#include "ui/gui/container/pipeline/table_model_pipeline.hpp"
#include "ui/gui/helper/droppable_widget/droppable_widget.hpp"
#include "ui/gui/helper/html_delegate.hpp"
#include "ui/gui/helper/html_header.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/pipeline_overview_delegate.hpp"
#include "ui/gui/helper/table_view.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
PanelPipeline::PanelPipeline(WindowMain *windowMain, joda::settings::AnalyzeSettings &settings) : mWindowMain(windowMain), mAnalyzeSettings(settings)
{
  mMainLayout = new QVBoxLayout();
  mMainLayout->setContentsMargins(0, 0, 0, 0);
  auto *toolbar = new QToolBar();
  toolbar->setIconSize(QSize(16, 16));

  {
    //
    // New pipeline
    //
    auto *newPipeline = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("plus"), "Add new pipeline");
    newPipeline->setStatusTip("Add new pipeline or use predefined template");
    connect(newPipeline, &QAction::triggered, [this]() { addChannel(joda::settings::Pipeline{}); });
    mTemplatesMenu = new QMenu();
    newPipeline->setMenu(mTemplatesMenu);
    toolbar->addAction(newPipeline);

    //
    // Open stack options dialof
    //
    auto *pipelineSettings = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("wrench"), "Stack options");
    pipelineSettings->setStatusTip("Open stack options dialog");
    connect(pipelineSettings, &QAction::triggered, [this]() { mStackOptionsDialog->exec(); });
    toolbar->addAction(pipelineSettings);

    //
    // Open template
    //
    auto *openTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("folder-open"), "Open template");
    openTemplate->setStatusTip("Open pipeline from template");
    connect(openTemplate, &QAction::triggered, [this]() {
      QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
      QString filePathOfSettingsFile = QFileDialog::getOpenFileName(
          this, "Open template", folderToOpen, "ImageC template files (*" + QString(joda::fs::EXT_PIPELINE_TEMPLATE.data()) + ")");
      if(filePathOfSettingsFile.isEmpty()) {
        return;
      }

      addChannel(filePathOfSettingsFile);
    });
    toolbar->addAction(openTemplate);

    //
    // Save template
    //
    auto *saveAsTemplateButton = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("floppy-disk"), "Save as template");
    saveAsTemplateButton->setStatusTip("Save pipeline as template");
    connect(saveAsTemplateButton, &QAction::triggered, [this]() { this->saveAsTemplate(); });
    toolbar->addAction(saveAsTemplateButton);

    toolbar->addSeparator();
    //
    // Move down
    //
    auto *moveDown = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-down"), "Move down");
    moveDown->setStatusTip("Move selected pipeline down");
    connect(moveDown, &QAction::triggered, this, &PanelPipeline::moveDown);
    toolbar->addAction(moveDown);

    //
    // Move up
    //
    auto *moveUp = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("caret-up"), "Move up");
    moveUp->setStatusTip("Move selected pipeline up");
    connect(moveUp, &QAction::triggered, this, &PanelPipeline::moveUp);
    toolbar->addAction(moveUp);

    toolbar->addSeparator();

    //
    // Copy selection
    //
    auto *copy = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("copy"), "Copy selected pipeline");
    copy->setStatusTip("Copy selected pipeline");
    connect(copy, &QAction::triggered, [this]() {
      joda::settings::Pipeline copiedPipeline = getSelectedPipeline()->mutablePipeline();
      copiedPipeline.meta.name += " (copy)";
      addChannel(copiedPipeline);
    });
    toolbar->addAction(copy);
    toolbar->addSeparator();

    //
    // Delete column
    //
    auto *deleteColumn = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("trash-simple"), "Delete selected pipeline", this);
    deleteColumn->setStatusTip("Delete selected pipeline");
    toolbar->addAction(deleteColumn);
    connect(deleteColumn, &QAction::triggered, [this]() {
      QMessageBox messageBox(mWindowMain);
      messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
      messageBox.setWindowTitle("Delete pipeline?");
      messageBox.setText("Delete pipeline?");
      QPushButton *noButton  = messageBox.addButton(tr("No"), QMessageBox::NoRole);
      QPushButton *yesButton = messageBox.addButton(tr("Yes"), QMessageBox::YesRole);
      messageBox.setDefaultButton(noButton);
      auto reply = messageBox.exec();
      if(messageBox.clickedButton() == noButton) {
        return;
      }
      mWindowMain->showPanelStartPage();
      erase(getSelectedPipeline());
    });
  }

  {
    mStackOptionsDialog = new QDialog(mWindowMain);
    mStackOptionsDialog->setWindowTitle("Stack handling");
    mStackOptionsDialog->setMinimumWidth(400);
    auto *formLayout = new QFormLayout;
    //
    // Stack handling
    //
    mStackHandlingZ = new QComboBox();
    mStackHandlingZ->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EACH_ONE));
    mStackHandlingZ->addItem("Defined by pipeline", static_cast<int32_t>(joda::settings::ProjectImageSetup::ZStackHandling::EXACT_ONE));
    formLayout->addRow(new QLabel(tr("Z-Stack")), mStackHandlingZ);
    connect(mStackHandlingZ, &QComboBox::currentIndexChanged, [this]() { toSettings(); });

    //
    mStackHandlingT = new QComboBox();
    mStackHandlingT->addItem("Each one", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EACH_ONE));
    mStackHandlingT->addItem("Defined by pipeline", static_cast<int32_t>(joda::settings::ProjectImageSetup::TStackHandling::EXACT_ONE));
    connect(mStackHandlingT, &QComboBox::currentIndexChanged, [this]() { toSettings(); });
    formLayout->addRow(new QLabel(tr("T-Stack")), mStackHandlingT);

    auto *tStackRangeLayout = new QHBoxLayout;
    mTStackFrameStart       = new QLineEdit();
    mTStackFrameStart->setStatusTip("Time frame to start (0 to -1).");
    mTStackFrameEnd = new QLineEdit();
    mTStackFrameEnd->setStatusTip("Time frame to stop (-1 = last time frame).");
    tStackRangeLayout->addWidget(mTStackFrameStart);
    tStackRangeLayout->addWidget(mTStackFrameEnd);
    formLayout->addRow(new QLabel(tr("T-Range")), tStackRangeLayout);

    mStackOptionsDialog->setLayout(formLayout);
  }

  // Create a widget to hold the panels
  {
    mPipelineTable = new PlaceholderTableView(this);
    mPipelineTable->setPlaceholderText("Press the + button to add a new pipeline.");
    mPipelineTable->setFrameStyle(QFrame::NoFrame);
    mPipelineTable->verticalHeader()->setVisible(false);
    mPipelineTable->horizontalHeader()->setVisible(true);
    mPipelineTable->setAlternatingRowColors(true);
    mPipelineTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPipelineTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mPipelineTable->setItemDelegateForColumn(0, new HtmlDelegate(mPipelineTable));
    mPipelineTable->setItemDelegateForColumn(1, new ColorSquareDelegatePipeline(mPipelineTable));
    mTableModel = new TableModelPipeline(mAnalyzeSettings.projectSettings.classification, mPipelineTable);
    mTableModel->setData(&settings.pipelines);
    mPipelineTable->setModel(mTableModel);

    connect(mPipelineTable->selectionModel(), &QItemSelectionModel::currentChanged,
            [&](const QModelIndex &current, const QModelIndex &previous) { openSelectedPipeline(current, previous); });
    connect(mPipelineTable, &QTableView::clicked, [this](const QModelIndex &index) { openSelectedPipeline(index, index); });
    connect(mPipelineTable, &QTableView::doubleClicked, [this](const QModelIndex &index) { openSelectedPipelineSettings(index); });
  }

  mCommandSelectionDialog = std::make_shared<DialogCommandSelection>(mWindowMain);

  mMainLayout->addWidget(toolbar);
  mMainLayout->addWidget(mPipelineTable, 1);

  setLayout(mMainLayout);
}
///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::unselectPipeline()
{
  mPipelineTable->clearSelection();
  removePipelineWidget();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::removePipelineWidget()
{
  //
  // Remove old pipeline form the layout
  //
  int count = mMainLayout->count();
  if(count > 2) {
    if(mActivePipeline != nullptr) {
      mActivePipeline->setActive(false);
      mActivePipeline = nullptr;
    }
    QLayoutItem *item = mMainLayout->takeAt(count - 1);
    if(item != nullptr) {
      QWidget *widget = item->widget();
      if(widget != nullptr) {
        widget->setParent(nullptr);    // Detach from layout and parent
      }
      delete item;    // Delete the layout item wrapper (not the widget!)
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
void PanelPipeline::openSelectedPipeline(const QModelIndex &current, const QModelIndex &previous)
{
  //
  // Add new one
  //
  auto selectedRow = current.row();
  if(selectedRow >= 0) {
    for(auto &pipeline : mChannels) {
      if(&pipeline->mutablePipeline() == mTableModel->getCell(selectedRow)) {
        if(mActivePipeline == pipeline.get()) {
          // Pipeline is still open
          return;
        }
        removePipelineWidget();
        pipeline->setActive(true);
        mActivePipeline = pipeline.get();
        mMainLayout->addWidget(pipeline.get(), 4);
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
void PanelPipeline::openSelectedPipelineSettings(const QModelIndex &current)
{
  openSelectedPipelineSettings(current.row());
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::openSelectedPipelineSettings(int32_t selectedRow)
{
  if(selectedRow >= 0) {
    for(const auto &pipeline : mChannels) {
      if(&pipeline->mutablePipeline() == mTableModel->getCell(selectedRow)) {
        pipeline->openPipelineSettings();
        return;
      }
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto PanelPipeline::getSelectedPipeline() -> PanelPipelineSettings *
{
  auto row = mPipelineTable->selectionModel()->currentIndex().row();
  for(const auto &pipeline : mChannels) {
    if(&pipeline->mutablePipeline() == mTableModel->getCell(row)) {
      return pipeline.get();
    }
  }
  return nullptr;
}

///
/// \brief      Templates loaded from templates folder
/// \author     Joachim Danmayr
///
void PanelPipeline::loadTemplates()
{
  auto foundTemplates = joda::templates::TemplateParser::findTemplates(
      {"templates/pipelines", joda::templates::TemplateParser::getUsersTemplateDirectory().string()}, joda::fs::EXT_PIPELINE_TEMPLATE);

  mTemplatesMenu->clear();
  size_t addedPerCategory = 0;
  std::string actCategory = "basic";
  for(const auto &[category, dataInCategory] : foundTemplates) {
    for(const auto &[_, data] : dataInCategory) {
      // Now the user templates start, add an addition separator
      if(category != actCategory) {
        actCategory = category;
        if(addedPerCategory > 0) {
          mTemplatesMenu->addSeparator();
        }
      }
      if(!data.icon.isNull()) {
        auto *action = mTemplatesMenu->addAction(QIcon(data.icon.scaled(28, 28)), data.title.data());
        connect(action, &QAction::triggered, [this, path = data.path]() { onAddChannel(path.data()); });
      } else {
        auto *action = mTemplatesMenu->addAction(generateSvgIcon("favorite"), data.title.data());
        connect(action, &QAction::triggered, [this, path = data.path]() { onAddChannel(path.data()); });
      }
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
void PanelPipeline::onAddChannel(const QString &path)
{
  addChannel(path);
  mWindowMain->checkForSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addElement(std::unique_ptr<PanelPipelineSettings> baseContainer)
{
  mChannels.emplace(std::move(baseContainer));
  mTableModel->refresh();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::erase(PanelPipelineSettings *toRemove)
{
  if(toRemove != nullptr) {
    toRemove->setActive(false);
    // Find the iterator to the element using the pointer
    auto it = std::find_if(mChannels.begin(), mChannels.end(),
                           [&toRemove](const std::unique_ptr<PanelPipelineSettings> &entry) { return entry.get() == toRemove; });

    if(it != mChannels.end()) {
      void *elementInSettings = &it->get()->mutablePipeline();
      mAnalyzeSettings.pipelines.remove_if([&elementInSettings](const joda::settings::Pipeline &item) { return &item == elementInSettings; });
      mChannels.erase(it);
      mWindowMain->checkForSettingsChanged();
    }
    mTableModel->refresh();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::clear()
{
  mChannels.clear();
  mTableModel->refresh();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(joda::settings::Pipeline settings)
{
  if(settings.pipelineSetup.cStackIndex == -2) {
    auto *dialog = new DialogPipelineSettings(mWindowMain->getSettings().projectSettings.classification, settings, mWindowMain);
    if(dialog->exec() == QDialog::Rejected) {
      return;
    }
  }
  mAnalyzeSettings.pipelines.push_back(joda::settings::Pipeline{});
  auto &newlyAdded = mAnalyzeSettings.pipelines.back();
  auto panel1 = std::make_unique<PanelPipelineSettings>(mWindowMain, mWindowMain->getPreviewDock(), mWindowMain->getPreviewResultsDock(), newlyAdded,
                                                        mCommandSelectionDialog);
  panel1->fromSettings(settings);
  panel1->toSettings();
  addElement(std::move(panel1));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const QString &pathToSettings)
{
  try {
    addChannel(joda::templates::TemplateParser::loadChannelFromTemplate(std::filesystem::path(pathToSettings.toStdString())));
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const nlohmann::json &json)
{
  try {
    addChannel(joda::templates::TemplateParser::loadChannelFromTemplate(json));
  } catch(const std::exception &ex) {
    QMessageBox messageBox(this);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not load settings!");
    messageBox.setText("Could not load settings, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::moveUp()
{
  QItemSelectionModel *selectionModel = mPipelineTable->selectionModel();
  if(selectionModel->hasSelection()) {
    QModelIndex index = selectionModel->selectedRows().first();

    mPipelineTable->blockSignals(true);
    auto rowAct = index.row();
    auto newPos = rowAct - 1;
    if(newPos < 0) {
      return;
    }
    movePipelineToPosition(rowAct, newPos);
    mPipelineTable->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::moveDown()
{
  QItemSelectionModel *selectionModel = mPipelineTable->selectionModel();
  if(selectionModel->hasSelection()) {
    QModelIndex index = selectionModel->selectedRows().first();
    mPipelineTable->blockSignals(true);
    auto rowAct = index.row();
    auto newPos = rowAct + 1;
    if(newPos >= mTableModel->rowCount()) {
      return;
    }
    movePipelineToPosition(rowAct, newPos);
    mPipelineTable->blockSignals(false);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::movePipelineToPosition(size_t fromPos, size_t newPos)
{
  auto moveElementToListPosition = [](std::list<joda::settings::Pipeline> &myList, size_t oldPos, size_t newPos) {
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

  moveElementToListPosition(mAnalyzeSettings.pipelines, fromPos, newPos);
  mWindowMain->checkForSettingsChanged();
  mTableModel->refresh();
}

///
/// \brief      Save as template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::saveAsTemplate()
{
  QString folderToOpen           = joda::templates::TemplateParser::getUsersTemplateDirectory().string().data();
  QString filePathOfSettingsFile = QFileDialog::getSaveFileName(this, "Save template", folderToOpen,
                                                                "ImageC template files (*" + QString(joda::fs::EXT_PIPELINE_TEMPLATE.data()) + ")");
  if(filePathOfSettingsFile.isEmpty()) {
    return;
  }

  try {
    nlohmann::json templateJson = getSelectedPipeline()->mutablePipeline();
    joda::templates::TemplateParser::saveTemplate(templateJson, std::filesystem::path(filePathOfSettingsFile.toStdString()));
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    QMessageBox messageBox(mWindowMain);
    messageBox.setIconPixmap(generateSvgIcon("data-warning").pixmap(48, 48));
    messageBox.setWindowTitle("Could not save template!");
    messageBox.setText("Could not save template, got error >" + QString(ex.what()) + "<!");
    messageBox.addButton(tr("Okay"), QMessageBox::AcceptRole);
    auto reply = messageBox.exec();
  }
}

///
/// \brief      Save as template
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::toSettings()
{
  mAnalyzeSettings.imageSetup.zStackHandling = static_cast<joda::settings::ProjectImageSetup::ZStackHandling>(mStackHandlingZ->currentData().toInt());
  mAnalyzeSettings.imageSetup.tStackHandling = static_cast<joda::settings::ProjectImageSetup::TStackHandling>(mStackHandlingT->currentData().toInt());

  mAnalyzeSettings.imageSetup.tStackSettings.startFrame = mTStackFrameStart->text().toInt();
  mAnalyzeSettings.imageSetup.tStackSettings.endFrame   = mTStackFrameEnd->text().toInt();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::fromSettings(const joda::settings::AnalyzeSettings &settings)
{
  {
    auto idx = mStackHandlingZ->findData(static_cast<int>(settings.imageSetup.zStackHandling));
    if(idx >= 0) {
      mStackHandlingZ->setCurrentIndex(idx);
    } else {
      mStackHandlingZ->setCurrentIndex(0);
    }
  }
  {
    mTStackFrameStart->setText(QString::number(settings.imageSetup.tStackSettings.startFrame));
    mTStackFrameEnd->setText(QString::number(settings.imageSetup.tStackSettings.endFrame));
  }
  {
    auto idx = mStackHandlingT->findData(static_cast<int>(settings.imageSetup.tStackHandling));
    if(idx >= 0) {
      mStackHandlingT->setCurrentIndex(idx);
    } else {
      mStackHandlingT->setCurrentIndex(0);
    }
  }
}

}    // namespace joda::ui::gui
