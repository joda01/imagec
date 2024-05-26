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
#include <qwidget.h>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <cmath>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include "backend/results/analyzer/plugins/heatmap_for_plate.hpp"
#include "backend/results/analyzer/plugins/heatmap_for_well.hpp"
#include "backend/results/db_column_ids.hpp"
#include "ui/container/container_button.hpp"
#include "ui/helper/layout_generator.hpp"

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
    connect(mHeatmap01, &ChartHeatMap::onDoubleClicked, this, &PanelHeatmap::onOpenNextLevel);
    auto *breadCrump = createBreadCrump(this);
    plateViewer->setContentsMargins(16, 0, 16, 16);
    plateViewer->addWidget(breadCrump);
    breadCrump->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    plateViewer->addWidget(mHeatmap01);
    mHeatmap01->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plateViewerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  //
  // Well edit
  //
  {
    auto [verticalLayoutContainer, _1] =
        joda::ui::qt::helper::addVerticalPanel(horizontalLayout, "rgba(218, 226, 255,0)", 0, false, 250, 250, 16);

    auto [verticalLayoutMeta, _2] =
        joda::ui::qt::helper::addVerticalPanel(verticalLayoutContainer, "rgb(246, 246, 246)");

    verticalLayoutMeta->addWidget(joda::ui::qt::helper::createTitle("Well editor"));

    auto *openWellButton = new ContainerButton("Open well", "", windowMain);
    connect(openWellButton, &ContainerButton::valueChanged, this, &PanelHeatmap::paintWell);
    verticalLayoutMeta->addWidget(openWellButton->getEditableWidget());

    _2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    verticalLayoutContainer->addStretch();
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
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::onOpenNextLevel(uint64_t id)
{
  int actMenu = static_cast<int>(mNavigation);
  actMenu++;
  if(actMenu <= 2) {
    mNavigation = static_cast<Navigation>(actMenu);
  }
  switch(mNavigation) {
    case Navigation::PLATE:
      paintPlate();
      break;
    case Navigation::WELL:
      mSelectedWellId.well.wellId = static_cast<uint16_t>(id);
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
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::CIRCLE);
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
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::RECTANGLE);
  }
}

///
/// \brief      Constructor
/// \author     Joachim Danmayr
///
void PanelHeatmap::paintImage()
{
  /*
  mBackButton->setEnabled(true);
  if(mAnalyzer != nullptr) {
    mNavigation = Navigation::IMAGE;
    auto result = joda::results::analyze::plugins::HeatmapForWell::getData(
        *mAnalyzer, mFilter.plateId, mHeatmap01->getSelectedWell(), mFilter.channelIdx, mFilter.measureChannel,
        mFilter.stats);
    mHeatmap01->setData(result, ChartHeatMap::MatrixForm::RECTANGLE);
  }*/
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

void ChartHeatMap::setData(const joda::results::Table &data, MatrixForm form)
{
  mData = data;
  mRows = mData.getRows();
  mCols = mData.getCols();

  mForm = form;
  update();
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

  uint32_t width  = size().width() - (spacing + X_LEFT_MARGIN);
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

    // Define rectangle properties
    uint32_t idx = 0;
    for(uint32_t x = 0; x < mCols; x++) {
      uint32_t txtX = x * rectWidth + spacing + X_LEFT_MARGIN + rectWidth / 2 - 6;
      painter.drawText(txtX, spacing * 4, QString::number(x + 1));

      for(uint32_t y = 0; y < mRows; y++) {
        uint32_t txtY   = y * rectWidth + rectWidth / 2 + spacing + Y_TOP_MARING;
        char toPrint[2] = {0};
        toPrint[0]      = y + 'A';
        painter.drawText(spacing, txtY, std::string(toPrint, 1).data());

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
        auto data    = mData.data();
        double value = data[y][x].getVal();
        double val   = (value - min) / (max - min);
        auto color   = mColorMap.upper_bound(val)->second;
        if(!data[y][x].isValid()) {
          color = QColor(255, 255, 255);
        }

        painter.setBrush(color);    // Change color as desired
        painter.fillPath(path, painter.brush());
        if(idx == mSelectedWell) {
          painter.setPen(QPen(Qt::green, 1));
        } else if(idx == mHoveredWell) {
          painter.setPen(QPen(Qt::red, 1));
        } else {
          painter.setPen(QPen(Qt::black, 1));
        }
        painter.drawPath(path);

        const int32_t xReduce = rectWidth / 3;
        const int32_t yReduce = rectWidth / 3;
        if(!data[y][x].isValid()) {
          painter.drawLine(x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                           x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);

          painter.drawLine(x * rectWidth + spacing + rectWidth - xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + yReduce + Y_TOP_MARING,
                           x * rectWidth + spacing + xReduce + X_LEFT_MARGIN,
                           y * rectWidth + spacing + rectWidth - yReduce + Y_TOP_MARING);
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
      uint32_t xStart = spacing + X_LEFT_MARGIN;
      uint32_t yStart = mRows * rectWidth + spacing + Y_TOP_MARING + 3 * spacing;
      float length    = (mCols * rectWidth + spacing + Y_TOP_MARING) - xStart;
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
  if(newSelectedWellId >= 0 && mSelectedWell != newSelectedWellId) {
    mSelectedWell  = newSelectedWellId;
    mSelectedPoint = selectedPoint;
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
void ChartHeatMap::mouseDoubleClickEvent(QMouseEvent *event)
{
  auto [newSelectedWellId, selectedPoint] = getWellUnderMouse(event);
  // Update hovering index and trigger repaint if necessary
  if(newSelectedWellId >= 0 && mSelectedWell != newSelectedWellId) {
    mSelectedWell  = newSelectedWellId;
    mSelectedPoint = selectedPoint;
    update();    // Trigger repaint to reflect hover state change
  }
  auto id = mData.data().at(mSelectedPoint.x).at(mSelectedPoint.y).getId();
  std::cout << "X: " << std::to_string(mSelectedPoint.x) << " Y:" << std::to_string(mSelectedPoint.y)
            << " U: " << std::to_string(id) << std::endl;
  emit onDoubleClicked(id);
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
  int32_t newSelectedWellId = -1;
  Point newSelectedWell;
  uint32_t width  = size().width() - (spacing + X_LEFT_MARGIN);
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
