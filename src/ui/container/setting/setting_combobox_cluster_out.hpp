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
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
class SettingComboBoxClusterOut : public SettingBase
{
public:
  struct ComboEntry
  {
    enums::ClusterIdIn key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    enums::ClusterIdIn key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(enums::ClusterIdIn defaultVal);
  void reset() override;
  void clear() override;

  void clusterNamesChanged() override;
  QString getName(enums::ClusterIdIn key) const;
  enums::ClusterIdIn getValue();
  std::pair<enums::ClusterIdIn, std::string> getValueAndNames();

  void setValue(const enums::ClusterIdIn &valueIn);

  void connectWithSetting(enums::ClusterIdIn *setting)
  {
    mSetting = setting;
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mComboBox) {
      mComboBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  std::optional<enums::ClusterIdIn> mDefaultValue;
  QComboBox *mComboBox;
  enums::ClusterIdIn *mSetting = nullptr;

  static uint32_t toInt(const enums::ClusterIdIn &in)
  {
    return static_cast<uint32_t>(in);
  }

  static enums::ClusterIdIn fromInt(uint32_t in)
  {
    return static_cast<enums::ClusterIdIn>(in);
  }

private slots:
  void onValueChanged();
};

}    // namespace joda::ui
