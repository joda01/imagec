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
#include "ui/container/setting/setting_combobox _multi.hpp"
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

  Measure(settings::MeasureSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settings), mParent(parent)
  {
    //
    //
    clustersIn =
        SettingBase::create<SettingComboBoxMulti<enums::ClusterIdIn>>(parent, "icons8-connection-50.png", "Cluster In");
    clustersIn->addOptions({
        {enums::ClusterIdIn::$, "THIS"},
        {enums::ClusterIdIn::A, "A"},
        {enums::ClusterIdIn::B, "B"},
        {enums::ClusterIdIn::C, "C"},
        {enums::ClusterIdIn::D, "D"},
        {enums::ClusterIdIn::E, "E"},
        {enums::ClusterIdIn::F, "F"},
        {enums::ClusterIdIn::G, "G"},
        {enums::ClusterIdIn::H, "H"},
        {enums::ClusterIdIn::I, "I"},
        {enums::ClusterIdIn::J, "J"},
    });
    clustersIn->setValue(settings.clustersIn);
    clustersIn->connectWithSetting(&settings.clustersIn);

    //
    //
    classesIn = SettingBase::create<SettingComboBoxMulti<enums::ClassId>>(parent, "", "Classes In");
    classesIn->addOptions({
        {enums::ClassId::C0, "0"},
        {enums::ClassId::C1, "1"},
        {enums::ClassId::C2, "2"},
        {enums::ClassId::C3, "3"},
        {enums::ClassId::C4, "4"},
        {enums::ClassId::C5, "5"},
        {enums::ClassId::C6, "6"},
        {enums::ClassId::C7, "7"},
        {enums::ClassId::C8, "8"},
        {enums::ClassId::C9, "9"},
    });
    classesIn->setValue(settings.classesIn);
    classesIn->connectWithSetting(&settings.classesIn);

    //
    //
    cStackIndex =
        SettingBase::create<SettingComboBoxMulti<int32_t>>(parent, "icons8-unknown-status-50.png", "Image channel");
    cStackIndex->addOptions({{0, "CH 0"},
                             {1, "CH 1"},
                             {2, "CH 2"},
                             {3, "CH 3"},
                             {4, "CH 4"},
                             {5, "CH 5"},
                             {6, "CH 6"},
                             {7, "CH 7"},
                             {8, "CH 8"},
                             {9, "CH 9"},
                             {10, "CH 10"}});
    std::set<int32_t> cStacks;
    enums::ZProjection zProject = enums::ZProjection::NONE;
    for(const auto &cStack : settings.planesIn) {
      cStacks.emplace(cStack.imagePlane.cStack);
      zProject = cStack.imageIdx;
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
    addSetting(tab, "Input classes", {{clustersIn.get(), true}, {classesIn.get(), true}});
    addSetting(tab, "Input image channels", {{cStackIndex.get(), true}, {zProjection.get(), true}});

    // auto *addClassifier = addActionButton("Add class", "icons8-genealogy-50.png");
    //  connect(addClassifier, &QAction::triggered, this, &Classifier::addClassifier);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingComboBoxMulti<enums::ClusterIdIn>> clustersIn;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> classesIn;
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
      id.imageIdx          = zProjection->getValue();
      mSettings.planesIn.emplace_back(id);
    }
  }
};

}    // namespace joda::ui
