///
/// \file      panel_channel.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <QtWidgets>
#include <memory>

namespace joda::ui::qt {

class WindowMain;
class ContainerVoronoi;

class PanelVoronoiOverview : public QWidget
{
public:
  /////////////////////////////////////////////////////
  PanelVoronoiOverview(WindowMain *, ContainerVoronoi *);

private:
  /////////////////////////////////////////////////////
  void mousePressEvent(QMouseEvent *event) override;
  ContainerVoronoi *mParentContainer;
  WindowMain *mWindowMain;
};

}    // namespace joda::ui::qt
