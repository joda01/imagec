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
#include "ui/gui/container/pipeline/panel_channel_overview.hpp"
#include "ui/gui/container/pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/helper/droppable_widget/droppable_widget.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/pipeline_overview_delegate.hpp"
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
  auto *layout = new QVBoxLayout();

  {
    auto *toolbar = new QToolBar();

    //
    // New pipeline
    //
    auto *newPipeline = new QAction(generateSvgIcon("list-add"), "Add new pipeline");
    newPipeline->setStatusTip("Add new pipeline or use predefined template");
    connect(newPipeline, &QAction::triggered, [this]() { addChannel(joda::settings::Pipeline{}); });
    mTemplatesMenu = new QMenu();
    newPipeline->setMenu(mTemplatesMenu);
    toolbar->addAction(newPipeline);

    //
    // Open template
    //
    auto *openTemplate = new QAction(generateSvgIcon("folder-open"), "Open template");
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

    toolbar->addSeparator();
    //
    // Start button
    //
    mActionStart = new QAction(generateSvgIcon("media-playback-start"), "Start analyze");
    mActionStart->setStatusTip("Start analyze");
    mActionStart->setEnabled(false);
    connect(mActionStart, &QAction::triggered, windowMain, &WindowMain::onStartClicked);
    toolbar->addAction(mActionStart);

    toolbar->addSeparator();
    //
    // Move down
    //
    auto *moveDown = new QAction(generateSvgIcon("go-down"), "Move down");
    moveDown->setStatusTip("Move selected pipeline down");
    connect(moveDown, &QAction::triggered, this, &PanelPipeline::moveDown);
    toolbar->addAction(moveDown);

    //
    // Move up
    //
    auto *moveUp = new QAction(generateSvgIcon("go-up"), "Move up");
    moveUp->setStatusTip("Move selected pipeline up");
    connect(moveUp, &QAction::triggered, this, &PanelPipeline::moveUp);
    toolbar->addAction(moveUp);

    layout->addWidget(toolbar);
  }

  // Create a widget to hold the panels
  {
    mPipelineTable = new PlaceholderTableWidget(0, 1);
    mPipelineTable->setPlaceholderText("Press the + button to add a pipeline.");
    mPipelineTable->verticalHeader()->setVisible(false);
    mPipelineTable->horizontalHeader()->setVisible(false);
    mPipelineTable->setHorizontalHeaderLabels({"Pipeline"});
    mPipelineTable->setAlternatingRowColors(true);
    mPipelineTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPipelineTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // auto *delegate = new PipelineOverviewDelegate(mPipelineTable);
    // mPipelineTable->setItemDelegateForColumn(0, delegate);    // Set the delegate for the desired column
  }

  mCommandSelectionDialog = std::make_shared<DialogCommandSelection>(mWindowMain);

  layout->addWidget(mPipelineTable);

  setLayout(layout);
}

void PanelPipeline::setActionStartEnabled(bool enabled)
{
  mActionStart->setEnabled(enabled);
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
void PanelPipeline::addElement(std::unique_ptr<PanelPipelineSettings> baseContainer, void *pointerToSettings)
{
  auto newRow = mPipelineTable->rowCount();
  mPipelineTable->insertRow(newRow);
  mPipelineTable->setCellWidget(newRow, 0, baseContainer->getOverviewPanel());
  // mContentWidget->getLayout()->addWidget(baseContainer->getOverviewPanel());
  mChannels.emplace(std::move(baseContainer), pointerToSettings);
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
    auto it = std::find_if(mChannels.begin(), mChannels.end(), [&toRemove](std::pair<const std::unique_ptr<PanelPipelineSettings>, void *> &entry) {
      return entry.first.get() == toRemove;
    });

    if(it != mChannels.end()) {
      void *elementInSettings = it->second;

      mAnalyzeSettings.pipelines.remove_if([&elementInSettings](const joda::settings::Pipeline &item) { return &item == elementInSettings; });

      int32_t row = 0;
      for(; row < mPipelineTable->rowCount(); row++) {
        if(mPipelineTable->cellWidget(row, 0) == toRemove->getOverviewPanel()) {
          break;
        }
      }

      mPipelineTable->removeRow(row);
      toRemove->getOverviewPanel()->setParent(nullptr);
      mChannels.erase(it);
      mWindowMain->checkForSettingsChanged();
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
void PanelPipeline::clear()
{
  mPipelineTable->setRowCount(0);
  mChannels.clear();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::addChannel(const joda::settings::Pipeline &settings)
{
  mAnalyzeSettings.pipelines.push_back(joda::settings::Pipeline{});
  auto &newlyAdded = mAnalyzeSettings.pipelines.back();
  auto panel1      = std::make_unique<PanelPipelineSettings>(mWindowMain, mWindowMain->getPreviewDock(), newlyAdded, mCommandSelectionDialog);
  panel1->fromSettings(settings);
  panel1->toSettings();
  addElement(std::move(panel1), &newlyAdded);
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
  auto rowAct  = mPipelineTable->currentRow();
  auto *widget = mPipelineTable->cellWidget(rowAct, 0);
  auto newPos  = rowAct - 1;
  if(newPos < 0) {
    return;
  }
  movePipelineToPosition(widget, rowAct, newPos);
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
  auto rowAct  = mPipelineTable->currentRow();
  auto *widget = mPipelineTable->cellWidget(rowAct, 0);
  auto newPos  = rowAct + 1;
  if(newPos >= mPipelineTable->rowCount()) {
    return;
  }
  movePipelineToPosition(widget, rowAct, newPos);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void PanelPipeline::movePipelineToPosition(const QWidget *toMove, size_t fromPos, size_t newPos)
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

  auto moveRow = [&](int fromRow, int toRow) {
    if(fromRow == toRow || fromRow < 0 || toRow < 0 || fromRow >= mPipelineTable->rowCount() || toRow > mPipelineTable->rowCount()) {
      return;    // invalid input
    }

    // Save items from the source row
    QWidget *fromWidget = mPipelineTable->cellWidget(fromRow, 0);
    QWidget *toWidget   = mPipelineTable->cellWidget(toRow, 0);
    if(toRow < fromRow) {
      mPipelineTable->insertRow(toRow);
      mPipelineTable->setCellWidget(toRow, 0, fromWidget);
      mPipelineTable->setCellWidget(fromRow, 0, toWidget);
      mPipelineTable->removeRow(fromRow + 1);
    } else {
      mPipelineTable->insertRow(toRow + 1);
      mPipelineTable->setCellWidget(toRow + 1, 0, fromWidget);
      mPipelineTable->setCellWidget(toRow, 0, toWidget);
      mPipelineTable->removeRow(fromRow);
    }
    mPipelineTable->selectRow(toRow);
  };

  moveElementToListPosition(mAnalyzeSettings.pipelines, fromPos, newPos);
  moveRow(fromPos, newPos);

  mWindowMain->checkForSettingsChanged();
}

}    // namespace joda::ui::gui
