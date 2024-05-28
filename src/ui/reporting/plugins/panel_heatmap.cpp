///
/// \file      panel_heatmap.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-21
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "panel_heatmap.hpp"
#include <qaction.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qgridlayout.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include "backend/results/analyzer/plugins/heatmap_for_image.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_well.hpp"
#include "backend/results/db_column_ids.hpp"
#include "ui/container/container_button.hpp"
#include "ui/container/container_label.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/panel_preview.hpp"

namespace joda::ui::qt::reporting::plugin {

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
PanelHeatmap::PanelHeatmap(QMainWindow *windowMain, QWidget *parent) : QWidget(parent)
{
  // Create and set up the grid layout
  auto [horizontalLayout, centerWidget] = joda::ui::qt::helper::createLayout(this);
  horizontalLayout->setContentsMargins(0, 0, 0, 0);

  // vertical->addWidget(createBreadCrump());
  // vertical->addWidget(centerWidget);

  //
  // Plate view
  //
  {
    auto [plateViewer, plateViewerWidget] =
        joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgb(251, 252, 253)", 16, false, 800, 2048, 24);
    mHeatmap01 = new ChartHeatMap(this);
    connect(mHeatmap01, &ChartHeatMap::onElementClick, this, &PanelHeatmap::onElementSelected);
    connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelHeatmap::onOpenNextLevel);
    auto *breadCrump = createBreadCrump(this);
    plateViewer->setContentsMargins(16, 0, 16, 16);
    plateViewer->addWidget(breadCrump);
    breadCrump->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    //
    // Middle widget
    //
    // QWidget *middleWidget     = new QWidget();
    // QHBoxLayout *middleLayout = new QHBoxLayout();
    // middleLayout->addSpacing(64);
    // middleWidget->setLayout(middleLayout);
    //
    // Preview
    //
    // mPreviewImage = new PanelPreview(PREVIEW_BASE_SIZE, PREVIEW_BASE_SIZE, middleWidget);
    // mPreviewImage->resetImage("");
    // middleLayout->addWidget(mHeatmap01);
    // middleLayout->addWidget(mPreviewImage);

    //
    // Plate
    //
    plateViewer->addWidget(mHeatmap01);
    mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plateViewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  {
    auto [verticalLayoutContainer, _1] =
        joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 250, 16);

    //
    // Well edit
    //
    {
      auto [verticalLayoutMeta, _2] =
          joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");

      verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Info"));

      mLabelName = new ContainerLabel("...", "", windowMain);
      verticalLayoutMeta->addWidget(mLabelName->getEditableWidget());

      mLabelValue = new ContainerLabel("...", "", windowMain);
      verticalLayoutMeta->addWidget(mLabelValue->getEditableWidget());

      mLabelMeta = new ContainerLabel("...", "", windowMain);
      verticalLayoutMeta->addWidget(mLabelMeta->getEditableWidget());

      _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
      verticalLayoutContainer->addStretch();
    }
  }
  centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setLayout(horizontalLayout);
  // horizontalLayout->addStretch();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
QWidget *PanelHeatmap::createBreadCrump(QWidget *parent)
{
  QWidget *breadCrump = new QWidget(parent);
  breadCrump->setObjectName("BreadCrump");

  breadCrump->setStyleSheet(
      "QWidget#BreadCrump { border-radius: 12px; border: 2px none #696969; padding-top: 10px; "
      "padding-bottom: 10px;"
      "background-color: rgb(246, 246, 246);}");

  QHBoxLayout *layout = new QHBoxLayout(breadCrump);
  breadCrump->setMaximumHeight(48);
  breadCrump->setMaximumHeight(48);

  mBackButton = new QAction(QIcon(":/icons/outlined/icons8-left-50.png"), "Back");
  mBackButton->setEnabled(false);
  connect(mBackButton, &QAction::triggered, this, &PanelHeatmap::onBackClicked);
  QToolButton *actionButton = new QToolButton(this);
  actionButton->setDefaultAction(mBackButton);

  layout->addWidget(actionButton);

  breadCrump->setLayout(layout);

  layout->addStretch();

  return breadCrump;
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::onBackClicked()
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu--;
  if(actMenu >= 0) {
    mNavigation = static_cast<Navigation>(actMenu);
  }
  repaintHeatmap();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::setData(std::shared_ptr<joda::results::Analyzer> analyzer, const SelectedFilter &filter)
{
  mAnalyzer = std::move(analyzer);
  mFilter   = filter;
  repaintHeatmap();
}

///
/// \brief      An element has been selected
/// \author     Joachim Danmayr
///
void PanelHeatmap::onElementSelected(results::TableCell value)
{
  switch(mNavigation) {
    case Navigation::PLATE: {
      auto wellId = results::WellId{.well{.wellId = static_cast<uint16_t>(value.getId())}};
      mLabelName->setText("Well: " + QString(wellId.toString().data()));
      auto [result, channel] =
          mAnalyzer->getWellInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, wellId);

      mLabelValue->setText(QString(mFilter.measureChannel.toString().data()) + ": " + QString::number(value.getVal()));
      mLabelMeta->setText(channel.name.data());
    } break;
    case Navigation::WELL: {
      auto [image, channel] =
          mAnalyzer->getImageInformation(mFilter.analyzeId, mFilter.plateId, mFilter.channelIdx, value.getId());

      mLabelName->setText(image.originalImagePath.filename().string().data());
      mLabelValue->setText(QString(mFilter.measureChannel.toString().data()) + ": " + QString::number(value.getVal()));
      mLabelMeta->setText(channel.name.data());

    }

    break;
    case Navigation::IMAGE:
      mLabelName->setText("Selected Object " + QString::number(value.getId()));
      break;
  }
}

///
/// \brief      Open the next deeper level form the element with given id
/// \author     Joachim Danmayr
///
void PanelHeatmap::onOpenNextLevel(results::TableCell value)
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu++;
  if(actMenu <= 2) {
    mNavigation = static_cast<Navigation>(actMenu);
  } else {
    return;
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      paintPlate();
      break;
    case Navigation::WELL:
      mSelectedWellId.well.wellId = static_cast<uint16_t>(value.getId());
      paintWell();
      break;
    case Navigation::IMAGE:
      mSelectedImageId = value.getId();
      paintImage();
      break;
  }
  update();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::repaintHeatmap()
{
  switch(mNavigation) {
    case Navigation::PLATE:
      paintPlate();
      break;
    case Navigation::WELL:
      paintWell();
      break;
    case Navigation::IMAGE:
      paintImage();
      break;
  }
  update();
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::paintPlate()
{
  mBackButton->setEnabled(false);
  if(mAnalyzer != nullptr) {
    mNavigation = Navigation::PLATE;
    auto result = joda::results::analyze::plugins::HeatmapPerPlate::getData(
        *mAnalyzer, mFilter.plateId, mFilter.plateRows, mFilter.plateCols, mFilter.channelIdx, mFilter.measureChannel,
        mFilter.stats);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::CIRCLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::paintWell()
{
  mBackButton->setEnabled(true);
  if(mAnalyzer != nullptr) {
    mNavigation = Navigation::WELL;
    auto result = joda::results::analyze::plugins::HeatmapForWell::getData(
        *mAnalyzer, mFilter.plateId, mSelectedWellId, mFilter.channelIdx, mFilter.measureChannel, mFilter.stats);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::NO,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::paintImage()
{
  mBackButton->setEnabled(true);
  if(mAnalyzer != nullptr) {
    mNavigation = Navigation::IMAGE;
    auto result = joda::results::analyze::plugins::HeatmapForImage::getData(*mAnalyzer, mSelectedImageId,
                                                                            mFilter.channelIdx, mFilter.measureChannel,
                                                                            mFilter.stats, mFilter.densityMapAreaSize);
    mHeatmap01->setData(mAnalyzer, result, ChartHeatMap::MatrixForm::RECTANGLE, ChartHeatMap::PaintControlImage::YES,
                        static_cast<int32_t>(mNavigation));
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
ChartHeatMap::ChartHeatMap(PanelHeatmap *parent) : QWidget(parent), mParent(parent)
{
  setMinimumSize(parent->size());
  setMouseTracking(true);
}

void ChartHeatMap::setData(std::shared_ptr<joda::results::Analyzer> analyzer, const joda::results::Table &data,
                           MatrixForm form, PaintControlImage paint, int32_t newHierarchy)
{
  if(mActHierarchy > newHierarchy) {
    // We navigate back
    mSelection[mActHierarchy].mSelectedWell = -1;
    mHoveredWell                            = -1;
  }
  mActHierarchy = newHierarchy;
  mAnalyzer     = analyzer;
  mData         = data;
  mRows         = mData.getRows();
  mCols         = mData.getCols();

  mForm           = form;
  mPaintCtrlImage = PaintControlImage::NO;
  update();
  if(mSelection[mActHierarchy].mSelectedWell >= 0) {
    emit onElementClick(
        mData.data(mSelection[mActHierarchy].mSelectedPoint.x, mSelection[mActHierarchy].mSelectedPoint.y));
  }
}

///
/// \brief      Painter
/// \author     Joachim Danmayr
///
void ChartHeatMap::paintEvent(QPaintEvent *event)
{
  // mData.print();
  //  Create a random device and use it to seed the random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Create a uniform real distribution to produce numbers in the range [0, 1)
  std::uniform_real_distribution<> dis(0.0, 1.0);
  double dividend = 1;
  if(mPaintCtrlImage == PaintControlImage::YES) {
    dividend = 2;
  }

  uint32_t width  = size().width() / dividend - (spacing + X_LEFT_MARGIN);
  uint32_t height = size().height() - (spacing + Y_TOP_MARING + 2 * LEGEND_HEIGHT);

  auto [min, max] = mData.getMinMax();

  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);    // Enable smooth edges

    QFont fontHeader;
    fontHeader.setPixelSize(HEATMAP_FONT_SIZE);
    fontHeader.setBold(false);
    fontHeader.setFamily("Courier New");
    painter.setFont(fontHeader);
    QFontMetrics fm(fontHeader);

    QString newControlImagePath;

    // Define rectangle properties
    uint32_t idx = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      uint32_t txtX = x * rectWidth + spacing + X_LEFT_MARGIN + rectWidth / 2 - 6;
      painter.setPen(QPen(Qt::black, 1));
      painter.drawText(txtX, spacing * 4, QString::number(x + 1));

      for(uint32_t y = 0; y < mRows; y++) {
        uint32_t txtY = y * rectWidth + rectWidth / 2 + spacing + Y_TOP_MARING;
        painter.setPen(QPen(Qt::black, 1));
        char toPrint = y + 'A';
        painter.drawText(spacing, txtY, std::string(1, toPrint).data());

        uint32_t rectXPos = x * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = y * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos, rectYPos, rectWidth, rectWidth);
        int cornerRadius = 10;
        QPainterPath path;
        // path.addRoundedRect(rect, cornerRadius, cornerRadius);
        switch(mForm) {
          case MatrixForm::CIRCLE:
            path.addEllipse(rect);
            break;
          case MatrixForm::RECTANGLE:
            path.addRect(rect);
            break;
        }

        // Generate a random number
        // double random_number = dis(gen);
        auto data = mData.data(y, x);

        auto ctrl = data.getControlImagePath().string();
        if(!ctrl.empty()) {
          newControlImagePath = ctrl.data();
        }

        double value = data.getVal();
        double val   = (value - min) / (max - min);
        auto iter    = mColorMap.upper_bound(val);
        QColor color;
        if(iter != mColorMap.end()) {
          color = iter->second;
        } else {
          color = mColorMap[1];
        }
        if(!data.isValid()) {
          color = QColor(255, 255, 255);
        }

        painter.setBrush(color);    // Change color as desired
        painter.fillPath(path, painter.brush());
        if(data.isNAN() && !data.isValid()) {
          painter.setPen(QPen(Qt::lightGray, 1));
        } else if(idx == mSelection[mActHierarchy].mSelectedWell) {
          painter.setPen(QPen(Qt::green, 1));
        } else if(idx == mHoveredWell) {
          painter.setPen(QPen(Qt::red, 1));
        } else {
          painter.setPen(QPen(Qt::black, 1));
        }

        painter.drawPath(path);

        const int32_t xReduce = rectWidth / 3;
        const int32_t yReduce = rectWidth / 3;
        if(!data.isValid()) {
          if(!data.isNAN()) {
            painter.drawLine(x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                             x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);

            painter.drawLine(x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                             x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                             y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);
          }
        } else {
          QString txtToPaint = formatDoubleScientific(value);
          // Get text metrics
          QFontMetrics fontMetrics(painter.font());
          QRect textRect = fontMetrics.boundingRect(txtToPaint);
          int precision  = 2;
          while(textRect.width() >= rectWidth) {
            if(precision < 0) {
              txtToPaint = "...";
              textRect   = fontMetrics.boundingRect(txtToPaint);
              break;
            } else {
              txtToPaint = formatDoubleScientific(value, precision);
              textRect   = fontMetrics.boundingRect(txtToPaint);
            }
            precision--;
          }
          // Calculate center coordinates for text placement within the rectangle
          int textX = rect.center().x() - textRect.width() / 2;
          int textY = rect.center().y() + textRect.height() / 2 - fontMetrics.descent();    // Adjust for descent
          painter.drawText(textX, textY, txtToPaint);
        }
        idx++;
      }
    }

    //
    // Paint the legend
    //
    {
      painter.setPen(QPen(Qt::black, 1));
      uint32_t xStart = spacing + X_LEFT_MARGIN;
      uint32_t yStart = mRows * rectWidth + spacing + Y_TOP_MARING + 3 * spacing;
      float length    = (mCols * rectWidth + spacing + X_LEFT_MARGIN) - xStart;
      // painter.drawRect(xStart, yStart, length, LEGEND_HEIGHT);

      uint32_t partWith = std::floor(length / static_cast<float>(mColorMap.size()));
      for(int n = 0; n < mColorMap.size(); n++) {
        uint32_t startX = xStart + n * partWith;
        float val       = (float) n / (float) mColorMap.size();
        auto color      = mColorMap.upper_bound(val)->second;
        painter.setBrush(color);    // Change color as desired
        painter.drawRect(startX, yStart, partWith, LEGEND_COLOR_ROW_HEIGHT);

        if(n == 0) {
          painter.setPen(QPen(Qt::black, 1));
          painter.drawText(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE,
                           formatDoubleScientific(min));
        }
        if(n == mColorMap.size() - 1) {
          painter.setPen(QPen(Qt::black, 1));
          painter.drawText(startX, yStart + LEGEND_COLOR_ROW_HEIGHT + spacing + HEATMAP_FONT_SIZE,
                           formatDoubleScientific(max));
        }
      }
    }

    //
    // Paint control image
    //
    if(mPaintCtrlImage == PaintControlImage::YES) {
      if(newControlImagePath != mActControlImagePath) {
        mActControlImagePath = newControlImagePath;
        auto path            = mAnalyzer->getAbsolutePathToControlImage(mActControlImagePath.toStdString());

        mActControlImage.load(path.string().data());
        mActControlImage = mActControlImage.scaled(QSize(size().width() / 2, size().height()), Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
      }
      uint32_t xStart = spacing + X_LEFT_MARGIN + (mCols * rectWidth + spacing + Y_TOP_MARING);
      uint32_t yStart = spacing + Y_TOP_MARING;

      uint32_t width  = rectWidth * mCols;
      uint32_t height = rectWidth * mRows;
      painter.drawImage(xStart, yStart, mActControlImage);
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
void ChartHeatMap::mouseMoveEvent(QMouseEvent *event)
{
  auto [newHoveredWellId, _] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  if(newHoveredWellId >= 0 && mHoveredWell != newHoveredWellId) {
    mHoveredWell = newHoveredWellId;
    update();    // Trigger repaint to reflect hover state change
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mousePressEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mData.data(selectedPoint.x, selectedPoint.y);
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      update();    // Trigger repaint to reflect hover state change
    }

    emit onElementClick(selectedData);
  }
}

///
/// \brief
/// \authors
/// \param[in]
/// \param[out]
/// \return
///
void ChartHeatMap::mouseDoubleClickEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  auto selectedData = mData.data(selectedPoint.x, selectedPoint.y);
  if(!selectedData.isNAN()) {
    if(newSelectedWellId >= 0 && mSelection[mActHierarchy].mSelectedWell != newSelectedWellId) {
      mSelection[mActHierarchy].mSelectedWell  = newSelectedWellId;
      mSelection[mActHierarchy].mSelectedPoint = selectedPoint;
      update();    // Trigger repaint to reflect hover state change
    }
    emit onDoubleClicked(selectedData);
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
std::tuple<int32_t, ChartHeatMap::Point> ChartHeatMap::getWellUnderMouse(QMouseEvent *event)
{
  double dividend = 1;
  if(mPaintCtrlImage == PaintControlImage::YES) {
    dividend = 2;
  }

  int32_t newSelectedWellId = -1;
  Point newSelectedWell;
  uint32_t width  = size().width() / dividend - (spacing + X_LEFT_MARGIN);
  uint32_t height = size().height() - (spacing + Y_TOP_MARING + 2 * LEGEND_HEIGHT);
  auto [min, max] = mData.getMinMax();
  if(mRows > 0 && mCols > 0) {
    uint32_t rectWidth = std::min(width / mCols, height / mRows);
    uint32_t idx       = 0;
    for(uint32_t col = 0; col < mCols; col++) {
      for(uint32_t row = 0; row < mRows; row++) {
        uint32_t rectXPos = col * rectWidth + spacing + X_LEFT_MARGIN;
        uint32_t rectYPos = row * rectWidth + spacing + Y_TOP_MARING;
        QRectF rect(rectXPos, rectYPos, rectWidth, rectWidth);
        if(rect.contains(event->pos())) {
          newSelectedWellId = idx;
          newSelectedWell.x = row;
          newSelectedWell.y = col;
          break;
        }
        idx++;
      }
    }
  }
  return {newSelectedWellId, newSelectedWell};
}

QString ChartHeatMap::formatDoubleScientific(double value, int precision)
{
  QString formattedString = QString::number(value, 'e', precision);
  // Optional: Handle special cases like zero or infinity
  if(formattedString == "0.0e+00") {
    formattedString = "0";
  } else if(formattedString.contains("inf") || formattedString.contains("nan")) {
    formattedString = "N/A";    // Or any appropriate placeholder
  }
  return formattedString;
}

}    // namespace joda::ui::qt::reporting::plugin
