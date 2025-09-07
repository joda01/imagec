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

#include <qglobal.h>
#include <qspinbox.h>
#include <cstdint>
#include <optional>
#include <string>
#include "backend/enums/enums_units.hpp"
#include "backend/helper/ome_parser/physical_size.hpp"
#include "ui/gui/helper/clickablelineedit.hpp"
#include "ui/gui/helper/jump_spinbox.hpp"
#include <nlohmann/json_fwd.hpp>
#include "setting_base.hpp"

namespace joda::ui::gui {

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
    mSpinBox = new JumpSpinBox();
    mSpinBox->setMinimumHeight(34);
    mSpinBox->setRange(-1.0, std::numeric_limits<float>::max());
    mSpinBox->setSpecialValueText("(disabled)");
    mSpinBox->setDecimals(3);    // optional, number of decimal places
    // mLineEdit->setClearButtonEnabled(true);
    // if(!getIcon().isNull()) {
    //  mSpinBox->addAction(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE), QLineEdit::LeadingPosition);
    //}
    connect(mSpinBox, &QSpinBox::editingFinished, this, &SettingSpinBox::onValueChanged);
    // connect(mSpinBox, &QSpinBox::valueChanged, this, &SettingSpinBox::onValueChanged);
    //   connect(mLineEdit, &QLineEdit::textChanged, this, &SettingSpinBox::onValueChanged);
    return mSpinBox;
  }

  QDoubleSpinBox *getSpinBox()
  {
    return mSpinBox;
  }

  void setUnit(QString unit, enums::ObjectType objectType)
  {
    mObjectType = objectType;
    if(mObjectType != enums::ObjectType::Undefined) {
      auto [cmdUnit, physicalSize] = getUnit();
      mActUnit                     = cmdUnit;
      nlohmann::json unitJson      = cmdUnit;
      unit                         = unitJson.get<std::string>().c_str();
      switch(cmdUnit) {
        case enums::Units::Pixels:
          mSpinBox->setDecimals(0);
          mSpinBox->setSingleStep(1);
          break;
        case enums::Units::nm:
          mSpinBox->setDecimals(1);
          mSpinBox->setSingleStep(0.1);
          break;
        case enums::Units::mm:
        case enums::Units::um:
          mSpinBox->setDecimals(3);
          mSpinBox->setSingleStep(0.01);
          break;
        case enums::Units::cm:
          mSpinBox->setDecimals(2);
          mSpinBox->setSingleStep(0.1);
          break;
        case enums::Units::m:
          mSpinBox->setDecimals(3);
          mSpinBox->setSingleStep(0.01);
          break;
        case enums::Units::km:
          mSpinBox->setDecimals(2);
          mSpinBox->setSingleStep(0.1);
          break;
        case enums::Units::Undefined:
          break;
      }
    }
    SettingBase::setUnit(unit);
    mSpinBox->setSuffix(" " + unit);
  }

  void changeUnit() override
  {
    if(mObjectType != enums::ObjectType::Undefined) {
      if(mActUnit == enums::Units::Undefined) {
        setUnit("", mObjectType);
        return;
      }
      if(getValue() < 0) {
        return;
      }
      auto [newUnit, physicalSize] = getUnit();
      if(newUnit != mActUnit) {
        double valueInUm     = 0;
        auto [pxX, pxY, pxZ] = physicalSize.getPixelSize(enums::Units::um);
        switch(mActUnit) {
          case enums::Units::Pixels:
            // Not possible
            if(mObjectType == enums::ObjectType::AREA2D) {
              valueInUm = static_cast<double>(static_cast<double>(getValue()) * (pxX * pxY));
            } else {
              valueInUm = static_cast<double>(getValue()) * pxX;
            }
            break;
          case enums::Units::nm:
            valueInUm = static_cast<double>(getValue()) / 1e3;
            break;
          case enums::Units::um:
            valueInUm = static_cast<double>(getValue());
            break;
          case enums::Units::mm:
            valueInUm = static_cast<double>(getValue()) * 1e3;
            break;
          case enums::Units::cm:
            valueInUm = static_cast<double>(getValue()) * 1e4;
            break;
          case enums::Units::m:
            valueInUm = static_cast<double>(getValue()) * 1e6;
            break;
          case enums::Units::km:
            valueInUm = static_cast<double>(getValue()) * 1e9;
            break;
          case enums::Units::Undefined:
            break;
        }

        setUnit("", mObjectType);
        switch(newUnit) {
          case enums::Units::Pixels:
            if(mObjectType == enums::ObjectType::AREA2D) {
              setValue(static_cast<VALUE_T>(static_cast<int32_t>(std::nearbyint(valueInUm / (pxX * pxY)))));
            } else {
              setValue(static_cast<VALUE_T>(static_cast<int32_t>(std::nearbyint(valueInUm / pxX))));
            }
            break;
          case enums::Units::nm:
            setValue(static_cast<VALUE_T>(valueInUm * 1e3));
            break;
          case enums::Units::um:
            setValue(static_cast<VALUE_T>(valueInUm));
            break;
          case enums::Units::mm:
            setValue(static_cast<VALUE_T>(valueInUm / 1e3));
            break;
          case enums::Units::cm:
            setValue(static_cast<VALUE_T>(valueInUm / 1e4));
            break;
          case enums::Units::m:
            setValue(static_cast<VALUE_T>(valueInUm / 1e6));
            break;
          case enums::Units::km:
            setValue(static_cast<VALUE_T>(valueInUm / 1e9));
            break;
          case enums::Units::Undefined:
            break;
        }
      }
    }
  }

  void setDefaultValue(VALUE_T defaultVal)
  {
    mDefaultValue = defaultVal;
    reset();
  }

  void setMinMax(VALUE_T min, VALUE_T max, int32_t precision = 3, double singleStep = 1)
    requires Number_t<VALUE_T>
  {
    if(mSpinBox != nullptr) {
      mSpinBox->setMinimum(min);
      mSpinBox->setMaximum(max);
      mSpinBox->setDecimals(precision);    // optional, number of decimal places
      mSpinBox->setSingleStep(singleStep);
      if(min < 0) {
        mSpinBox->setSpecialValueText("Disabled");
      } else {
        mSpinBox->setSpecialValueText("");
      }
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
    mSpinBox->setValue(mSpinBox->minimum());
  }

  VALUE_T getValue()
  {
    if constexpr(std::same_as<VALUE_T, int>) {
      if(mSpinBox->text().isEmpty()) {
        return -1;
      }
      return static_cast<VALUE_T>(mSpinBox->value());
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      if(mSpinBox->text().isEmpty()) {
        return 0;
      }
      return static_cast<VALUE_T>(mSpinBox->value());
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      if(mSpinBox->text().isEmpty()) {
        return 0;
      }
      return static_cast<VALUE_T>(mSpinBox->value());
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      if(mSpinBox->text().isEmpty()) {
        return -1;
      }
      return static_cast<VALUE_T>(mSpinBox->value());
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      return static_cast<VALUE_T>(mSpinBox->value());
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
  JumpSpinBox *mSpinBox = nullptr;
  std::optional<VALUE_T> mDefaultValue;
  VALUE_T *mSetting = nullptr;
  std::optional<VALUE_T> mOldValue;
  enums::Units mActUnit         = enums::Units::Undefined;
  enums::ObjectType mObjectType = enums::ObjectType::Undefined;

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

}    // namespace joda::ui::gui
