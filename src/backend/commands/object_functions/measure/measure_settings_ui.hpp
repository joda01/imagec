///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qaction.h>
#include <qwidget.h>
#include <cstdint>
#include "backend/commands/classification/classifier_filter.hpp"
#include "backend/commands/command.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "measure_settings.hpp"

namespace joda::ui {

class Measure : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Measure";
  inline static std::string ICON  = "icons8-genealogy-50.png";

  Measure(settings::MeasureSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    auto *tab  = addTab("Input class", [] {});
    auto *col1 = tab->addVerticalPanel();
    // col1->addGroup("Input class");

    auto *addClassifier = addActionButton("Add class", "icons8-genealogy-50.png");
    // connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  settings::MeasureSettings &mSettings;
  QWidget *mParent;
private slots:
};

}    // namespace joda::ui
