///
/// \file      dialog_command_selection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "dialog_command_selection.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <memory>
#include <string>
#include "backend/commands/classification/hough_transform/hough_transform_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings_ui.hpp"
#include "backend/commands/image_functions/enhance_contrast/enhance_contrast_settings.hpp"
#include "backend/commands/image_functions/fill_holes/fill_holes_settings.hpp"
#include "backend/commands/image_functions/image_math/image_math_settings.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation_settings.hpp"
#include "backend/commands/image_functions/rank_filter/rank_filter_settings.hpp"
#include "backend/commands/image_functions/skeletonize/skeletonize_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/threshold_adaptive/threshold_adaptive_settings.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/commands/object_functions/measure_distance/measure_distance_settings.hpp"
#include "backend/commands/object_functions/object_transform/object_transform_settings.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/editor/widget_pipeline/dialog_pipeline_settings/dialog_pipeline_settings.hpp"
#include "ui/gui/editor/widget_pipeline/panel_pipeline_settings.hpp"
#include "ui/gui/editor/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogCommandSelection::DialogCommandSelection(WindowMain *parent) : QDialog(parent), mParent(parent)
{
  mSearch = new QLineEdit();
  mSearch->setPlaceholderText("Search...");
  connect(mSearch, &QLineEdit::textChanged, [this]() { filterCommands({mSearch->text(), mOutOfStepBefore}); });

  mCommands = new QTableWidget();
  mCommands->setColumnCount(3);
  mCommands->setColumnHidden(0, true);
  mCommands->setHorizontalHeaderLabels({"", "", "Command"});
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(false);
  mCommands->horizontalHeader()->setVisible(false);
  mCommands->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  mCommands->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  mCommands->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
  mCommands->setColumnWidth(1, 26);
  mCommands->setShowGrid(false);
  mCommands->setStyleSheet("QTableView::item { border-top: 0px solid black; border-bottom: 1px solid gray; }");
  mCommands->verticalHeader()->setMinimumSectionSize(36);
  mCommands->verticalHeader()->setDefaultSectionSize(36);
  mCommands->installEventFilter(this);

  // mCommands.setsh

  connect(mCommands, &QTableWidget::cellDoubleClicked, [&](int row, int /*column*/) {
    auto idx = mCommands->item(row, 0)->text().toInt();
    addNewCommand(idx);
    close();
  });

  //
  auto *layout = new QVBoxLayout();
  layout->addWidget(mSearch);
  layout->addWidget(mCommands);
  setLayout(layout);
  setMinimumHeight(600);
  setMinimumWidth(500);

  /// \todo Put this to a thread
  addCommandsToTable();
  // std::thread([this]() { addCommandsToTable(); }).detach();
}

///
/// \brief      Start search when typing within the table
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool DialogCommandSelection::eventFilter(QObject *obj, QEvent *event)
{
  if(obj == mCommands && event->type() == QEvent::KeyPress) {
    auto *keyEvent = static_cast<QKeyEvent *>(event);
    mSearch->setText(keyEvent->text());
    mSearch->setFocus();
  }
  // Standard event processing
  return QObject::eventFilter(obj, event);
}

void DialogCommandSelection::show(const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore, joda::settings::Pipeline *settings,
                                  PanelPipelineSettings *pipelineStepSettingsUi)
{
  mSettings               = settings;
  mPipelineStepSettingsUi = pipelineStepSettingsUi;
  mOutOfStepBefore        = outOfStepBefore;
  mPipelineStepBefore     = pipelineStepBefore;
  filterCommands({mSearch->text(), outOfStepBefore});
  exec();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::unique_ptr<joda::ui::gui::Command> DialogCommandSelection::generateCommand(const settings::PipelineStep &step)
{
  return joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(step, mParent);
}

void assignLabels()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogCommandSelection::addCommandsToTable()
{
  mCommands->setRowCount(0);
  {
    addTitleToTable("Image Processing", Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$colorFilter = settings::ColorFilterSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$crop = settings::MarginCropSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$blur = settings::BlurSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$intensityTransform = settings::IntensityTransformationSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$rollingBall = settings::RollingBallSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$medianSubtract = settings::MedianSubtractSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$sobel = settings::EdgeDetectionSobelSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$canny = settings::EdgeDetectionCannySettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$imageMath = settings::ImageMathSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$imageToCache = settings::ImageCacheSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$enhanceContrast = settings::EnhanceContrastSettings{}}, Group::IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$rank = settings::RankFilterSettings{}}, Group::IMAGE_PROCESSING);
  }

  {
    addTitleToTable("Binary image Processing", Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$threshold = settings::ThresholdSettings{}}, Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$thresholdAdaptive = settings::ThresholdAdaptiveSettings{}}, Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$watershed = settings::WatershedSettings{}}, Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$morphologicalTransform = settings::MorphologicalTransformSettings{}}, Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$fillHoles = settings::FillHolesSettings{}}, Group::BINARY_IMAGE_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$skeletonize = settings::SkeletonizeSettings{}}, Group::BINARY_IMAGE_PROCESSING);
  }

  {
    settings::ClassifierSettings defaultClassify;
    defaultClassify.modelClasses = {{.pixelClassId = 1}};
    addTitleToTable("Classification", Group::CLASSIFICATION);
    addCommandToTable(settings::PipelineStep{.$classify = defaultClassify}, Group::CLASSIFICATION);
    addCommandToTable(settings::PipelineStep{.$aiClassify = settings::AiClassifierSettings{}}, Group::CLASSIFICATION);
    addCommandToTable(settings::PipelineStep{.$houghTransform = settings::HoughTransformSettings{}}, Group::CLASSIFICATION);
  }

  {
    addTitleToTable("Object Processing", Group::OBJECT_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$voronoi = settings::VoronoiGridSettings{}}, Group::OBJECT_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$reclassify = settings::ReclassifySettings{}}, Group::OBJECT_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$objectsToImage = settings::ObjectsToImageSettings{}}, Group::OBJECT_PROCESSING);
    addCommandToTable(settings::PipelineStep{.$objectTransform = settings::ObjectTransformSettings{}}, Group::OBJECT_PROCESSING);
  }

  {
    addTitleToTable("Measurement", Group::MEASUREMENT);
    addCommandToTable(settings::PipelineStep{.$colocalization = settings::ColocalizationSettings{}}, Group::MEASUREMENT);
    addCommandToTable(settings::PipelineStep{.$measureIntensity = settings::MeasureIntensitySettings{}}, Group::MEASUREMENT);
    addCommandToTable(settings::PipelineStep{.$measureDistance = settings::MeasureDistanceSettings{}}, Group::MEASUREMENT);
  }

  {
    addTitleToTable("Filtering", Group::FILTERING);
    addCommandToTable(settings::PipelineStep{.$thresholdValidator = settings::ThresholdValidatorSettings{}}, Group::FILTERING);
    addCommandToTable(settings::PipelineStep{.$noiseValidator = settings::NoiseValidatorSettings{}}, Group::FILTERING);
  }

  {
    addTitleToTable("Output", Group::OUTPUT);
    addCommandToTable(settings::PipelineStep{.$saveImage = settings::ImageSaverSettings{}}, Group::OUTPUT);
    addCommandToTable(settings::PipelineStep{.$imageFromClass = settings::ImageFromClassSettings{}}, Group::OUTPUT);
  }
}

void DialogCommandSelection::addTitleToTable(const std::string &title, Group group)
{
  int newRow = mCommands->rowCount();
  if(newRow < 0) {
    newRow = 0;
  }
  mTitleINdex.emplace(group, newRow);
  mCommands->insertRow(newRow);
  auto *iconItem = new QTableWidgetItem();
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsSelectable);
  QFont font;
  font.setBold(true);
  iconItem->setFont(font);    // Set the font to Arial, 12 points, and bold

  // iconItem->setIcon(cmd->getIcon());
  iconItem->setText(title.data());
  iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
  mCommands->setItem(newRow, 1, iconItem);

  auto *vectorIndex = new QTableWidgetItem();
  vectorIndex->setText("");
  mCommands->setItem(newRow, 0, vectorIndex);

  mCommands->setSpan(newRow, 1, 1, 2);    // Starts at (row 0, column 0) and spans 1 row and 2 columns
}

int DialogCommandSelection::addCommandToTable(const settings::PipelineStep &step, Group group)
{
  std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(step, nullptr);
  if(cmd != nullptr) {
    mCommandList.emplace_back(
        CommandListEntry{step, cmd->getInOut(), cmd->getTitle().toLower(), cmd->getDescription().toLower(), group, cmd->getTags()});
    int newRow = mCommands->rowCount();
    mCommandIndexMap.emplace(mCommandList.size() - 1, newRow);
    mCommands->insertRow(newRow);

    QString text = cmd->getTitle() + "<br/><span style='color:gray;'><i>" + cmd->getDescription() + "</i></span>";

    // Set the icon in the first column
    auto *textIcon = new QLabel();
    textIcon->setText(text);
    textIcon->setTextFormat(Qt::RichText);
    mCommands->setCellWidget(newRow, 2, textIcon);

    auto *iconItem = new QTableWidgetItem();
    iconItem->setIcon(cmd->getIcon());
    iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
    mCommands->setItem(newRow, 1, iconItem);

    auto *vectorIndex = new QTableWidgetItem();
    vectorIndex->setText(std::to_string(mCommandList.size() - 1).data());
    mCommands->setItem(newRow, 0, vectorIndex);
    return 1;
  }
  cmd.reset();
  return 0;
}

void DialogCommandSelection::filterCommands(const CommandTableFilter &filter)
{
  auto searchTexts = filter.searchText.toLower();
  std::set<Group> groups;
  for(size_t n = 0; n < mCommandList.size(); n++) {
    const auto &command = mCommandList.at(n);
    int32_t tableIndex  = mCommandIndexMap.at(n);
    auto filterInOut    = filter.outOfStepBefore;
    if(filter.outOfStepBefore != InOuts::ALL && !command.inOuts.in.contains(filterInOut)) {
      mCommands->setRowHidden(tableIndex, true);
    } else if(command.name.contains(searchTexts) || command.description.contains(searchTexts)) {
      // Enable
      groups.emplace(command.group);
      mCommands->setRowHidden(tableIndex, false);
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
      mCommands->setRowHidden(tableIndex, !found);
    }
  }

  for(int32_t n = 0; n < static_cast<int32_t>(Group::LAST_GROUP); n++) {
    int32_t tableIndex = mTitleINdex.at(static_cast<Group>(n));
    if(groups.contains(static_cast<Group>(n))) {
      mCommands->setRowHidden(tableIndex, false);
    } else {
      mCommands->setRowHidden(tableIndex, true);
    }
  }
}

void DialogCommandSelection::addNewCommand(size_t commandListIdx)
{
  if(mPipelineStepBefore == nullptr && mSettings != nullptr) {
    auto inserted = mSettings->pipelineSteps.insert(mSettings->pipelineSteps.begin(), mCommandList[commandListIdx].pipelineStep);
    std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(*inserted, mParent);
    mPipelineStepSettingsUi->insertNewPipelineStep(0, std::move(cmd), &*inserted);
    return;
  }
  if(mSettings != nullptr) {
    int posInserted = 0;
    for(auto it = mSettings->pipelineSteps.begin(); it != mSettings->pipelineSteps.end(); ++it) {
      posInserted++;
      if(&*it == mPipelineStepBefore) {
        const auto &inserted                        = mSettings->pipelineSteps.insert(std::next(it), mCommandList[commandListIdx].pipelineStep);
        std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(*inserted, mParent);
        mPipelineStepSettingsUi->insertNewPipelineStep(posInserted, std::move(cmd), &*inserted);
        return;
      }
    }
  }
}

}    // namespace joda::ui::gui
