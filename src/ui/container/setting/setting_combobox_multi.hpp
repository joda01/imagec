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
#include "ui/helper/multicombobox.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///

template <NumberOrEnum_t VALUE_T>
class SettingComboBoxMulti : public SettingBase
{
public:
  struct ComboEntry
  {
    VALUE_T key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    VALUE_T key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mComboBox = new QComboBoxMulti();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

    SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMulti::onValueChanged);
    SettingBase::connect(mComboBox, &QComboBoxMulti::currentTextChanged, this, &SettingComboBoxMulti::onValueChanged);

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
      setValue({mDefaultValue.value()});
    }
  }

  void clear() override
  {
    mComboBox->setCurrentIndex(0);
  }

  void addOptions(const std::vector<ComboEntry> &options)
  {
    mComboBox->clear();
    for(const auto &data : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(data.key));
      } else {
        variant = QVariant(data.key);
      }
      if(data.icon.isEmpty()) {
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), data.label, variant);
      } else {
        const QIcon myIcon(":/icons/icons/" + data.icon);
        mComboBox->addItem(QIcon(myIcon.pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), data.label, variant);
      }
    }
  }

  void changeOptionText(const std::map<VALUE_T, QString> &options)
  {
    auto findItem = [this](VALUE_T key) -> int {
      int count = mComboBox->count();
      for(int i = 0; i < count; ++i) {
        auto item = mComboBox->itemData(i, Qt::UserRole + 1);
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
    onValueChanged();
  }

  QString getName(VALUE_T key) const
  {
    auto idx = mComboBox->findData(static_cast<int>(key), Qt::UserRole + 1);
    if(idx >= 0) {
      return mComboBox->itemText(idx);
    }
    return "";
  }

  std::set<VALUE_T> getValue()
  {
    std::set<VALUE_T> toReturn;
    auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

    for(const auto &[data, _] : checked) {
      if constexpr(std::same_as<VALUE_T, int32_t>) {
        toReturn.emplace(data.toInt());
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        toReturn.emplace(data.toUInt());
      }
      if constexpr(std::same_as<VALUE_T, uint16_t>) {
        toReturn.emplace(data.toUInt());
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        toReturn.emplace(data.toFloat());
      }
      if constexpr(std::same_as<VALUE_T, bool>) {
        toReturn.emplace(data.toBool());
      }
      if constexpr(std::is_enum<VALUE_T>::value) {
        toReturn.emplace((VALUE_T) data.toInt());
      }
    }

    return toReturn;
  }

  std::map<VALUE_T, std::string> getValueAndNames()
  {
    std::map<VALUE_T, std::string> toReturn;
    auto checked = ((QComboBoxMulti *) mComboBox)->getCheckedItems();

    for(const auto &[data, txt] : checked) {
      if constexpr(std::same_as<VALUE_T, int32_t>) {
        toReturn.emplace(data.toInt(), txt.toStdString());
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        toReturn.emplace(data.toUInt(), txt.toStdString());
      }
      if constexpr(std::same_as<VALUE_T, uint16_t>) {
        toReturn.emplace(data.toUInt(), txt.toStdString());
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        toReturn.emplace(data.toFloat(), txt.toStdString());
      }
      if constexpr(std::same_as<VALUE_T, bool>) {
        toReturn.emplace(data.toBool(), txt.toStdString());
      }
      if constexpr(std::is_enum<VALUE_T>::value) {
        toReturn.emplace((VALUE_T) data.toInt(), txt.toStdString());
      }
    }

    return toReturn;
  }

  void selectAll()
  {
    mComboBox->checkAll();
  }

  void setValue(const std::set<VALUE_T> &valueIn)
  {
    QVariantList toCheck;
    for(const auto &value : valueIn) {
      if constexpr(std::same_as<VALUE_T, int32_t>) {
        toCheck.append(static_cast<int>(value));
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        toCheck.append(static_cast<int>(value));
      }
      if constexpr(std::same_as<VALUE_T, uint16_t>) {
        toCheck.append(static_cast<int>(value));
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        toCheck.append(static_cast<float>(value));
      }
      if constexpr(std::same_as<VALUE_T, bool>) {
        toCheck.append(static_cast<int>(value));
      }
      if constexpr(std::is_enum<VALUE_T>::value) {
        toCheck.append(static_cast<int>(value));
      }
    }
    ((QComboBoxMulti *) mComboBox)->setCheckedItems(toCheck);
  }

  void connectWithSetting(std::set<VALUE_T> *setting)
  {
    mSetting = setting;
  }

private:
  /////////////////////////////////////////////////////
  std::optional<VALUE_T> mDefaultValue;
  QComboBoxMulti *mComboBox;
  std::set<VALUE_T> *mSetting = nullptr;

private slots:
  void onValueChanged()
  {
    if(mSetting != nullptr) {
      *mSetting = getValue();
    }
    QVariant itemData = mComboBox->itemData(mComboBox->currentIndex(), Qt::DecorationRole);
    if(itemData.isValid() && itemData.canConvert<QIcon>()) {
      auto selectedIcon = qvariant_cast<QIcon>(itemData);
      SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), selectedIcon);
    } else {
      SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText());
    }
  }
};

}    // namespace joda::ui
