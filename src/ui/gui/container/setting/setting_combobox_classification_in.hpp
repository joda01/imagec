///
/// \file      setting_line_Edit.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-22
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qcombobox.h>
#include <qnamespace.h>
#include <iostream>
#include <optional>
#include <set>
#include "backend/enums/enums_classes.hpp"
#include "backend/settings/setting.hpp"
#include "backend/settings/settings_types.hpp"
#include "ui/gui/helper/multicombobox.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author
/// \brief
///
class SettingComboBoxClassificationIn : public SettingBase
{
public:
  struct ComboEntry
  {
    enums::ClassIdIn key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    enums::ClassIdIn key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(enums::ClassIdIn defaultVal);
  void reset() override;
  void clear() override;

  void classsNamesChanged() override;
  void outputClassesChanges() override;
  QString getName(enums::ClassIdIn key) const;
  settings::ObjectInputClasss getValue();
  std::map<enums::ClassIdIn, std::string> getValueAndNames();

  void setValue(const settings::ObjectInputClasss &valueIn);

  void connectWithSetting(settings::ObjectInputClasss *setting)
  {
    mSetting = setting;
  }

  static uint32_t toInt(const enums::ClassIdIn &in)
  {
    return (static_cast<uint16_t>(in));
  }

  static uint32_t toInt(const enums::ClassId &in)
  {
    return (static_cast<uint16_t>(in));
  }

  static enums::ClassIdIn fromInt(uint32_t in)
  {
    return static_cast<enums::ClassIdIn>(in);
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mComboBox) {
      mComboBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::optional<enums::ClassIdIn> mDefaultValue;
  QComboBox *mComboBox;
  settings::ObjectInputClasss *mSetting = nullptr;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui::gui
