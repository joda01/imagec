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
#include "ui/gui/dialogs/dialog_interactive_ai_trainer/dialog_interactive_ai_trainer.hpp"
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
DialogImageViewer::DialogImageViewer(QWidget *parent, joda::settings::AnalyzeSettings *settings, QToolBar *toolbarParent) :
    QWidget(parent), mImageViewRight(parent), mSettings(settings)
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

    auto *histogram = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("chart-bar"), "Histogram");
    histogram->setObjectName("ToolButton");
    histogram->setStatusTip("Histogram");
    connect(histogram, &QAction::triggered, [this] {
      auto *dialog = new DialogHistogramSettings(&mImageViewRight, this);
      dialog->show();
    });
    toolbarTop->addAction(histogram);

    //
    // Image channel
    //

    auto *channelMenu      = new QMenu();
    mImageChannelMenuGroup = new QActionGroup(toolbarTop);

    auto addChannel = [this, &channelMenu](int32_t chNr) {
      QString numberName = "zero";
      switch(chNr) {
        case 1:
          numberName = "one";
          break;
        case 2:
          numberName = "two";
          break;
        case 3:
          numberName = "three";
          break;
        case 4:
          numberName = "four";
          break;
        case 5:
          numberName = "five";
          break;
        case 6:
          numberName = "six";
          break;
        case 7:
          numberName = "seven";
          break;
        case 8:
          numberName = "eight";
          break;
        case 9:
          numberName = "nine";
          break;
      }

      auto *action =
          channelMenu->addAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("number-square-" + numberName), "CH" + QString::number(chNr));
      action->setCheckable(true);
      if(chNr == 0) {
        action->setChecked(true);
      }
      mImageChannelMenuGroup->addAction(action);
      mChannelSelections.emplace(chNr, action);
    };
    for(int n = 0; n < 9; n++) {
      addChannel(n);
    }

    mImageChannel = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("number-square-zero"), "Image channel");
    mImageChannel->setStatusTip("Image channel to show");
    mImageChannel->setMenu(channelMenu);
    toolbarTop->addAction(mImageChannel);
    auto *btn = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(mImageChannel));
    btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    connect(mImageChannelMenuGroup, &QActionGroup::triggered, this, &DialogImageViewer::onSettingsChanged);
    connect(channelMenu, &QMenu::triggered, [this](QAction *triggeredAction) {
      if(triggeredAction != nullptr) {
        mImageChannel->setIcon(triggeredAction->icon());
      }
    });

    //
    // Open image settings
    //
    auto *imgSettings = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("wrench"), "Image settings");
    imgSettings->setObjectName("ToolButton");
    imgSettings->setStatusTip("Image settings");
    connect(imgSettings, &QAction::triggered, [this] {
      auto *dialog = new DialogImageSettings(&mImageSettings, this, mImageViewRight.getOmeInfo());
      if(dialog->exec() == QDialog::Accepted) {
        onSettingsChanged();
      }
    });
    toolbarTop->addAction(imgSettings);

    //
    // AI Training
    //
    mInteractiveAITraining = toolbarTop->addAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("brain"), "ML training");
    mInteractiveAITraining->setCheckable(true);
    mInteractiveAITraining->setStatusTip("Train pixel classifier");
    connect(mInteractiveAITraining, &QAction::toggled, [this](bool checked) {
      mInteractiveAiTrainer->blockSignals(true);
      if(checked) {
        mInteractiveAiTrainer->show();
      } else {
        mInteractiveAiTrainer->hide();
      }
      mInteractiveAiTrainer->blockSignals(false);
    });

    mInteractiveAiTrainer = new DialogInteractiveAiTrainer(&mImageViewRight, parent);
    connect(mInteractiveAiTrainer, &DialogInteractiveAiTrainer::dialogDisappeared, [this]() {
      //  mInteractiveAITraining->setChecked(false);
    });

    toolbarTop->addSeparator();

    // Painting tools
    auto *paintingToolActionGroup = new QActionGroup(toolbarTop);

    auto *move = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("hand"), "Move");
    move->setStatusTip("Move");
    move->setCheckable(true);
    move->setChecked(true);
    paintingToolActionGroup->addAction(move);
    toolbarTop->addAction(move);
    connect(move, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImageViewRight.setState(PanelImageView::State::MOVE);
      }
    });

    // Classification
    auto *classificationMenu = new QMenu();
    mPixelClassMenuGroup     = new QActionGroup(toolbarTop);

    auto addPixelClass = [this, &classificationMenu]<Color T>(int32_t classID, const QString &name) {
      auto *action = classificationMenu->addAction(generateSvgIcon<Style::DUETONE, T>("circle-dashed"), name);
      action->setCheckable(true);
      if(classID == 0) {
        action->setChecked(true);
      }
      mPixelClassMenuGroup->addAction(action);
      mPixelClassSelections.emplace(classID, action);
    };

    addPixelClass.operator()<Color::BLACK>(0, "Background");
    addPixelClass.operator()<Color::RED>(1, "Class 1");
    addPixelClass.operator()<Color::BLUE>(2, "Class 2");
    addPixelClass.operator()<Color::GREEN>(3, "Class 3");
    addPixelClass.operator()<Color::YELLOW>(4, "Class 4");

    mPixelClass = new QAction(generateSvgIcon<Style::DUETONE, Color::BLACK>("circle-dashed"), "Pixel class");
    mPixelClass->setStatusTip("Pixel class used for annotation");
    mPixelClass->setMenu(classificationMenu);
    toolbarTop->addAction(mPixelClass);
    auto *btnPxlClass = qobject_cast<QToolButton *>(toolbarTop->widgetForAction(mPixelClass));
    btnPxlClass->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
    connect(classificationMenu, &QMenu::triggered, [this](QAction *triggeredAction) {
      if(triggeredAction != nullptr) {
        mPixelClass->setIcon(triggeredAction->icon());
        auto pxClass = getSelectedPixelClass();
        QColor color = Qt::gray;
        switch(pxClass) {
          case 0:
            color = Qt::gray;
            break;
          case 1:
            color = Qt::red;
            break;
          case 2:
            color = Qt::blue;
            break;
          case 3:
            color = Qt::green;
            break;
          case 4:
            color = Qt::yellow;
            break;
          default:
            color = Qt::gray;
        }

        mImageViewRight.setSelectedPixelClass(pxClass, color);
      }
    });

    auto *paintRectangle = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("rectangle"), "Rectangle");
    paintRectangle->setStatusTip("Paint rectangle");
    paintRectangle->setCheckable(true);
    paintingToolActionGroup->addAction(paintRectangle);
    toolbarTop->addAction(paintRectangle);
    connect(paintRectangle, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImageViewRight.setState(PanelImageView::State::PAINT_RECTANGLE);
      }
    });

    auto *paintCircle = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("circle"), "Circle");
    paintCircle->setStatusTip("Paint circle");
    paintCircle->setCheckable(true);
    paintingToolActionGroup->addAction(paintCircle);
    toolbarTop->addAction(paintCircle);
    connect(paintCircle, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImageViewRight.setState(PanelImageView::State::PAINT_OVAL);
      }
    });

    auto *paintPolygon = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("polygon"), "Polygon");
    paintPolygon->setStatusTip("Paint polygon");
    paintPolygon->setCheckable(true);
    paintingToolActionGroup->addAction(paintPolygon);
    toolbarTop->addAction(paintPolygon);
    connect(paintPolygon, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImageViewRight.setState(PanelImageView::State::PAINT_POLYGON);
      }
    });

    auto *paintBrush = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("paint-brush"), "Brush");
    paintBrush->setStatusTip("Paint brush");
    paintBrush->setCheckable(true);
    paintingToolActionGroup->addAction(paintBrush);
    // toolbarTop->addAction(paintBrush);
    connect(paintBrush, &QAction::triggered, this, [this](bool checked) {
      if(checked) {
        mImageViewRight.setState(PanelImageView::State::PAIN_BRUSH);
      }
    });

    toolbarTop->addSeparator();

    auto *fitToScreen = new QAction(generateSvgIcon<Style::REGULAR, Color::BLUE>("magnifying-glass"), "Fit");
    fitToScreen->setStatusTip("Fit image to screen");
    fitToScreen->setObjectName("ToolButton");
    connect(fitToScreen, &QAction::triggered, this, &DialogImageViewer::onFitImageToScreenSizeClicked);
    toolbarTop->addAction(fitToScreen);

    toolbarTop->addSeparator();

    showOverlay = new QAction(generateSvgIcon<Style::REGULAR, Color::RED>("circle"), "Overlay");
    showOverlay->setStatusTip("Show/Hide results as overlay");
    showOverlay->setCheckable(true);
    showOverlay->setChecked(true);
    connect(showOverlay, &QAction::triggered, [this](bool selected) { mImageViewRight.setShowOverlay(selected); });
    toolbarTop->addAction(showOverlay);

    mFillOVerlay = new QAction(generateSvgIcon<Style::DUETONE, Color::RED>("circle"), "Fill");
    mFillOVerlay->setStatusTip("Fill/Outline results overlay");
    mFillOVerlay->setCheckable(true);
    connect(mFillOVerlay, &QAction::triggered, this, &DialogImageViewer::onSettingsChanged);
    toolbarTop->addAction(mFillOVerlay);

    mOverlayOpaque = new QSlider();
    mOverlayOpaque->setOrientation(Qt::Orientation::Horizontal);
    mOverlayOpaque->setMinimum(0);
    mOverlayOpaque->setMaximum(100);
    mOverlayOpaque->setValue(90);
    mOverlayOpaque->setMaximumWidth(100);
    connect(mOverlayOpaque, &QSlider::valueChanged,
            [this] { mImageViewRight.setOverlayOpaque(static_cast<float>(mOverlayOpaque->value()) / 100.0F); });
    mOverlayOpaqueAction = toolbarTop->addWidget(mOverlayOpaque);

    toolbarTop->addSeparator();

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

    showPixelInfo = new QAction(generateSvgIcon<Style::REGULAR, Color::BLACK>("vector-two"), "Pixel info");
    showPixelInfo->setStatusTip("Show/Hide pixel information");
    showPixelInfo->setCheckable(true);
    showPixelInfo->setChecked(true);
    connect(showPixelInfo, &QAction::triggered, this, &DialogImageViewer::onShowPixelInfo);
    toolbarTop->addAction(showPixelInfo);

    toolbarTop->addSeparator();

    if(toolbarParent == nullptr) {
      mMainLayout->addWidget(toolbarTop);
    }
  }

  // Central images
  {
    mCentralLayout      = new QBoxLayout(QBoxLayout::TopToBottom);
    auto *centralWidget = new QWidget();
    mCentralLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setContentsMargins(0, 0, 0, 0);
    auto *rightVerticalLayout = new QVBoxLayout();
    mImageViewRight.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightVerticalLayout->addWidget(&mImageViewRight);
    mCentralLayout->addLayout(rightVerticalLayout);
    mImageViewRight.resetImage();

    // centralLayout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    centralWidget->setLayout(mCentralLayout);
    mMainLayout->addWidget(centralWidget);

    connect(&mImageViewRight, &PanelImageView::tileClicked, this, &DialogImageViewer::onSettingsChanged);
    connect(&mImageViewRight, &PanelImageView::classesToShowChanged, this, &DialogImageViewer::onSettingsChanged);
    connect(&mImageViewRight, &PanelImageView::updateImage, [this]() {
      if(nullptr != mVideoButtonGroup) {
        mVideoButtonGroup->setMaxTimeStacks(mImageViewRight.getNrOfTstacks());
      }
    });
  }

  // Bottom toolbar
  {
    mVideoButtonGroup = new VideoControlButtonGroup([this]() { onSettingsChanged(); }, toolbarTop);
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
  mSelectedZStack            = settings.plane.z;
  mSelectedTStack            = settings.plane.t;
  if(nullptr != mVideoButtonGroup) {
    mVideoButtonGroup->setMaxTimeStacks(mImageViewRight.getNrOfTstacks());
    mVideoButtonGroup->setValue(settings.plane.t);
  }

  for(const auto &[chNr, action] : mChannelSelections) {
    if(chNr == settings.plane.c) {
      action->setChecked(true);
      mImageChannel->setIcon(action->icon());
    } else {
      action->setChecked(false);
    }
  }

  mImageViewRight.setSelectedTile(settings.tileX, settings.tileY);
  mImageSettings.tileWidth = settings.tileWidth;
  applySettingsToImagePanel();
  mImageChannelMenuGroup->blockSignals(false);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setImageChannel(int32_t channel)
{
  if(getSelectedImageChannel() == channel) {
    return;
  }
  for(const auto &[chNr, action] : mChannelSelections) {
    if(chNr == channel) {
      action->setChecked(true);
      mImageChannel->setIcon(action->icon());
    } else {
      action->setChecked(false);
    }
  }
  onSettingsChanged();
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
  mImageViewRight.setShowCrosshandCursor(show);
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
  return &mImageViewRight;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::setOverlayButtonsVisible(bool visible)
{
  mFillOVerlay->setVisible(visible);
  showOverlay->setVisible(visible);
  mOverlayOpaqueAction->setVisible(visible);
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
  mImageViewRight.setSeries(mImageSettings.imageSeries);
  if(nullptr != mVideoButtonGroup) {
    mVideoButtonGroup->setMaxTimeStacks(mImageViewRight.getNrOfTstacks());
    mSelectedTStack = mVideoButtonGroup->value();
  }
  mImageViewRight.setZprojection(getSelectedZProjection());
  mImageViewRight.setImagePlane({.z = mSelectedZStack, .c = getSelectedImageChannel(), .t = mSelectedTStack});
  mImageViewRight.setImageTile(tileSizeIn, tileSizeIn);
  mImageViewRight.setDefaultPhysicalSize(joda::settings::ProjectImageSetup::PhysicalSizeSettings{.mode          = mImageSettings.sizeMode,
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
  mImageViewRight.reloadImage();
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
  mImageViewRight.zoomImage(true);
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
  mImageViewRight.zoomImage(false);
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
  mImageViewRight.fitImageToScreenSize();
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
  mImageViewRight.setShowThumbnail(checked);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void DialogImageViewer::onShowPixelInfo(bool checked)
{
  mImageViewRight.setShowPixelInfo(checked);
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
  mImageViewRight.setShowCrosshandCursor(checked);
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
  if(mImageChannelMenuGroup != nullptr) {
    for(const auto &[chNr, action] : mChannelSelections) {
      if(action->isChecked()) {
        return chNr;
      }
    }
  }
  return 0;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int32_t DialogImageViewer::getSelectedPixelClass() const
{
  if(mPixelClassMenuGroup != nullptr) {
    for(const auto &[pxNr, action] : mPixelClassSelections) {
      if(action->isChecked()) {
        return pxNr;
      }
    }
  }
  return 0;
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
  mImageViewRight.setWaiting(waiting);
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
