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

#include <qwidget.h>
#include <cstdint>
#include <string>
#include "backend/commands/command.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/layout_generator.hpp"
#include "ui/helper/setting_generator.hpp"
#include "image_saver_settings.hpp"

namespace joda::ui {

class ImageSaver : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Save control image";
  inline static std::string ICON  = "icons8-image-50.png";

  ImageSaver(settings::ImageSaverSettings &settings, QWidget *parent) :
      Command(TITLE.data(), ICON.data(), parent), mSettings(settings)
  {
    auto *tab = addTab("", [] {});
    {
      //
      //
      clustersIn = generateClusterDropDown<SettingComboBoxMulti<enums::ClusterIdIn>>("Clusters to paint", parent);

      //
      //
      classesIn = generateClassDropDown<SettingComboBoxMulti<enums::ClassId>>("Classes to paint", parent);

      //
      //
      style = SettingBase::create<SettingComboBox<settings::ImageSaverSettings::SaveClass::Style>>(parent, "", "Style");
      style->setDefaultValue(settings::ImageSaverSettings::SaveClass::Style::OUTLINED);
      style->addOptions({{.key = settings::ImageSaverSettings::SaveClass::Style::OUTLINED, .label = "Outlined"},
                         {.key = settings::ImageSaverSettings::SaveClass::Style::FILLED, .label = "Filled"}});

      //
      //
      mImageNamePrefix = SettingBase::create<SettingLineEdit<std::string>>(parent, "", "Image name prefix");
      mImageNamePrefix->setDefaultValue("control");
      mImageNamePrefix->setPlaceholderText("Name ...");
      mImageNamePrefix->setUnit("");
      mImageNamePrefix->setValue(settings.namePrefix);
      mImageNamePrefix->connectWithSetting(&settings.namePrefix);
    }
    //
    // Load from settings
    //
    {
      std::set<enums::ClusterIdIn> clustersToSet;
      std::set<enums::ClassId> classesToSet;
      for(const auto &cluster : settings.clustersIn) {
        clustersToSet.emplace(cluster.clusterIn);
        for(const auto &classs : cluster.classesIn) {
          classesToSet.emplace(classs.classIn);
          style->setValue(classs.style);
        }
      }

      clustersIn->setValue(clustersToSet);
      classesIn->setValue(classesToSet);
    }

    addSetting(tab, "Input classes", {{clustersIn.get(), true}, {classesIn.get(), true}});
    addSetting(tab, "Image name", {{mImageNamePrefix.get(), true}, {style.get(), false}});

    connect(style.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(classesIn.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(clustersIn.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<std::string>> mImageNamePrefix;
  std::unique_ptr<SettingComboBoxMulti<enums::ClusterIdIn>> clustersIn;
  std::unique_ptr<SettingComboBoxMulti<enums::ClassId>> classesIn;
  std::unique_ptr<SettingComboBox<settings::ImageSaverSettings::SaveClass::Style>> style;

  settings::ImageSaverSettings &mSettings;

  static inline const std::vector<std::string> colors = {
      "#0000FF",    // Blue
      "#00FF00",    // Green
      "#FF0000",    // Red
      "#FFFF00",    // Yellow
      "#00FFFF",    // Cyan
      "#FF00FF",    // Magenta
      "#808080",    // Gray
      "#C0C0C0",    // Silver
      "#000000",    // Black
      "#FF8C00",    // Orange
      "#9ACD32",    // YellowGreen
      "#008000",    // DarkGreen
      "#800000",    // Maroon
      "#8B0000",    // DarkRed
      "#000080",    // Navy
      "#4682B4",    // SteelBlue
      "#D3D3D3",    // LightGray
      "#A52A2A",    // Brown
      "#FFA500"     // Gold
  };

  void onChange()
  {
    auto clusters = clustersIn->getValue();
    auto classess = classesIn->getValue();
    int colorIdx  = 0;
    mSettings.clustersIn.clear();
    for(const auto &cluster : clusters) {
      settings::ImageSaverSettings::SaveCluster clusterObj;
      clusterObj.clusterIn = cluster;

      clusterObj.classesIn.clear();
      for(const auto &classs : classess) {
        clusterObj.classesIn.emplace_back(
            settings::ImageSaverSettings::SaveClass{.classIn          = classs,
                                                    .color            = colors[colorIdx % colors.size()],
                                                    .style            = style->getValue(),
                                                    .paintBoundingBox = false});
        colorIdx++;
      }
      mSettings.clustersIn.emplace_back(clusterObj);
    }
  }
};

}    // namespace joda::ui
