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
class SettingComboBoxClassesOut : public SettingBase
{
public:
  struct ComboEntry
  {
    enums::ClassId key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    enums::ClassId key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override;
  void setDefaultValue(enums::ClassId defaultVal);
  void reset() override;
  void clear() override;

  void clusterNamesChanged() override;
  QString getName(enums::ClassId key) const;
  enums::ClassId getValue();
  std::pair<enums::ClassId, std::string> getValueAndNames();

  void setValue(const enums::ClassId &valueIn);

  void connectWithSetting(enums::ClassId *setting)
  {
    mSetting = setting;
  }

private:
  /////////////////////////////////////////////////////
  std::optional<enums::ClassId> mDefaultValue;
  QComboBox *mComboBox;
  enums::ClassId *mSetting = nullptr;

  static uint32_t toInt(const enums::ClassId &in)
  {
    return static_cast<uint32_t>(in);
  }

  static enums::ClassId fromInt(uint32_t in)
  {
    return static_cast<enums::ClassId>(in);
  }

private slots:
  void onValueChanged();
};

}    // namespace joda::ui
