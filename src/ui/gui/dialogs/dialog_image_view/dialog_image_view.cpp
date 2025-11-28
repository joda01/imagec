///
/// \file      dialog_image_view.cpp
/// \author    Joachim Danmayr
/// \date      2024-07-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "dialog_image_view.hpp"
#include <qaction.h>
#include <qactiongroup.h>
#include <qboxlayout.h>
#include <qdialog.h>
#include <qdockwidget.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmenu.h>
#include <qnamespace.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtoolbar.h>
#include <qwidget.h>
#include <cmath>
#include <cstdint>
#include <string>
#include <thread>
#include "backend/helper/image/image.hpp"
#include "ui/gui/dialogs/dialog_ml_trainer/dialog_ml_trainer.hpp"
#include "ui/gui/dialogs/dialog_roi_manager/dialog_roi_manager.hpp"
#include "ui/gui/dialogs/widget_video_control_button_group/widget_video_control_button_group.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "dialog_histogram_settings.hpp"
#include "dialog_image_settings.hpp"
#include "panel_image_view.hpp"

namespace joda::ui::gui {

using namespace std::chrono_literals;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::DialogImageViewer(QWidget *parent, const std::shared_ptr<atom::ObjectList> &objectMap, joda::settings::AnalyzeSettings *settings,
                                     QToolBar *toolbarParent) :
    QWidget(parent),
    mSettings(settings)
{
  setWindowTitle("Preview");
  setContentsMargins(0, 0, 0, 0);

  mMainLayout = new QVBoxLayout();
  mMainLayout->setContentsMargins(0, 0, 0, 0);

  // ======================================
  // Toolbar
  // ======================================
  QToolBar *toolbarTop;
  {
    if(toolbarParent == nullptr) {
      toolbarTop = new QToolBar();
    } else {
      toolbarTop = toolbarParent;
    }

    //
    // Painting tools
    //
    {
      auto *paintingToolActionGroup = new QActionGroup(toolbarTop);

      mMoveAction = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("hand"), "Move");
      mMoveAction->setStatusTip("Move image");
      mMoveAction->setCheckable(true);
      mMoveAction->setChecked(true);
      paintingToolActionGroup->addAction(mMoveAction);
      toolbarTop->addAction(mMoveAction);
      connect(mMoveAction, &QAction::triggered, this, [this](bool checked) {
        if(checked) {
          mImagePanel->setState(PanelImageView::State::MOVE);
        }
      });

      mSelectAction = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("cursor-click"), "Select");
      mSelectAction->setStatusTip("Select and move region of interests");
      mSelectAction->setCheckable(true);
      paintingToolActionGroup->addAction(mSelectAction);
      toolbarTop->addAction(mSelectAction);
      connect(mSelectAction, &QAction::triggered, this, [this](bool checked) {
        if(checked) {
          mImagePanel->setState(PanelImageView::State::SELECT);
        }
      });

      mSeparatorStatePaint = toolbarTop->addSeparator();

      mActionPaintRectangle = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("rectangle"), "Rectangle");
      mActionPaintRectangle->setStatusTip("Paint rectangle");
      mActionPaintRectangle->setCheckable(true);
      paintingToolActionGroup->addAction(mActionPaintRectangle);
      toolbarTop->addAction(mActionPaintRectangle);
      connect(mActionPaintRectangle, &QAction::triggered, this, [this](bool checked) {
        if(checked) {
          mImagePanel->setState(PanelImageView::State::PAINT_RECTANGLE);
        }
      });

      mActionPaintCircle = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("circle"), "Circle");
      mActionPaintCircle->setStatusTip("Paint circle");
      mActionPaintCircle->setCheckable(true);
      paintingToolActionGroup->addAction(mActionPaintCircle);
      toolbarTop->addAction(mActionPaintCircle);
      connect(mActionPaintCircle, &QAction::triggered, this, [this](bool checked) {
        if(checked) {
          mImagePanel->setState(PanelImageView::State::PAINT_OVAL);
        }
      });

      mPaintPolygon = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("polygon"), "Polygon");
      mPaintPolygon->setStatusTip("Paint polygon");
      mPaintPolygon->setCheckable(true);
      paintingToolActionGroup->addAction(mPaintPolygon);
      toolbarTop->addAction(mPaintPolygon);
      connect(mPaintPolygon, &QAction::triggered, this, [this](bool checked) {
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

      auto *magicWand = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("magic-wand"), "Magic wand tool");
      magicWand->setStatusTip("Paint brush");
      magicWand->setCheckable(true);
      paintingToolActionGroup->addAction(magicWand);
      // toolbar->addAction(magicWand);
      connect(magicWand, &QAction::triggered, this, [this](bool checked) {
        if(checked) {
          mImagePanel->setState(PanelImageView::State::PAINT_MAGIC_WAND);
        }
      });
    }

    mSeparatorPaintHistogram = toolbarTop->addSeparator();

    //
    // Histogram
    //
    auto *histogram = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("chart-bar"), "Histogram");
    histogram->setObjectName("ToolButton");
    histogram->setStatusTip("Histogram");
    connect(histogram, &QAction::triggered, [this] { mHistogramSettings->show(); });
    toolbarTop->addAction(histogram);

    //
    // Open image settings
    //
    auto *imgSettings = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("wrench"), "Image settings");
    imgSettings->setObjectName("ToolButton");
    imgSettings->setStatusTip("Image settings");
    connect(imgSettings, &QAction::triggered, [this] {
      auto *dialog = new DialogImageSettings(&mImageSettings, this, mImagePanel->getOmeInfo());
      if(dialog->exec() == QDialog::Accepted) {
        onSettingsChanged();
      }
    });
    toolbarTop->addAction(imgSettings);
    mSeparatorHistogramMlTraining = toolbarTop->addSeparator();

    //
    // ML Trainer
    //
    {
      mActionMlTrainer = toolbarTop->addAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("fediverse-logo"), "ML Trainer");
      mActionMlTrainer->setCheckable(true);
      mActionMlTrainer->setStatusTip("Train pixel and object classifier");
    }

    toolbarTop->addSeparator();

    auto *fitToScreen = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("magnifying-glass"), "Fit");
    fitToScreen->setStatusTip("Fit image to screen");
    fitToScreen->setObjectName("ToolButton");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    toolbarTop->addSeparator();

    showOverlay = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("paint-bucket"), "Overlay");
    showOverlay->setStatusTip("Show/Hide results as overlay");
    showOverlay->setCheckable(true);
    showOverlay->setChecked(true);
    connect(showOverlay, &QAction::triggered, [this](bool selected) { mImagePanel->setShowRois(selected); });
    toolbarTop->addAction(showOverlay);

    mFillOVerlay = new QAction(generateSvgIcon<Style::DUETONE, Color::RED>("circle"), "Fill");
    mFillOVerlay->setStatusTip("Fill/Outline results overlay");
    mFillOVerlay->setCheckable(true);
    mFillOVerlay->setChecked(true);
    connect(mFillOVerlay, &QAction::triggered, [this](bool selected) { mImagePanel->setFillRois(selected); });
    // toolbarTop->addAction(mFillOVerlay);

    mOverlayOpaque = new QSlider();
    mOverlayOpaque->setOrientation(Qt::Orientation::Horizontal);
    mOverlayOpaque->setMinimum(0);
    mOverlayOpaque->setMaximum(100);
    mOverlayOpaque->setValue(80);
    mOverlayOpaque->setMaximumWidth(100);
    connect(mOverlayOpaque, &QSlider::valueChanged, [this] { mImagePanel->setRoisOpaque(static_cast<float>(mOverlayOpaque->value()) / 100.0F); });
    mOverlayOpaqueAction = toolbarTop->addWidget(mOverlayOpaque);

    mSeparatorFillAndOverlays = toolbarTop->addSeparator();

    showCrossHairCursor = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("crosshair"), "");
    showCrossHairCursor->setStatusTip("Show/Hide cross hair cursor (right click to place)");
    showCrossHairCursor->setCheckable(true);
    showCrossHairCursor->setChecked(false);
    connect(showCrossHairCursor, &QAction::triggered, this, &DialogImageViewer::onShowCrossHandCursor);
    toolbarTop->addAction(showCrossHairCursor);

    auto *showThumbnail = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("picture-in-picture"), "Thumbnail");
    showThumbnail->setStatusTip("Show/Hide image thumbnail");
    showThumbnail->setCheckable(true);
    showThumbnail->setChecked(true);
    connect(showThumbnail, &QAction::triggered, this, &DialogImageViewer::onShowThumbnailChanged);
    toolbarTop->addAction(showThumbnail);

    toolbarTop->addSeparator();

    if(toolbarParent == nullptr) {
      mMainLayout->addWidget(toolbarTop);
    }
  }

  {
    mVideoButtonGroup = new VideoControlButtonGroup([this]() { onSettingsChanged(); }, toolbarTop);
    mImagePanel       = new PanelImageView(objectMap, &settings->projectSettings.classification, mVideoButtonGroup, parent);
    mImagePanel->setFillRois(true);

    mHistogramSettings = new DialogHistogramSettings(mImagePanel, parent);
  }
  {
    mDialogMlTrainer = new DialogMlTrainer(settings, objectMap, mImagePanel, parent);
    connect(mDialogMlTrainer, &DialogMlTrainer::dialogDisappeared, [this]() {
      mActionMlTrainer->blockSignals(true);
      mActionMlTrainer->setChecked(false);
      mActionMlTrainer->blockSignals(false);
    });
    connect(mActionMlTrainer, &QAction::triggered, [this](bool checked) {
      mDialogMlTrainer->blockSignals(true);
      if(checked) {
        mDialogMlTrainer->show();
      } else {
        mDialogMlTrainer->close();
      }
      mDialogMlTrainer->blockSignals(false);
    });
  }
  // Central images
  {
    mCentralLayout      = new QBoxLayout(QBoxLayout::TopToBottom);
    auto *centralWidget = new QWidget();
    mCentralLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    auto *rightVerticalLayout = new QVBoxLayout();
    mImagePanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightVerticalLayout->addWidget(mImagePanel);
    mCentralLayout->addLayout(rightVerticalLayout);
    mImagePanel->resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(mCentralLayout);
    mMainLayout->addWidget(centralWidget);

    connect(mImagePanel, &PanelImageView::tileClicked, this, &DialogImageViewer::onSettingsChanged);
    connect(mImagePanel, &PanelImageView::imageOpened, [this]() {
      if(nullptr != mVideoButtonGroup) {
        mVideoButtonGroup->setMaxTimeStacks(mImagePanel->getNrOfTstacks());
      }
    });

    connect(mImagePanel, &PanelImageView::drawingToolChanged, [this](PanelImageView::State state) {
      switch(state) {
        case PanelImageView::MOVE:
          mMoveAction->setChecked(true);
          break;
        case PanelImageView::SELECT:
          mSelectAction->setChecked(true);
          break;
        case PanelImageView::PAINT_RECTANGLE:
          mActionPaintRectangle->setChecked(true);
          break;
        case PanelImageView::PAINT_OVAL:
          mActionPaintCircle->setChecked(true);
          break;
        case PanelImageView::PAINT_POLYGON:
          mPaintPolygon->setChecked(true);
          break;
        case PanelImageView::PAIN_BRUSH:
          break;
        case PanelImageView::PAINT_MAGIC_WAND:
          break;
      }
    });
  }

  // setLayout(layout);
  setLayout(mMainLayout);

  onSettingsChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
DialogImageViewer::~DialogImageViewer()
{
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setImagePlane(const ImagePlaneSettings &settings)
{
  mImageSettings.imageSeries = settings.series;
  mSelectedZStack            = settings.plane.zStack;
  mSelectedTStack            = settings.plane.tStack;
  if(nullptr != mVideoButtonGroup) {
    mVideoButtonGroup->setMaxTimeStacks(mImagePanel->getNrOfTstacks());
    mVideoButtonGroup->setValue(settings.plane.tStack);
  }

  mImagePanel->setSelectedTile(settings.tileX, settings.tileY);
  mImageSettings.tileWidth = settings.tileWidth;
  applySettingsToImagePanel();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setShowCrossHairCursor(bool show)
{
  showCrossHairCursor->setChecked(show);
  mImagePanel->setShowCrosshandCursor(show);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::fromSettings(const joda::settings::AnalyzeSettings &settings)
{
  if(mSettings != nullptr) {
    mImageSettings.zProjection   = settings.imageSetup.zStackSettings.defaultZProjection;
    mImageSettings.pixelHeight   = settings.imageSetup.imagePixelSizeSettings.pixelHeight;
    mImageSettings.pixelWidth    = settings.imageSetup.imagePixelSizeSettings.pixelWidth;
    mImageSettings.pixelSizeUnit = settings.imageSetup.imagePixelSizeSettings.pixelSizeUnit;
    mImageSettings.sizeMode      = settings.imageSetup.imagePixelSizeSettings.mode;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogImageViewer::getImagePanel() -> PanelImageView *
{
  return mImagePanel;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setReadOnly(bool visible)
{
  CHECK_GUI_THREAD(mFillOVerlay)
  mFillOVerlay->setVisible(visible);
  CHECK_GUI_THREAD(showOverlay)
  showOverlay->setVisible(visible);
  CHECK_GUI_THREAD(mOverlayOpaqueAction)
  mOverlayOpaqueAction->setVisible(visible);
  CHECK_GUI_THREAD(mMoveAction)
  mMoveAction->setVisible(visible);
  CHECK_GUI_THREAD(mSelectAction)
  mSelectAction->setVisible(visible);
  CHECK_GUI_THREAD(mActionPaintRectangle)
  mActionPaintRectangle->setVisible(visible);
  CHECK_GUI_THREAD(mActionPaintCircle)
  mActionPaintCircle->setVisible(visible);
  CHECK_GUI_THREAD(mPaintPolygon)
  mPaintPolygon->setVisible(visible);
  CHECK_GUI_THREAD(mActionMlTrainer)
  mActionMlTrainer->setVisible(visible);

  CHECK_GUI_THREAD(mSeparatorStatePaint)
  mSeparatorStatePaint->setVisible(visible);
  CHECK_GUI_THREAD(mSeparatorPaintHistogram)
  mSeparatorPaintHistogram->setVisible(visible);
  CHECK_GUI_THREAD(mSeparatorHistogramMlTraining)
  mSeparatorHistogramMlTraining->setVisible(visible);
  CHECK_GUI_THREAD(mSeparatorFillAndOverlays)
  mSeparatorFillAndOverlays->setVisible(visible);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::removeVideoControl()
{
  mVideoButtonGroup->setVisible(false);
  delete mVideoButtonGroup;
  mVideoButtonGroup = nullptr;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::applySettingsToImagePanel()
{
  auto tileSizeIn = getTileSize();
  mImagePanel->setSeries(mImageSettings.imageSeries);
  if(nullptr != mVideoButtonGroup) {
    mVideoButtonGroup->setMaxTimeStacks(mImagePanel->getNrOfTstacks());
    mSelectedTStack = mVideoButtonGroup->value();
    if(mVideoButtonGroup->isVideoRunning()) {
      mImagePanel->setWaitBannerVisible(false);
    } else {
      mImagePanel->setWaitBannerVisible(true);
    }
  } else {
    mImagePanel->setWaitBannerVisible(true);
  }
  mImagePanel->setZprojection(getSelectedZProjection());
  mImagePanel->setImagePlane({.tStack = mSelectedTStack, .zStack = mSelectedZStack, .cStack = getSelectedImageChannel()});
  mImagePanel->setImageTile(tileSizeIn, tileSizeIn);
  mImagePanel->setDefaultPhysicalSize(joda::settings::ProjectImageSetup::PhysicalSizeSettings{.mode          = mImageSettings.sizeMode,
                                                                                              .pixelSizeUnit = mImageSettings.pixelSizeUnit,
                                                                                              .pixelWidth    = mImageSettings.pixelWidth,
                                                                                              .pixelHeight   = mImageSettings.pixelHeight});

  // Sync to settings
  if(mSettings != nullptr) {
    auto tileSize                                           = getTileSize();
    mSettings->imageSetup.zStackSettings.defaultZProjection = getSelectedZProjection();
    mSettings->imageSetup.imageTileSettings.tileHeight      = tileSize;
    mSettings->imageSetup.imageTileSettings.tileWidth       = tileSize;
    mSettings->imageSetup.series                            = mImageSettings.imageSeries;

    mSettings->imageSetup.imagePixelSizeSettings.pixelHeight   = mImageSettings.pixelHeight;
    mSettings->imageSetup.imagePixelSizeSettings.pixelWidth    = mImageSettings.pixelWidth;
    mSettings->imageSetup.imagePixelSizeSettings.pixelSizeUnit = mImageSettings.pixelSizeUnit;
    mSettings->imageSetup.imagePixelSizeSettings.mode          = mImageSettings.sizeMode;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onSettingsChanged()
{
  applySettingsToImagePanel();
  mImagePanel->reloadImage();
  emit settingChanged();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onZoomInClicked()
{
  mImagePanel->zoomImage(true);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onZoomOutClicked()
{
  mImagePanel->zoomImage(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onFitImageToScreenSizeClicked()
{
  mImagePanel->fitImageToScreenSize();
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowThumbnailChanged(bool checked)
{
  mImagePanel->setShowThumbnail(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowCrossHandCursor(bool checked)
{
  mImagePanel->setShowCrosshandCursor(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
auto DialogImageViewer::getSelectedZProjection() const -> enums::ZProjection
{
  return mImageSettings.zProjection;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getTileSize() const
{
  return mImageSettings.tileWidth;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSeries() const
{
  return mImageSettings.imageSeries;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSelectedImageChannel() const
{
  return mImagePanel->getImagePlane().cStack;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSelectedTimeStack() const
{
  if(nullptr != mVideoButtonGroup) {
    return mVideoButtonGroup->value();
  }
  return mSelectedTStack;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setWaiting(bool waiting)
{
  // Don't show waiting dialog if video is running for a better view.
  if(mVideoButtonGroup != nullptr && mVideoButtonGroup->isVideoRunning()) {
    return;
  }
  mImagePanel->setWaiting(waiting);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
bool DialogImageViewer::getFillOverlay() const
{
  return mFillOVerlay->isChecked();
}

}    // namespace joda::ui::gui
