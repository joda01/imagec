///
/// \file      dialog_command_selection.cpp
/// \author    Joachim Danmayr
/// \date      2024-08-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#include "dialog_command_selection.hpp"
#include <qboxlayout.h>
#include <qdialog.h>
#include <qtablewidget.h>
#include <memory>
#include <string>
#include "backend/commands/image_functions/blur/blur_settings.hpp"
#include "backend/commands/image_functions/blur/blur_settings_ui.hpp"
#include "backend/commands/image_functions/image_math/image_math_settings.hpp"
#include "backend/commands/image_functions/image_saver/image_saver_settings.hpp"
#include "backend/commands/image_functions/morphological_transformation/morphological_transformation_settings.hpp"
#include "backend/commands/image_functions/threshold/threshold_settings.hpp"
#include "backend/commands/image_functions/watershed/watershed_settings.hpp"
#include "backend/commands/object_functions/object_transform/object_transform_settings.hpp"
#include "backend/commands/object_functions/validator_threshold/validator_threshold_settings.hpp"
#include "backend/commands/object_functions/voronoi_grid/voronoi_grid_settings.hpp"
#include "backend/settings/pipeline/pipeline_factory.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "ui/gui/window_main/window_main.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogCommandSelection::DialogCommandSelection(joda::settings::Pipeline &settings, PanelPipelineSettings *pipelineStepSettingsUi,
                                               const settings::PipelineStep *pipelineStepBefore, InOuts outOfStepBefore, WindowMain *parent) :
    QDialog(parent),
    mParent(parent), mPipelineStepBefore(pipelineStepBefore), mSettings(settings), pipelineStepSettingsUi(pipelineStepSettingsUi)
{
  auto *layout = new QVBoxLayout();

  mCommands = new QTableWidget();
  mCommands->setColumnCount(2);
  mCommands->setColumnHidden(0, true);
  mCommands->setHorizontalHeaderLabels({"", "Command"});
  mCommands->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mCommands->verticalHeader()->setVisible(false);

  connect(mCommands, &QTableWidget::cellDoubleClicked, [&](int row, int column) {
    // Open results
    // mWindowMain->showPanelResults();
    // mWindowMain->getPanelResults()->openFromFile(mLastLoadedResults->item(row, 0)->text());
    auto idx = mCommands->item(row, 0)->text().toInt();
    addNewCommand(idx);
    close();
  });

  layout->addWidget(mCommands);
  addCommandsToTable(outOfStepBefore);
  setLayout(layout);
  setMinimumHeight(600);
  setMinimumWidth(500);
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
  return std::move(joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(step, mParent));
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogCommandSelection::addCommandsToTable(InOuts outOfStepBefore)
{
  mCommands->setRowCount(0);
  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$colorFilter = settings::ColorFilterSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$crop = settings::MarginCropSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$blur = settings::BlurSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$intensityTransform = settings::IntensityTransformationSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$rollingBall = settings::RollingBallSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$medianSubtract = settings::MedianSubtractSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$edgeDetection = settings::EdgeDetectionSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$imageMath = settings::ImageMathSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$imageToCache = settings::ImageCacheSettings{}}, outOfStepBefore);

    if(inserted > 0) {
      // Only insert title if at least one element has been added
      addTitleToTable("Image Processing", inserted);
    }
  }

  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$threshold = settings::ThresholdSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$watershed = settings::WatershedSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$morphologicalTransform = settings::MorphologicalTransformSettings{}}, outOfStepBefore);

    if(inserted > 0) {
      addTitleToTable("Binary image Processing", inserted);
    }
  }

  {
    settings::ClassifierSettings defaultClassify;
    defaultClassify.modelClasses = {{.modelClassId = 65535}};
    int inserted                 = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$classify = defaultClassify}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$aiClassify = settings::AiClassifierSettings{}}, outOfStepBefore);
    if(inserted > 0) {
      addTitleToTable("Classification", inserted);
    }
  }

  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$voronoi = settings::VoronoiGridSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$reclassify = settings::ReclassifySettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$objectsToImage = settings::ObjectsToImageSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$objectTransform = settings::ObjectTransformSettings{}}, outOfStepBefore);
    if(inserted > 0) {
      addTitleToTable("Object Processing", inserted);
    }
  }

  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$colocalization = settings::ColocalizationSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$measure = settings::MeasureSettings{}}, outOfStepBefore);
    if(inserted > 0) {
      addTitleToTable("Measurement", inserted);
    }
  }

  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$thresholdValidator = settings::ThresholdValidatorSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$noiseValidator = settings::NoiseValidatorSettings{}}, outOfStepBefore);
    if(inserted > 0) {
      addTitleToTable("Filtering", inserted);
    }
  }

  {
    int inserted = 0;
    inserted += addCommandToTable(settings::PipelineStep{.$saveImage = settings::ImageSaverSettings{}}, outOfStepBefore);
    inserted += addCommandToTable(settings::PipelineStep{.$imageFromClass = settings::ImageFromClassSettings{}}, outOfStepBefore);
    if(inserted > 0) {
      addTitleToTable("Output", inserted);
    }
  }
}

void DialogCommandSelection::addTitleToTable(const std::string &title, int position)
{
  int newRow = mCommands->rowCount() - position;
  if(newRow < 0) {
    newRow = 0;
  }
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
}

int DialogCommandSelection::addCommandToTable(const settings::PipelineStep &step, InOuts outOfStepBefore)
{
  std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(step, nullptr);
  if(cmd != nullptr) {
    // Add only commands which are allowed to add
    if(outOfStepBefore != InOuts::ALL && !cmd->getInOut().in.contains(outOfStepBefore)) {
      return 0;
    }
    mCommandList.emplace_back(step);
    int newRow = mCommands->rowCount();
    mCommands->insertRow(newRow);
    // Set the icon in the first column
    auto *iconItem = new QTableWidgetItem();
    iconItem->setIcon(cmd->getIcon());
    iconItem->setText(cmd->getTitle());
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

void DialogCommandSelection::addNewCommand(int commandListIdx)
{
  if(mPipelineStepBefore == nullptr) {
    auto inserted                               = mSettings.pipelineSteps.insert(mSettings.pipelineSteps.begin(), mCommandList[commandListIdx]);
    std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(*inserted, mParent);
    pipelineStepSettingsUi->insertNewPipelineStep(0, std::move(cmd), &*inserted);
    return;
  }

  int posInserted = 0;
  for(auto it = mSettings.pipelineSteps.begin(); it != mSettings.pipelineSteps.end(); ++it) {
    posInserted++;
    if(&*it == mPipelineStepBefore) {
      const auto &inserted                        = mSettings.pipelineSteps.insert(std::next(it), mCommandList[commandListIdx]);
      std::unique_ptr<joda::ui::gui::Command> cmd = joda::settings::PipelineFactory<joda::ui::gui::Command>::generate(*inserted, mParent);
      pipelineStepSettingsUi->insertNewPipelineStep(posInserted, std::move(cmd), &*inserted);
      return;
    }
  }
}

void DialogCommandSelection::setInOutBefore(InOuts inout)
{
  addCommandsToTable(inout);
}

}    // namespace joda::ui::gui
