///
/// \file      graph_qt_backend.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "graph_qt_backend.hpp"
#include <qevent.h>
#include <QFile>
#include <QPainter>
#include <filesystem>
#include <thread>
#include "backend/helper/logger/console_logger.hpp"

namespace joda::ui::gui {

QtBackend::QtBackend(const std::string &terminal, QWidget *parent) : QWidget(parent), matplot::backend::gnuplot()
{
  setMouseTracking(true);    // <--- This line is crucial
}

QtBackend::~QtBackend()
{
  // Remove temp files
  std::filesystem::path svgFile(output());
  if(std::filesystem::exists(svgFile)) {
    std::filesystem::remove(svgFile);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
  std::lock_guard<std::mutex> lock(mPaintMutex);
  if((svgRenderer != nullptr) && svgRenderer->isValid()) {
    QPainter painter(this);

    QSizeF svgSize    = svgRenderer->defaultSize();    // original SVG size
    QSizeF widgetSize = size();

    // Calculate scaled size preserving aspect ratio
    QSizeF scaledSize = svgSize;
    scaledSize.scale(widgetSize, Qt::KeepAspectRatio);

    // Center the SVG in the widget
    targetRect = QRectF((widgetSize.width() - scaledSize.width()) / 2.0, (widgetSize.height() - scaledSize.height()) / 2.0, scaledSize.width(),
                        scaledSize.height());

    svgRenderer->render(&painter, targetRect);

    //
    // This is a grid used for mouse events
    //
    QPen redPen(Qt::red);
    redPen.setWidth(1);                // Optional: set line width
    redPen.setStyle(Qt::SolidLine);    // <-- Make it dashed
    painter.setPen(redPen);
    float offsetX = (targetRect.width() / 7.7);
    float offsetY = (targetRect.height() / 13.3);
    float width   = ((targetRect.width() - 2.05 * offsetX) / mCols);
    float height  = ((targetRect.height() - 2.5 * offsetY) / mRows);
    mRects.clear();
    int idx = 0;
    for(float col = 0; col < mCols; col++) {
      for(float row = 0; row < mRows; row++) {
        float startY = offsetY + targetRect.y() + row * height;
        float startX = offsetX + targetRect.x() + col * width;
        auto rect    = QRectF(startX, startY, width, height);
        mRects.push_back({rect, QPoint{static_cast<int>(col), static_cast<int>(row)}});
        if(mSelectedIndex == idx) {
          QRect tmp(rect.x() + 2, rect.y() + 2, rect.width() - 4, rect.height() - 4);
          painter.drawRect(tmp);
        }
        idx++;
      }
    }
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::mousePressEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
  QPoint pos = event->pos();
  int idx    = 0;
  for(const auto &[rect, pt] : mRects) {
    if(rect.contains(pos)) {
      emit onGraphClicked(pt.y(), pt.x());
      mSelectedIndex = idx;
      update();
      return;
    }
    idx++;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void QtBackend::mouseDoubleClickEvent(QMouseEvent *event)
{
  QWidget::mouseDoubleClickEvent(event);
  QPoint pos = event->pos();
  int idx    = 0;
  for(const auto &[rect, pt] : mRects) {
    if(rect.contains(pos)) {
      emit onGraphDoubleClicked(pt.y(), pt.x());
      mSelectedIndex = idx;
      update();
      return;
    }
    idx++;
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
bool QtBackend::render_data()
{
  using namespace std::chrono_literals;
  bool okay = flush_commands();

  //
  // We plot the graph to svg and then plot this svg in our qt widget
  //
  auto start = std::chrono::steady_clock::now();
  std::filesystem::path svgFile(output());
  do {
    std::this_thread::sleep_for(25ms);
    if(std::chrono::steady_clock::now() - start > 5s) {
      joda::log::logWarning("Could not plot graph: Timeout");
      return false;    // timeout expired
    }
    if(std::filesystem::exists(svgFile)) {
      auto size = std::filesystem::file_size(svgFile);
      if(size <= 0) {
        continue;
      }
      std::lock_guard<std::mutex> lock(mPaintMutex);
      delete svgRenderer;
      QFile file(svgFile.string().data());
      if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        joda::log::logDebug("Could not open graph file. Retry ...");
        file.close();
        continue;
      }
      QByteArray fileData = file.readAll();
      if(!fileData.contains("</svg>")) {
        file.close();
        continue;
      }

      svgRenderer = new QSvgRenderer(fileData, this);
      file.close();
      update();
      std::filesystem::remove(svgFile);
      break;
    }
  } while(true);

  return okay;
}

}    // namespace joda::ui::gui
