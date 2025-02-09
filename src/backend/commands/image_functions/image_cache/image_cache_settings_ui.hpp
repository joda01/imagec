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
#include "backend/commands/command.hpp"
#include "backend/commands/image_functions/image_cache/image_cache_settings.hpp"
#include "backend/enums/enum_memory_idx.hpp"
#include "ui/container/command/command.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_line_edit.hpp"
#include "ui/helper/icon_generator.hpp"
#include "ui/helper/layout_generator.hpp"
#include "image_cache.hpp"

namespace joda::ui {

class ImageCache : public Command
{
public:
  /////////////////////////////////////////////////////
  inline static std::string TITLE = "Store image to cache";
  inline static std::string ICON  = "copy";

  ImageCache(joda::settings::PipelineStep &pipelineStep, settings::ImageCacheSettings &settings, QWidget *parent) :
      Command(pipelineStep, TITLE.data(), ICON.data(), parent, {{InOuts::IMAGE}, {InOuts::IMAGE}})
  {
    //
    //
    //
    mMemoryIdx = SettingBase::create<SettingComboBox<enums::MemoryIdx::Enum>>(parent, generateIcon("matrix"), "Cache");
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

    addSetting({{mMemoryIdx.get(), true, 0}});
  }

private:
  /////////////////////////////////////////////////////
  std::shared_ptr<SettingComboBox<enums::MemoryIdx::Enum>> mMemoryIdx;
};

}    // namespace joda::ui
