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
class SettingComboBoxMultiClassificationUnmanaged : public SettingBase
{
public:
  struct ComboEntry
  {
    joda::enums::ClassId key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    joda::enums::ClassId key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(joda::enums::ClassId defaultVal);
  void reset() override;
  void clear() override;

  QString getName(joda::enums::ClassId key) const;
  settings::ObjectInputClassesExp getValue();
  std::map<joda::enums::ClassId, std::pair<std::string, std::string>> getValueAndNames();

  void selectAll()
  {
    mComboBox->checkAll();
  }
  void addOptions(const std::map<joda::enums::ClassId, QString> &dataIn);
  void setValue(const settings::ObjectInputClassesExp &valueIn);

  void connectWithSetting(settings::ObjectInputClassesExp *setting)
  {
    mSetting = setting;
  }

  static uint32_t toInt(const joda::enums::ClassId &in)
  {
    return (static_cast<uint16_t>(in)) & 0x0000FFFF;
  }

  static joda::enums::ClassId fromInt(uint32_t in)
  {
    return static_cast<joda::enums::ClassId>(in & 0x0000FFFF);
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mComboBox) {
      mComboBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::optional<joda::enums::ClassId> mDefaultValue;
  QComboBoxMulti *mComboBox;
  settings::ObjectInputClassesExp *mSetting = nullptr;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui::gui
