///
/// \file      icon_generator.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-26
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qapplication.h>
#include <qicon.h>
#include <qsize.h>
#include <iostream>

namespace joda::ui::gui {

inline QIcon generateIcon(const QString &iconName)
{
  QIcon log;
  log.addFile(":/icons/icons/icons8-" + iconName + "-16.png", QSize(16, 16));
  log.addFile(":/icons/icons/icons8-" + iconName + "-30.png", QSize(30, 30));
  log.addFile(":/icons/icons/icons8-" + iconName + "-24.png", QSize(24, 24));
  log.addFile(":/icons/icons/icons8-" + iconName + "-40.png", QSize(40, 40));
  log.addFile(":/icons/icons/icons8-" + iconName + "-48.png", QSize(48, 48));
  log.addFile(":/icons/icons/icons8-" + iconName + "-80.png", QSize(80, 80));
  log.addFile(":/icons/icons/icons8-" + iconName + "-96.png", QSize(96, 96));
  return log;
}

inline QIcon generateSvgIcon(const QString &iconName)
{
  QIcon log;
  // log.addFile(":/icons-svg/16/icons-svg/16/" + iconName + ".svg", QSize(16, 16));
  // log.addFile(":/icons-svg/22/icons-svg/22/" + iconName + ".svg", QSize(22, 22));
  // log.addFile(":/icons-svg/32/icons-svg/32/" + iconName + ".svg", QSize(30, 30));
  // log.addFile(":/icons-svg/32/icons-svg/32/" + iconName + ".svg", QSize(24, 24));
  // log.addFile(":/icons-svg/32/icons-svg/32/" + iconName + ".svg", QSize(40, 40));
  // log.addFile(":/icons-svg/32/icons-svg/32/" + iconName + ".svg", QSize(48, 48));
  // log.addFile(":/icons-svg/32/icons-svg/32/" + iconName + ".svg", QSize(80, 80));

  // QSvgRenderer svgRenderer( somesvgfile );
  // QPixmap pix( svgRenderer.defaultSize() );
  log.addPixmap(QPixmap(":/icons-svg/22/icons-svg/22/" + iconName + ".svg").scaled(22, 22));
  if(log.isNull()) {
    std::cout << "Null " << iconName.toStdString() << std::endl;
  }

  return log;
}

enum class Style
{
  REGULAR,
  DUETONE
};

enum class Color
{
  BLACK,
  RED,
  GREEN,
  BLUE,
  GRAY,
  YELLOW,
  WHITE
};

template <Style STYLE, Color COLOR>
inline QIcon generateSvgIcon(const QString &iconName)
{
  QString colorStr = "black";
  if constexpr(COLOR == Color::BLACK) {
    colorStr = "black";
  }
  if constexpr(COLOR == Color::RED) {
    colorStr = "red";
  }
  if constexpr(COLOR == Color::GREEN) {
    colorStr = "green";
  }
  if constexpr(COLOR == Color::BLUE) {
    colorStr = "blue";
  }
  if constexpr(COLOR == Color::GRAY) {
    colorStr = "gray";
  }
  if constexpr(COLOR == Color::YELLOW) {
    colorStr = "yellow";
  }
  if constexpr(COLOR == Color::WHITE) {
    colorStr = "white";
  }

  if constexpr(STYLE == Style::REGULAR) {
    QIcon log;
    log.addPixmap(QPixmap(":/icons-svg/regular-" + colorStr + "/icons-svg/regular-" + colorStr + "/" + iconName + ".svg").scaled(256, 256));
    if(log.isNull()) {
      std::cout << "Null " << iconName.toStdString() << std::endl;
    }
    return log;
  }
  if constexpr(STYLE == Style::DUETONE) {
    QIcon log;
    log.addPixmap(QPixmap(":/icons-svg/duotone-" + colorStr + "/icons-svg/duotone-" + colorStr + "/" + iconName + "-duotone.svg").scaled(256, 256));
    if(log.isNull()) {
      std::cout << "Null " << iconName.toStdString() << std::endl;
    }
    return log;
  }
}

template <Style STYLE>
inline QIcon generateSvgIcon(Color COLOR, const QString &iconName)
{
  QString colorStr = "black";
  if(COLOR == Color::BLACK) {
    colorStr = "black";
  }
  if(COLOR == Color::RED) {
    colorStr = "red";
  }
  if(COLOR == Color::GREEN) {
    colorStr = "green";
  }
  if(COLOR == Color::BLUE) {
    colorStr = "blue";
  }
  if(COLOR == Color::GRAY) {
    colorStr = "gray";
  }
  if(COLOR == Color::YELLOW) {
    colorStr = "yellow";
  }

  if constexpr(STYLE == Style::REGULAR) {
    QIcon log;
    log.addPixmap(QPixmap(":/icons-svg/regular-" + colorStr + "/icons-svg/regular-" + colorStr + "/" + iconName + ".svg").scaled(256, 256));
    if(log.isNull()) {
      std::cout << "Null " << iconName.toStdString() << std::endl;
    }
    return log;
  }
  if constexpr(STYLE == Style::DUETONE) {
    QIcon log;
    log.addPixmap(QPixmap(":/icons-svg/duotone-" + colorStr + "/icons-svg/duotone-" + colorStr + "/" + iconName + "-duotone.svg").scaled(256, 256));
    if(log.isNull()) {
      std::cout << "Null " << iconName.toStdString() << std::endl;
    }
    return log;
  }
}

}    // namespace joda::ui::gui
