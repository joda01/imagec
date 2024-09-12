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
#include "ui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/container/setting/setting_combobox_multi_classification_in.hpp"
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

  ImageSaver(joda::settings::PipelineStep &pipelineStep, settings::ImageSaverSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {InOuts::OBJECT, InOuts::OBJECT}), mSettings(settings)
  {
    auto *tab = addTab("", [] {});
    {
      //
      //
      clustersIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, "", "Classes to paint");

      //
      //
      style = SettingBase::create<SettingComboBox<settings::ImageSaverSettings::Style>>(parent, "", "Style");
      style->setDefaultValue(settings::ImageSaverSettings::Style::OUTLINED);
      style->addOptions({{.key = settings::ImageSaverSettings::Style::OUTLINED, .label = "Outlined"},
                         {.key = settings::ImageSaverSettings::Style::FILLED, .label = "Filled"}});

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
      std::set<settings::ClassificatorSetting> clustersToSet;
      for(const auto &cluster : settings.clustersIn) {
        clustersToSet.emplace(cluster.inputCluster);
        style->setValue(cluster.style);
      }

      clustersIn->setValue(clustersToSet);
    }

    addSetting(tab, "Input classes", {{clustersIn.get(), true}});
    addSetting(tab, "Image name", {{mImageNamePrefix.get(), true}, {style.get(), false}});

    connect(style.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(clustersIn.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<std::string>> mImageNamePrefix;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> clustersIn;
  std::unique_ptr<SettingComboBox<settings::ImageSaverSettings::Style>> style;

  settings::ImageSaverSettings &mSettings;

  void onChange()
  {
    auto clusters = clustersIn->getValue();
    int colorIdx  = 0;
    mSettings.clustersIn.clear();
    for(const auto &cluster : clusters) {
      settings::ImageSaverSettings::SaveCluster clusterObj;
      clusterObj.inputCluster     = cluster;
      clusterObj.color            = settings::IMAGE_SAVER_COLORS[colorIdx % settings::IMAGE_SAVER_COLORS.size()];
      clusterObj.style            = style->getValue();
      clusterObj.paintBoundingBox = false;
      mSettings.clustersIn.emplace_back(clusterObj);
      colorIdx++;
    }
  }
};

}    // namespace joda::ui
