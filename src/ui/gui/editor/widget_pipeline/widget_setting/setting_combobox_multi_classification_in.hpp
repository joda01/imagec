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
class SettingComboBoxMultiClassificationIn : public SettingBase
{
public:
  struct ComboEntry
  {
    joda::enums::ClassIdIn key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    joda::enums::ClassIdIn key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(joda::enums::ClassIdIn defaultVal);
  void reset() override;
  void clear() override;

  void classsNamesChanged() override;
  void outputClassesChanges() override;
  QString getName(joda::enums::ClassIdIn key) const;
  settings::ObjectInputClasses getValue();
  std::map<joda::enums::ClassIdIn, std::string> getValueAndNames();

  void selectAll()
  {
    mComboBox->checkAll();
  }

  void setValue(const settings::ObjectInputClasses &valueIn);

  void connectWithSetting(settings::ObjectInputClasses *setting)
  {
    mSetting = setting;
  }

  static uint32_t toInt(const joda::enums::ClassIdIn &in)
  {
    return (static_cast<uint16_t>(in)) & 0x0000FFFF;
  }

  static uint32_t toInt(const joda::enums::ClassId &in)
  {
    return (static_cast<uint16_t>(in)) & 0x0000FFFF;
  }

  static joda::enums::ClassIdIn fromInt(uint32_t in)
  {
    return static_cast<joda::enums::ClassIdIn>(in);
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mComboBox) {
      mComboBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::optional<joda::enums::ClassIdIn> mDefaultValue;
  QComboBoxMulti *mComboBox;
  settings::ObjectInputClasses *mSetting = nullptr;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui::gui
