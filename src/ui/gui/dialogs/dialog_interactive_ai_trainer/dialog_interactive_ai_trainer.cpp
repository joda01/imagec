///
/// \file      dialog_interactive_ai_trainer.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_interactive_ai_trainer.hpp"
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <filesystem>
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/commands/classification/pixel_classifier/random_forest/random_forest.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ml_model_parser/ml_model_parser.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/iconless_dialog_button_box.hpp"
#include "ui/gui/helper/setting_generator.hpp"

namespace joda::ui::gui {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
DialogInteractiveAiTrainer::DialogInteractiveAiTrainer(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("Pixel classifier (alpha)");
  setMinimumSize(300, 400);
  auto *formLayout = new QFormLayout;

  auto *btnStartTraining = new QPushButton("Start training");
  connect(btnStartTraining, &QPushButton::pressed, [this]() { startTraining(); });

  formLayout->addRow(btnStartTraining);

  // Okay and canlce
  auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  formLayout->addWidget(buttonBox);

  setLayout(formLayout);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogInteractiveAiTrainer::startTraining()
{
  atom::ObjectList objectList;
  mImagePanel->getObjectMapFromAnnotatedRegions(objectList);
  std::set<int32_t> classesToTrain;

  for(const auto &[classId, _] : objectList) {
    classesToTrain.emplace(static_cast<int32_t>(classId));
  }

  if(classesToTrain.size() > 1) {
    std::filesystem::path modelPath = joda::ml::MlModelParser::getUsersMlModelDirectory() / ("tmp" + joda::fs::MASCHINE_LEARNING_OPCEN_CV_XML_MODEL);

    joda::settings::PixelClassifierTrainingSettings settings{
        .trainingClasses = classesToTrain, .method = joda::settings::PixelClassifierMethod::RANDOM_FOREST, .outPath = modelPath};
    joda::cmd::PixelClassifier::train(*mImagePanel->mutableImage()->getOriginalImage(), objectList, settings);
  }
}

}    // namespace joda::ui::gui
