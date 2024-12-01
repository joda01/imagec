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
#include <qspinbox.h>
#include <cstdint>
#include <optional>
#include "ui/helper/clickablelineedit.hpp"
#include "setting_base.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
template <Number_t VALUE_T>
class SettingSpinBox : public SettingBase
{
public:
  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mSpinBox = new QSpinBox();
    // mLineEdit->setClearButtonEnabled(true);
    // if(!getIcon().isNull()) {
    //  mSpinBox->addAction(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE), QLineEdit::LeadingPosition);
    //}
    connect(mSpinBox, &QSpinBox::editingFinished, this, &SettingSpinBox::onValueChanged);
    // connect(mSpinBox, &QSpinBox::valueChanged, this, &SettingSpinBox::onValueChanged);
    //   connect(mLineEdit, &QLineEdit::textChanged, this, &SettingSpinBox::onValueChanged);
    return mSpinBox;
  }

  QSpinBox *getLineEdit()
  {
    return mSpinBox;
  }

  void setDefaultValue(VALUE_T defaultVal)
  {
    mDefaultValue = defaultVal;
    reset();
  }

  void setMinMax(VALUE_T min, VALUE_T max)
    requires Number_t<VALUE_T>
  {
    if(mSpinBox != nullptr) {
      mSpinBox->setMinimum(min);
      mSpinBox->setMaximum(max);
    }
  }

  void reset() override
  {
    if(mDefaultValue.has_value()) {
      if constexpr(Number_t<VALUE_T>) {
        mSpinBox->setValue(mDefaultValue.value());
      } else {
        mSpinBox->setValue(mDefaultValue.value().data());
      }
    }
  }

  void clear() override
  {
    mSpinBox->clear();
  }

  VALUE_T getValue()
  {
    if constexpr(std::same_as<VALUE_T, int>) {
      if(mSpinBox->text().isEmpty()) {
        return -1;
      }
      return mSpinBox->value();
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      if(mSpinBox->text().isEmpty()) {
        return 0;
      }
      return mSpinBox->value();
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      if(mSpinBox->text().isEmpty()) {
        return 0;
      }
      return mSpinBox->value();
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      if(mSpinBox->text().isEmpty()) {
        return -1;
      }
      return mSpinBox->value();
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      return mSpinBox->value();
    }
  }

  void setValue(VALUE_T value)
  {
    mSpinBox->blockSignals(true);
    if constexpr(std::same_as<VALUE_T, int>) {
      if(value >= 0) {
        mSpinBox->setValue(value);
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      if(value >= 0) {
        mSpinBox->setValue(value);
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      if(value >= 0) {
        mSpinBox->setValue(value);
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      if(value >= 0) {
        mSpinBox->setValue(value);
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      mSpinBox->setValue(value);
    }
    onValueChanged();
    mSpinBox->blockSignals(false);
  }

  void connectWithSetting(VALUE_T *setting)
  {
    mSetting = setting;
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mSpinBox) {
      mSpinBox->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  QSpinBox *mSpinBox = nullptr;
  std::optional<VALUE_T> mDefaultValue;
  VALUE_T *mSetting = nullptr;
  std::optional<VALUE_T> mOldValue;

private slots:
  void onValueChanged()
  {
    // Only trigger value changed if old value is not equal to actual value
    if(mOldValue.has_value() && mOldValue == getValue()) {
      return;
    }
    mOldValue            = getValue();
    bool hasValueChanged = true;
    if(mSetting != nullptr) {
      hasValueChanged = *mSetting != getValue();
      *mSetting       = getValue();
    }
    if(mSpinBox != nullptr) {
      triggerValueChanged(QString::number(mSpinBox->value()), hasValueChanged);
    }
  }
};

}    // namespace joda::ui
