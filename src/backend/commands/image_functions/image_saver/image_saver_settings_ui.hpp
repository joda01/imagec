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
#include "ui/gui/container/command/command.hpp"
#include "ui/gui/container/setting/setting_combobox.hpp"
#include "ui/gui/container/setting/setting_combobox_classes_out.hpp"
#include "ui/gui/container/setting/setting_combobox_multi_classification_in.hpp"
#include "ui/gui/container/setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "ui/gui/helper/setting_generator.hpp"
#include "image_saver_settings.hpp"

namespace joda::ui::gui {

class ImageSaver : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Save control image";
  inline static std::string ICON              = "image";
  inline static std::string DESCRIPTION       = "...";
  inline static std::vector<std::string> TAGS = {};

  ImageSaver(joda::settings::PipelineStep &pipelineStep, settings::ImageSaverSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings)
  {
    auto *tab = addTab(
        "", [] {}, false);
    {
      //
      //
      classesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, generateIcon("circle"), "Classes to paint");

      //
      //
      style = SettingBase::create<SettingComboBox<settings::ImageSaverSettings::Style>>(parent, {}, "Style");
      style->setDefaultValue(settings::ImageSaverSettings::Style::OUTLINED);
      style->addOptions({{.key = settings::ImageSaverSettings::Style::OUTLINED, .label = "Outlined", .icon = generateIcon("no-filling")},
                         {.key = settings::ImageSaverSettings::Style::FILLED, .label = "Filled", .icon = generateIcon("fill-color")}});

      //
      //
      mImageNamePrefix = SettingBase::create<SettingLineEdit<std::string>>(parent, generateIcon("rename"), "Image name prefix");
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
      std::set<enums::ClassIdIn> classesToSet;
      for(const auto &classs : settings.classesIn) {
        classesToSet.emplace(classs.inputClass);
        style->setValue(classs.style);
      }

      classesIn->setValue(classesToSet);
    }

    addSetting(tab, "Input classes", {{classesIn.get(), true, 0}});
    addSetting(tab, "Image name", {{mImageNamePrefix.get(), true, 0}, {style.get(), false, 0}});

    connect(style.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(classesIn.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<std::string>> mImageNamePrefix;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> classesIn;
  std::unique_ptr<SettingComboBox<settings::ImageSaverSettings::Style>> style;

  settings::ImageSaverSettings &mSettings;

  void onChange()
  {
    auto classes = classesIn->getValue();
    int colorIdx = 0;
    mSettings.classesIn.clear();
    for(const auto &classs : classes) {
      settings::ImageSaverSettings::SaveClasss classsObj;
      classsObj.inputClass       = classs;
      classsObj.style            = style->getValue();
      classsObj.paintBoundingBox = false;
      mSettings.classesIn.emplace_back(classsObj);
      colorIdx++;
    }
  }
};

}    // namespace joda::ui::gui
