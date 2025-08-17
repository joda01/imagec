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
#include "ui/gui/helper/multicombobox.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui::gui {

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
    mComboBox = new QComboBoxMulti();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

    // SettingBase::connect(mComboBox, &QComboBoxMulti::currentIndexChanged, this, &SettingComboBoxMulti::onValueChanged);
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
    for(const auto &dataLoop : options) {
      QVariant variant;
      if constexpr(std::is_enum<VALUE_T>::value) {
        variant = QVariant(static_cast<int>(dataLoop.key));
      } else {
        variant = QVariant(dataLoop.key);
      }
      if(dataLoop.icon.isNull()) {
        mComboBox->addItem(SettingBase::getIcon(), dataLoop.label, variant);
      } else {
        mComboBox->addItem(dataLoop.icon, dataLoop.label, variant);
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

    for(const auto &[dataC, _] : checked) {
      if constexpr(std::same_as<VALUE_T, int32_t>) {
        toReturn.emplace(dataC.toInt());
      }
      if constexpr(std::same_as<VALUE_T, uint32_t>) {
        toReturn.emplace(dataC.toUInt());
      }
      if constexpr(std::same_as<VALUE_T, uint16_t>) {
        toReturn.emplace(dataC.toUInt());
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        toReturn.emplace(dataC.toFloat());
      }
      if constexpr(std::same_as<VALUE_T, bool>) {
        toReturn.emplace(dataC.toBool());
      }
      if constexpr(std::is_enum<VALUE_T>::value) {
        toReturn.emplace((VALUE_T) dataC.toInt());
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
    mComboBox->blockSignals(true);
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
    onValueChanged();
    mComboBox->blockSignals(false);
  }

  void connectWithSetting(std::set<VALUE_T> *setting)
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
  QComboBoxMulti *mComboBox;
  std::set<VALUE_T> *mSetting = nullptr;

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
      SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), hasValueChanged, selectedIcon);
    } else {
      SettingBase::triggerValueChanged(((QComboBoxMulti *) mComboBox)->getDisplayText(), hasValueChanged);
    }
  }
};

}    // namespace joda::ui::gui
