///
/// \file      command.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
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
  inline static std::string ICON              = "image-png";
  inline static std::string DESCRIPTION       = "Save a control image to disk.";
  inline static std::vector<std::string> TAGS = {"save", "control image", "image"};

  ImageSaver(joda::settings::PipelineStep &pipelineStep, settings::ImageSaverSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent, {{InOuts::OBJECT}, {InOuts::OBJECT}}), mSettings(settings)
  {
    auto *tab = addTab(
        "", [] {}, false);
    {
      //
      //
      classesIn = SettingBase::create<SettingComboBoxMultiClassificationIn>(parent, {}, "Classes to paint");

      //
      //
      canvas = SettingBase::create<SettingComboBox<settings::ImageSaverSettings::Canvas>>(parent, {}, "Canvas");
      canvas->setDefaultValue(settings::ImageSaverSettings::Canvas::IMAGE_$);
      canvas->addOptions(
          {{.key = settings::ImageSaverSettings::Canvas::IMAGE_$, .label = "Image", .icon = generateSvgIcon("color-mode-invert-image")},
           {.key = settings::ImageSaverSettings::Canvas::WHITE, .label = "White", .icon = generateSvgIcon("color-mode-invert-image")},
           {.key = settings::ImageSaverSettings::Canvas::BLACK, .label = "Black", .icon = generateSvgIcon("color-mode-invert-image")}});
      canvas->setValue(settings.canvas);
      canvas->connectWithSetting(&settings.canvas);

      //
      //
      style = SettingBase::create<SettingComboBox<settings::ImageSaverSettings::Style>>(parent, {}, "Style");
      style->setDefaultValue(settings::ImageSaverSettings::Style::OUTLINED);
      style->addOptions({{.key = settings::ImageSaverSettings::Style::OUTLINED, .label = "Outlined", .icon = generateSvgIcon("fill-color")},
                         {.key = settings::ImageSaverSettings::Style::FILLED, .label = "Filled", .icon = generateSvgIcon("fill-color")}});

      //
      //
      boundingBox = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Bounding box");
      boundingBox->setDefaultValue(false);
      boundingBox->addOptions({{.key = false, .label = "No bounding box", .icon = generateSvgIcon("select-rectangular")},
                               {.key = true, .label = "With bounding box", .icon = generateSvgIcon("select-rectangular")}});

      //
      //
      objectId = SettingBase::create<SettingComboBox<bool>>(parent, {}, "Object ID");
      objectId->setDefaultValue(false);
      objectId->addOptions({{.key = false, .label = "No object ID", .icon = generateSvgIcon("irc-remove-operator")},
                            {.key = true, .label = "With object ID", .icon = generateSvgIcon("irc-operator")}});

      //
      //
      mImageNamePrefix = SettingBase::create<SettingLineEdit<std::string>>(parent, generateSvgIcon("filename-initial-amarok"), "Image name prefix");
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
        boundingBox->setValue(classs.paintBoundingBox);
        objectId->setValue(classs.paintObjectId);
      }

      classesIn->setValue(classesToSet);
    }

    addSetting(tab, "Input classes", {{classesIn.get(), true, 0}, {mImageNamePrefix.get(), true, 0}});
    addSetting(tab, "Style", {{canvas.get(), false, 0}, {style.get(), false, 0}, {boundingBox.get(), false, 0}, {objectId.get(), false, 0}});

    connect(style.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(classesIn.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(boundingBox.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
    connect(objectId.get(), &SettingBase::valueChanged, this, &ImageSaver::onChange);
  }

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<SettingLineEdit<std::string>> mImageNamePrefix;
  std::unique_ptr<SettingComboBoxMultiClassificationIn> classesIn;
  std::unique_ptr<SettingComboBox<settings::ImageSaverSettings::Style>> style;
  std::unique_ptr<SettingComboBox<settings::ImageSaverSettings::Canvas>> canvas;
  std::unique_ptr<SettingComboBox<bool>> boundingBox;
  std::unique_ptr<SettingComboBox<bool>> objectId;

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
      classsObj.paintBoundingBox = boundingBox->getValue();
      classsObj.paintObjectId    = objectId->getValue();
      mSettings.classesIn.emplace_back(classsObj);
      colorIdx++;
    }
  }
};

}    // namespace joda::ui::gui
