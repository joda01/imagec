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

#include <QComboBox>
#include <iostream>
#include <optional>
#include <string>
#include "setting_base.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author
/// \brief
///
template <NumberOrEnum_t VALUE_T>
class SettingComboBox : public SettingBase
{
public:
  struct ComboEntry
  {
    VALUE_T key;
    QString label;
    QIcon icon;
  };

  struct ComboEntryText
  {
    VALUE_T key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mComboBox = new QComboBox();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mComboBox->addAction(getIcon(), "");

    connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBox::onValueChanged);

    return mComboBox;
  }

  void setDefaultValue(VALUE_T defaultVal)
  {
    mDefaultValue = defaultVal;
    reset();
  }

  void reset() override
  {
    if(mDefaultValue.has_value()) {
      setValue(mDefaultValue.value());
    }
  }

  void clear() override
  {
    mComboBox->setCurrentIndex(0);
  }

  void addOptions(const std::vector<ComboEntry> &options)
  {
    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();

    for(const auto &data : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else {
        variant = QVariant(data.key);
      }
      if(data.icon.isNull()) {
        mComboBox->addItem(getIcon(), data.label, variant);
      } else {
        mComboBox->addItem(data.icon, data.label, variant);
      }
    }
    setValue(actSelected);
    mComboBox->blockSignals(false);
  }

  void changeOptionText(const std::map<VALUE_T, QString> &options)
  {
    auto findItem = [this](VALUE_T key) -> int {
      int count = mComboBox->count();
      for(int i = 0; i < count; ++i) {
        auto item = mComboBox->itemData(i);
        if constexpr(std::same_as<VALUE_T, int32_t>) {
          if(key == item.toInt()) {
            return i;
          }
        }
        if constexpr(std::same_as<VALUE_T, uint32_t>) {
          if(key == item.toUInt()) {
            return i;
          }
        }
        if constexpr(std::same_as<VALUE_T, uint16_t>) {
          if(key == item.toUInt()) {
            return i;
          }
        }
        if constexpr(std::same_as<VALUE_T, float>) {
          if(key == item.toFloat()) {
            return i;
          }
        }
        if constexpr(std::same_as<VALUE_T, bool>) {
          if(key == item.toBool()) {
            return i;
          }
        }
        if constexpr(std::is_enum<VALUE_T>::value) {
          if(key == (VALUE_T) item.toInt()) {
            return i;
          }
        }
      }
      return -1;
    };

    auto act = getValue();
    mComboBox->clear();
    for(const auto &[key, label] : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(key));
      } else {
        variant = QVariant(key);
      }
      mComboBox->addItem(QIcon(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), label, variant);
    }
    setValue(act);
  }

  VALUE_T getValue()
  {
    if constexpr(std::same_as<VALUE_T, int32_t>) {
      return mComboBox->currentData().toInt();
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      return mComboBox->currentData().toUInt();
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      return mComboBox->currentData().toUInt();
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      return mComboBox->currentData().toFloat();
    }
    if constexpr(std::same_as<VALUE_T, bool>) {
      return mComboBox->currentData().toBool();
    }
    if constexpr(std::is_enum<VALUE_T>::value) {
      return (VALUE_T) mComboBox->currentData().toInt();
    }
  }

  void setValue(VALUE_T value)
  {
    mComboBox->blockSignals(true);
    int idx = -1;
    if constexpr(std::same_as<VALUE_T, int32_t>) {
      idx = mComboBox->findData(static_cast<int>(value));
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      idx = mComboBox->findData(static_cast<int>(value));
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      idx = mComboBox->findData(static_cast<int>(value));
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      idx = mComboBox->findData(static_cast<float>(value));
    }
    if constexpr(std::same_as<VALUE_T, bool>) {
      idx = mComboBox->findData(static_cast<int>(value));
    }
    if constexpr(std::is_enum<VALUE_T>::value) {
      idx = mComboBox->findData(static_cast<int>(value));
    }

    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    } else {
      mComboBox->setCurrentIndex(0);
    }

    onValueChanged();
    mComboBox->blockSignals(false);
  }

  void connectWithSetting(VALUE_T *setting)
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
  std::optional<VALUE_T> mDefaultValue;
  QComboBox *mComboBox = nullptr;
  VALUE_T *mSetting    = nullptr;

private slots:
  void onValueChanged()
  {
    bool hasValueChanged = true;
    if(mSetting != nullptr) {
      hasValueChanged = *mSetting != getValue();
      *mSetting       = getValue();
    }
    QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
    if(itemData.isValid() && itemData.canConvert<QIcon>()) {
      auto selectedIcon = qvariant_cast<QIcon>(itemData);
      triggerValueChanged(mComboBox->currentText(), hasValueChanged, selectedIcon);
    } else {
      triggerValueChanged(mComboBox->currentText(), hasValueChanged);
    }
  }
};

}    // namespace joda::ui::gui
