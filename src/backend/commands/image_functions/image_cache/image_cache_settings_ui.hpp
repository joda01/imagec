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
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_cache/image_cache_settings.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "ui/gui/editor/widget_pipeline/widget_command/command.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_line_edit.hpp"
#include "ui/gui/helper/icon_generator.hpp"
#include "ui/gui/helper/layout_generator.hpp"
#include "image_cache.hpp"

namespace joda::ui::gui {

class ImageCache : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE             = "Image cache";
  inline static std::string ICON              = "edit-paste";
  inline static std::string DESCRIPTION       = "Store or load an image to cache for later use.";
  inline static std::vector<std::string> TAGS = {"cache", "store", "load"};

  ImageCache(joda::settings::PipelineStep &pipelineStep, settings::ImageCacheSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), DESCRIPTION.data(), TAGS, ICON.data(), parent,
              {{InOuts::IMAGE, InOuts::BINARY, InOuts::OBJECT}, {InOuts::OUTPUT_EQUAL_TO_INPUT}})
  {
    //
    //
    //
    mMemoryIdx = SettingBase::create<SettingComboBox<enums::MemoryIdx::Enum>>(parent, generateSvgIcon("labplot-matrix"), "Cache");
    mMemoryIdx->addOptions({{enums::MemoryIdx::M0, "M0"},
                            {enums::MemoryIdx::M1, "M1"},
                            {enums::MemoryIdx::M2, "M2"},
                            {enums::MemoryIdx::M3, "M3"},
                            {enums::MemoryIdx::M4, "M4"},
                            {enums::MemoryIdx::M5, "M5"},
                            {enums::MemoryIdx::M6, "M6"},
                            {enums::MemoryIdx::M7, "M7"},
                            {enums::MemoryIdx::M8, "M8"},
                            {enums::MemoryIdx::M9, "M9"},
                            {enums::MemoryIdx::M10, "M10"}});
    mMemoryIdx->setValue(settings.memoryId);
    mMemoryIdx->connectWithSetting(&settings.memoryId);
    mMemoryIdx->setShortDescription("Cache: ");

    mMemoryScope = SettingBase::create<SettingComboBox<enums::MemoryScope>>(parent, generateSvgIcon("labplot-matrix"), "Storage scope");
    mMemoryScope->addOptions({{enums::MemoryScope::PIPELINE, "Pipeline"}, {enums::MemoryScope::ITERATION, "Iteration"}});
    mMemoryScope->setValue(settings.memoryScope);
    mMemoryScope->connectWithSetting(&settings.memoryScope);
    mMemoryScope->setShortDescription("");

    mMode = SettingBase::create<SettingComboBox<settings::ImageCacheSettings::Mode>>(parent, generateSvgIcon("labplot-matrix"), "Cache");
    mMode->addOptions({
        {settings::ImageCacheSettings::Mode::STORE, "Store"},
        {settings::ImageCacheSettings::Mode::LOAD, "Load"},
    });
    mMode->setValue(settings.mode);
    mMode->connectWithSetting(&settings.mode);
    mMode->setShortDescription("");

    addSetting({{mMemoryIdx.get(), true, 0}, {mMemoryScope.get(), false, 0}, {mMode.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingComboBox<enums::MemoryIdx::Enum>> mMemoryIdx;
  std::shared_ptr<SettingComboBox<enums::MemoryScope>> mMemoryScope;

  std::shared_ptr<SettingComboBox<settings::ImageCacheSettings::Mode>> mMode;
};

}    // namespace joda::ui::gui
