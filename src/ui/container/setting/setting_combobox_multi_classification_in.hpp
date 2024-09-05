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
#include "ui/helper/multicombobox.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui {

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
    settings::ClassificatorSetting key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    settings::ClassificatorSetting key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(settings::ClassificatorSetting defaultVal);
  void reset() override;
  void clear() override;

  void clusterNamesChanged() override;
  void outputClustersChanges() override;
  QString getName(settings::ClassificatorSetting key) const;
  settings::ObjectInputClusters getValue();
  std::map<settings::ClassificatorSetting, std::string> getValueAndNames();

  void selectAll()
  {
    mComboBox->checkAll();
  }

  void setValue(const settings::ObjectInputClusters &valueIn);

  void connectWithSetting(settings::ObjectInputClusters *setting)
  {
    mSetting = setting;
  }

  static uint32_t toInt(const settings::ClassificatorSetting &in)
  {
    return ((((uint16_t) in.clusterId) & 0x0000FFFF) << 16) | ((uint16_t) (in.classId)) & 0x0000FFFF;
  }

  static uint32_t toInt(const settings::ClassificatorSettingOut &in)
  {
    return ((((uint16_t) in.clusterId) & 0x0000FFFF) << 16) | ((uint16_t) (in.classId)) & 0x0000FFFF;
  }

  static settings::ClassificatorSetting fromInt(uint32_t in)
  {
    settings::ClassificatorSetting out;
    out.clusterId = static_cast<joda::enums::ClusterIdIn>((in >> 16) & 0x0000FFFF);
    out.classId   = static_cast<joda::enums::ClassId>(in & 0x0000FFFF);
    return out;
  }

private:
  /////////////////////////////////////////////////////
  std::optional<settings::ClassificatorSetting> mDefaultValue;
  QComboBoxMulti *mComboBox;
  settings::ObjectInputClusters *mSetting = nullptr;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui
