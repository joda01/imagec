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
#include <qboxlayout.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <filesystem>
#include "backend/artifacts/roi/roi.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_settings.hpp"
#include "backend/commands/classification/pixel_classifier/pixel_classifier_training_settings.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include "backend/helper/ml_model_parser/ml_model_parser.hpp"
#include "backend/settings/project_settings/project_classification.hpp"
#include "ui/gui/dialogs/dialog_image_view/panel_image_view.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/table_item_delegate_polygon.hpp"
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
DialogRoiManager::DialogRoiManager(const std::shared_ptr<atom::ObjectList> &objectMap, const joda::settings::Classification *classSettings,
                                   PanelImageView *imagePanel, QWidget *parent) :
    QWidget(parent),
    mImagePanel(imagePanel)
{
  setWindowTitle("ROI manager");
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  {
    auto *toolbar = new QToolBar();
    toolbar->setObjectName("SubToolBar");
    toolbar->setIconSize(QSize(16, 16));

    // toolbar->addSeparator();

    //
    // Open template
    //
    auto *openTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("folder-open"), "Open object class template");
    openTemplate->setStatusTip("Open object class template");
    connect(openTemplate, &QAction::triggered, [this]() {

    });
    // toolbar->addAction(openTemplate);

    //
    // Save as template
    //
    auto *saveAsTemplate = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("floppy-disk"), "Save classification settings as template");
    saveAsTemplate->setStatusTip("Save classification settings as template");
    connect(saveAsTemplate, &QAction::triggered, [this]() {});
    // toolbar->addAction(saveAsTemplate);

    toolbar->addSeparator();

    //
    // Delete column
    //
    auto *deleteColumn = new QAction(generateSvgIcon<Style::REGULAR, Color::GRAY>("trash-simple"), "Delete selected class", this);
    deleteColumn->setStatusTip("Delete selected ROIs");
    toolbar->addAction(deleteColumn);
    connect(deleteColumn, &QAction::triggered, [this]() { mImagePanel->deleteSelectedRois(); });

    layout->addWidget(toolbar);
  }

  {
    mPolygonsTable = new PlaceholderTableView(this);
    mPolygonsTable->setPlaceholderText("Press the + button to add a new pipeline.");
    mPolygonsTable->setFrameStyle(QFrame::NoFrame);
    mPolygonsTable->verticalHeader()->setVisible(false);
    mPolygonsTable->horizontalHeader()->setVisible(true);
    mPolygonsTable->setAlternatingRowColors(true);
    mPolygonsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPolygonsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mPolygonsTable->setItemDelegateForColumn(0, new ItemDelegatePolygon(mPolygonsTable));
    mTableModel = new TableModelPaintedPolygon(classSettings, objectMap, mPolygonsTable);
    mPolygonsTable->setModel(mTableModel);
    layout->addWidget(mPolygonsTable);

    connect(mPolygonsTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &, const QItemSelection &) {
      if(!mPolygonsTable->selectionModel()->hasSelection()) {
        mImagePanel->setSelectedRois({});
      } else {
        auto indexes = mPolygonsTable->selectionModel()->selectedIndexes();
        std::set<atom::ROI *> idxs;
        for(const auto row : indexes) {
          idxs.emplace(mTableModel->getCell(row.row()));
        }
        mImagePanel->setSelectedRois(idxs);
      }
    });
    // connect(mPolygonsTable, &QTableView::doubleClicked, [this](const QModelIndex &index) {});
  }

  setLayout(layout);

  connect(imagePanel, &PanelImageView::paintedPolygonsChanged, [this]() { mTableModel->refresh(); });
  connect(imagePanel, &PanelImageView::paintedPolygonClicked, [this](std::set<atom::ROI *> idxs) {
    bool firstRun = true;
    for(const auto &idx : idxs) {
      QModelIndex index = mPolygonsTable->model()->index(mTableModel->indexFor(idx), 0);    // pick column 0 for the row
      mPolygonsTable->selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
      if(firstRun) {
        firstRun = false;
        mPolygonsTable->setCurrentIndex(index);
      }
    }
  });
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogRoiManager::hideEvent(QHideEvent *event)
{
  // std::cout << "Hide" << std::endl;
  // mMoveAction->setChecked(true);
  // mImagePanel->setState(PanelImageView::State::MOVE);
  // emit dialogDisappeared();
  QWidget::hideEvent(event);
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void DialogRoiManager::closeEvent(QCloseEvent *event)
{
  mImagePanel->setState(PanelImageView::State::MOVE);

  emit dialogDisappeared();
  QWidget::closeEvent(event);
}

}    // namespace joda::ui::gui
