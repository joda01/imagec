///
/// \file      setting_line_Edit.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qglobal.h>
#include <optional>
#include "backend/enums/types.hpp"
#include "ui/helper/clickablelineedit.hpp"
#include "ui/helper/color_picker/color_picker.hpp"
#include "setting_base.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
class SettingColorPicker : public SettingBase
{
public:
  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mColorPicker = new ColorPicker();
    connect(mColorPicker, &ColorPicker::valueChanged, this, &SettingColorPicker::onValueChanged);

    return mColorPicker;
  }

  void reset() override
  {
    setValue({{}, {}, {}});
  }

  void clear() override
  {
    setValue({{}, {}, {}});
  }

  auto getValue() -> std::tuple<joda::enums::HsvColor, joda::enums::HsvColor, joda::enums::HsvColor>
  {
    return mColorPicker->getValue();
  }

  void setValue(const std::tuple<joda::enums::HsvColor, joda::enums::HsvColor, joda::enums::HsvColor> &fromTo)
  {
    mColorPicker->setValue(fromTo);
  }

  void connectWithSetting(joda::enums::HsvColor *a, joda::enums::HsvColor *b, joda::enums::HsvColor *c)
  {
    mSettingsA = a;
    mSettingsB = b;
    mSettingsC = c;
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mColorPicker) {
      mColorPicker->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  joda::ui::ColorPicker *mColorPicker;
  joda::enums::HsvColor *mSettingsA = nullptr;
  joda::enums::HsvColor *mSettingsB = nullptr;
  joda::enums::HsvColor *mSettingsC = nullptr;

private slots:
  void onValueChanged()
  {
    if(mSettingsA != nullptr && mSettingsB != nullptr && mSettingsC != nullptr) {
      auto [a, b, c] = mColorPicker->getValue();
      *mSettingsA    = a;
      *mSettingsB    = b;
      *mSettingsC    = c;
    }
    triggerValueChanged("From-To");
  }
};

}    // namespace joda::ui
