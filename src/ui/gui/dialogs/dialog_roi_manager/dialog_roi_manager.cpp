///
/// \file      dialog_roi_manager.cpp
/// \author    Joachim Danmayr
/// \date      2025-08-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_roi_manager.hpp"
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
#include "ui/gui/dialogs/dialog_roi_manager/table_model_painted_polygon.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/window_main.hpp"
#include "ui/gui/helper/html_delegate.hpp"
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
DialogRoiManager::DialogRoiManager(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("Pixel classifier (alpha)");
  setMinimumSize(300, 400);
  auto *formLayout = new QFormLayout;

  auto *btnStartTraining = new QPushButton("Start training");
  connect(btnStartTraining, &QPushButton::pressed, [this]() { startTraining(); });

  formLayout->addRow(btnStartTraining);

  {
    mPolygonsTable = new PlaceholderTableView(this);
    mPolygonsTable->setPlaceholderText("Press the + button to add a new pipeline.");
    mPolygonsTable->setFrameStyle(QFrame::NoFrame);
    mPolygonsTable->verticalHeader()->setVisible(false);
    mPolygonsTable->horizontalHeader()->setVisible(true);
    mPolygonsTable->setAlternatingRowColors(true);
    mPolygonsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPolygonsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTableModel = new TableModelPaintedPolygon(mPolygonsTable);
    mTableModel->setData(imagePanel->getPtrToPolygons());
    mPolygonsTable->setModel(mTableModel);
    formLayout->addRow(mPolygonsTable);

    connect(mPolygonsTable->selectionModel(), &QItemSelectionModel::currentChanged, [&](const QModelIndex &current, const QModelIndex &previous) {});
    connect(mPolygonsTable, &QTableView::clicked, [this](const QModelIndex &index) {});
    connect(mPolygonsTable, &QTableView::doubleClicked, [this](const QModelIndex &index) {});
  }

  // Okay and cancel
  auto *buttonBox = new IconlessDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  formLayout->addWidget(buttonBox);

  setLayout(formLayout);

  connect(imagePanel, &PanelImageView::paintedPolygonsChanged, [this]() { mTableModel->refresh(); });
  connect(imagePanel, &PanelImageView::paintedPolygonClicked, [this](int32_t selectedIndex) {
    QModelIndex index = mPolygonsTable->model()->index(selectedIndex, 0);    // pick column 0 for the row
    mPolygonsTable->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    mPolygonsTable->setCurrentIndex(index);
  });
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogRoiManager::startTraining()
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
