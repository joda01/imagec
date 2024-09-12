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
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox_multi.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "measure_settings.hpp"

namespace joda::ui {

class Measure : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Measure";
  inline static std::string ICON  = "icons8-ruler-50.png";

  Measure(joda::settings::PipelineStep &pipelineStep, settings::MeasureSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::OBJECT, InOuts::OBJECT}), mSettings(settings),
      mParent(parent)
  {
    //
    //
    clustersIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, "", "Cluster in");
    clustersIn->setValue(settings.inputClusters);
    clustersIn->connectWithSetting(&settings.inputClusters);

    //
    //
    cStackIndex = generateCStackCombo<SettingComboBoxMulti<int32_t>>("Image channel", parent);
    std::set<int32_t> cStacks;
    enums::ZProjection zProject = enums::ZProjection::NONE;
    for(const auto &cStack : settings.planesIn) {
      cStacks.emplace(cStack.imagePlane.cStack);
      zProject = cStack.zProjection;
    }
    cStackIndex->setValue(cStacks);
    connect(cStackIndex.get(), &SettingBase::valueChanged, this, &Measure::onCStackChanged);

    //
    //
    zProjection =
        SettingBase::create<SettingComboBox<enums::ZProjection>>(parent, "icons8-layers-50.png", "Z-Projection");
    zProjection->addOptions({{enums::ZProjection::NONE, "Off"},
                             {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                             {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                             {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}});
    zProjection->setValue(zProject);
    connect(zProjection.get(), &SettingBase::valueChanged, this, &Measure::onCStackChanged);

    //
    //
    auto *tab = addTab("Input class", [] {});
    addSetting(tab, "Input classes", {{clustersIn.get(), true}});
    addSetting(tab, "Input image channels", {{cStackIndex.get(), true}, {zProjection.get(), true}});

    // auto *addClassifier = addActionButton("Add class", "icons8-genealogy-50.png");
    //  connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMultiClassificationIn> clustersIn;
  std::unique_ptr<SettingComboBoxMulti<int32_t>> cStackIndex;
  std::unique_ptr<SettingComboBox<enums::ZProjection>> zProjection;

  settings::MeasureSettings &mSettings;
  QWidget *mParent;
private slots:

  void onCStackChanged()
  {
    std::cout << "Val changhed" << std::endl;
    auto cStacks = cStackIndex->getValue();
    mSettings.planesIn.clear();
    for(const auto &cStack : cStacks) {
      enums::ImageId id;
      id.imagePlane.cStack = cStack;
      id.zProjection       = zProjection->getValue();
      mSettings.planesIn.emplace_back(id);
    }
  }
};

}    // namespace joda::ui
