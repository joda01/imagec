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
#include "ui/helper/multicombobox.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui {

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
    settings::ClassificatorSettingOut key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    settings::ClassificatorSettingOut key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(settings::ClassificatorSettingOut defaultVal);
  void reset() override;
  void clear() override;

  QString getName(settings::ClassificatorSettingOut key) const;
  settings::ObjectInputClustersExp getValue();
  std::map<settings::ClassificatorSettingOut, std::pair<std::string, std::string>> getValueAndNames();

  void selectAll()
  {
    mComboBox->checkAll();
  }
  void addOptions(const std::map<settings::ClassificatorSettingOut, QString> &dataIn);
  void setValue(const settings::ObjectInputClustersExp &valueIn);

  void connectWithSetting(settings::ObjectInputClustersExp *setting)
  {
    mSetting = setting;
  }

  static uint32_t toInt(const settings::ClassificatorSettingOut &in)
  {
    return ((((uint16_t) in.clusterId) & 0x0000FFFF) << 16) | ((uint16_t) (in.classId)) & 0x0000FFFF;
  }

  static settings::ClassificatorSettingOut fromInt(uint32_t in)
  {
    settings::ClassificatorSettingOut out;
    out.clusterId = static_cast<joda::enums::ClusterId>((in >> 16) & 0x0000FFFF);
    out.classId   = static_cast<joda::enums::ClassId>(in & 0x0000FFFF);
    return out;
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mComboBox) {
      mComboBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::optional<settings::ClassificatorSettingOut> mDefaultValue;
  QComboBoxMulti *mComboBox;
  settings::ObjectInputClustersExp *mSetting = nullptr;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui
