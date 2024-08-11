
#pragma once

#include <QColor>
#include <iostream>
#include <map>

inline QColor interpolate(const QColor &start, const QColor &end, float t)
{
  int r = static_cast<int>(start.red() + t * (end.red() - start.red()));
  int g = static_cast<int>(start.green() + t * (end.green() - start.green()));
  int b = static_cast<int>(start.blue() + t * (end.blue() - start.blue()));
  return QColor(r, g, b);
}

inline std::map<float, QColor> generateColorMap()
{
  std::map<float, QColor> colorMap;
  QColor start(32, 102, 168);
  QColor middle(237, 237, 237);
  QColor end(174, 40, 44);

  for(int i = 0; i < 9; ++i) {
    float key     = i / 8.0;
    colorMap[key] = interpolate(start, middle, key);
  }

  for(int i = 0; i < 8; ++i) {
    float key     = (i + 9) / 16.0;
    colorMap[key] = interpolate(middle, end, key);
  }

  return colorMap;
}
