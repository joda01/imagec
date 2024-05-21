///
/// \file      panel_heatmap.hpp
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

#pragma once

#include <qwidget.h>
#include "backend/results/table/table.hpp"

namespace joda::ui::qt::reporting::plugin {

///
/// \class      ChartHeatMap
/// \author     Joachim Danmayr
/// \brief      Chart plotting a heatmap
///
class ChartHeatMap : public QWidget
{
public:
  /////////////////////////////////////////////////////
  ChartHeatMap(QWidget *parent);
  void setData(const joda::results::Table &);

private:
  /////////////////////////////////////////////////////
  void paintEvent(QPaintEvent *ev) override;
  joda::results::Table mData;
};

///
/// \class      PanelHeatmap
/// \author     Joachim Danmayr
/// \brief      Heatmap panel
///
class PanelHeatmap : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelHeatmap(QWidget *parent);
  void setData(const joda::results::Table &);

private:
  /////////////////////////////////////////////////////
  ChartHeatMap *mHeatmap01;
};

}    // namespace joda::ui::qt::reporting::plugin
