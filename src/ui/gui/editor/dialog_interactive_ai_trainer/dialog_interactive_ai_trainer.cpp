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
#include "backend/commands/classification/random_forest/random_forest.hpp"
#include "backend/commands/classification/random_forest/random_forest_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
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
DialogInteractiveAiTrainer::DialogInteractiveAiTrainer(const joda::settings::Classification &classes, joda::settings::Pipeline &settings,
                                                       joda::ctrl::Preview *objects, WindowMain *parent) :
    QDialog(parent),
    mPreviewResult(objects)
{
  setWindowTitle("Pixel classifier (alpha)");
  setMinimumSize(300, 400);
  auto *formLayout = new QFormLayout;

  defaultClassId = new QComboBox();
  formLayout->addRow("Class to train from", defaultClassId);

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
  joda::settings::RandomForestSettings settings;
  joda::cmd::RandomForest randForest(settings);

  cv::Mat featList;
  cv::Mat labelList;
  randForest.prepareTrainingDataFromROI(*mPreviewResult->originalImage.getOriginalImage(), mPreviewResult->objectMap.at(enums::ClassId::C0), featList,
                                        labelList);

  // auto ret = randForest.trainRandomForest(featList, labelList);
  // ret->save("tmp/mymodel.xml");
}

}    // namespace joda::ui::gui
