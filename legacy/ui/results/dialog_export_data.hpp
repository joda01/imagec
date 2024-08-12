///
/// \file      dialog_settings.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-29
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qtmetamacros.h>
#include <qwindow.h>
#include <memory>
#include <thread>
#include <tuple>
#include "backend/results/exporter/exporter.hpp"
#include "ui/dialog_shadow/dialog_shadow.h"
#include <nlohmann/json_fwd.hpp>

namespace joda::ui::qt {

class DialogExportData : public DialogShadow
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  DialogExportData(QWidget *windowMain);
  struct Ret
  {
    int ret = -1;
    std::map<results::MeasureChannel, results::Stats> channelsToExport;
    bool exportHeatmap = false;
    bool exportList    = false;
  };
  Ret execute();

private:
  std::map<std::tuple<results::MeasureChannel, results::Stats>, QCheckBox *> mChannelsToExport;
  int retVal          = 0;
  bool mExportHeatmap = false;
  bool mExportList    = false;

private slots:
  void onExportListClicked();
  void onExportHeatmapClicked();
  void onCancelClicked();
};

}    // namespace joda::ui::qt
