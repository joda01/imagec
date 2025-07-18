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
class SettingComboBoxClassificationUnmanaged : public SettingBase
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
  void setDefaultValue(joda::enums::ClassId defaultVal);
  void reset() override;
  void clear() override;

  QString getName(joda::enums::ClassId key) const;
  joda::enums::ClassId getValue();
  std::pair<joda::enums::ClassId, std::pair<std::string, std::string>> getValueAndNames();

  void setValue(const joda::enums::ClassId &valueIn);
  void addOptions(const std::map<joda::enums::ClassId, QString> &data);

  static uint32_t toInt(const joda::enums::ClassId &in)
  {
    return (static_cast<uint16_t>(in));
  }

  static joda::enums::ClassId fromInt(uint32_t in)
  {
    return static_cast<joda::enums::ClassId>(in);
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
  QComboBox *mComboBox;

private slots:
  void onValueChanged();
};

}    // namespace joda::ui::gui
