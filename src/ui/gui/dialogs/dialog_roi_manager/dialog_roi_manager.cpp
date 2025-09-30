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
DialogRoiManager::DialogRoiManager(PanelImageView *imagePanel, QWidget *parent) : QDialog(parent), mImagePanel(imagePanel)
{
  setWindowTitle("ROI manager");
  setMinimumSize(300, 400);
  auto *layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  {
    auto *toolbar = new QToolBar();
    toolbar->setObjectName("SubToolBar");
    toolbar->setIconSize(QSize(16, 16));

    auto *paintingToolActionGroup = new QActionGroup(toolbar);

    mMoveAction = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("hand"), "Move");
    mMoveAction->setStatusTip("Move");
    mMoveAction->setCheckable(true);
    mMoveAction->setChecked(true);
    paintingToolActionGroup->addAction(mMoveAction);
    toolbar->addAction(mMoveAction);
    connect(mMoveAction, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::MOVE);
      }
    });

    mSelectAction = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("cursor-click"), "Select");
    mSelectAction->setStatusTip("Move");
    mSelectAction->setCheckable(true);
    paintingToolActionGroup->addAction(mSelectAction);
    toolbar->addAction(mSelectAction);
    connect(mSelectAction, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::SELECT);
      }
    });

    toolbar->addSeparator();

    auto *paintRectangle = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("rectangle"), "Rectangle");
    paintRectangle->setStatusTip("Paint rectangle");
    paintRectangle->setCheckable(true);
    paintingToolActionGroup->addAction(paintRectangle);
    toolbar->addAction(paintRectangle);
    connect(paintRectangle, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::PAINT_RECTANGLE);
      }
    });

    auto *paintCircle = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("circle"), "Circle");
    paintCircle->setStatusTip("Paint circle");
    paintCircle->setCheckable(true);
    paintingToolActionGroup->addAction(paintCircle);
    toolbar->addAction(paintCircle);
    connect(paintCircle, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::PAINT_OVAL);
      }
    });

    auto *paintPolygon = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("polygon"), "Polygon");
    paintPolygon->setStatusTip("Paint polygon");
    paintPolygon->setCheckable(true);
    paintingToolActionGroup->addAction(paintPolygon);
    toolbar->addAction(paintPolygon);
    connect(paintPolygon, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::PAINT_POLYGON);
      }
    });

    auto *paintBrush = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("paint-brush"), "Brush");
    paintBrush->setStatusTip("Paint brush");
    paintBrush->setCheckable(true);
    paintingToolActionGroup->addAction(paintBrush);
    // toolbar->addAction(paintBrush);
    connect(paintBrush, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::PAIN_BRUSH);
      }
    });

    auto *magicWand = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("magic-wand"), "MAgic wand tool");
    magicWand->setStatusTip("Paint brush");
    magicWand->setCheckable(true);
    paintingToolActionGroup->addAction(magicWand);
    // toolbar->addAction(magicWand);
    connect(magicWand, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImagePanel->setState(PanelImageView::State::PAINT_MAGIC_WAND);
      }
    });

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
    mTableModel = new TableModelPaintedPolygon(mPolygonsTable);
    mTableModel->setData(imagePanel->getPtrToPolygons());
    mPolygonsTable->setModel(mTableModel);
    layout->addWidget(mPolygonsTable);

    connect(mPolygonsTable->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &, const QItemSelection &) {
      if(!mPolygonsTable->selectionModel()->hasSelection()) {
        mImagePanel->setSelectedRois({});
      } else {
        auto indexes = mPolygonsTable->selectionModel()->selectedIndexes();
        std::set<QGraphicsItem *> idxs;
        for(const auto row : indexes) {
          idxs.emplace(mTableModel->getCell(row.row())->item);
        }
        mImagePanel->setSelectedRois(idxs);
      }
    });
    // connect(mPolygonsTable, &QTableView::doubleClicked, [this](const QModelIndex &index) {});
  }

  setLayout(layout);

  connect(imagePanel, &PanelImageView::paintedPolygonsChanged, [this]() { mTableModel->refresh(); });
  connect(imagePanel, &PanelImageView::paintedPolygonClicked, [this](QList<QGraphicsItem *> idxs) {
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
  QDialog::hideEvent(event);
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
  mMoveAction->setChecked(true);
  mImagePanel->setState(PanelImageView::State::MOVE);

  emit dialogDisappeared();
  QDialog::closeEvent(event);
}

}    // namespace joda::ui::gui
