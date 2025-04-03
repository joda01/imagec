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
#include "ui/gui/helper/icon_generator.hpp"
#include "setting_base.hpp"
#include "setting_combobox.hpp"

namespace joda::ui::gui {

template <typename T>
concept ClassId_t = std::same_as<T, enums::ClassIdIn> || std::same_as<T, enums::ClassId>;

///
/// \class
/// \author
/// \brief
///
template <ClassId_t CLASSID>
class SettingComboBoxClassesOutTemplate : public SettingBase
{
public:
  struct ComboEntry
  {
    CLASSID key;
    QString label;
    QString icon;
  };

  struct ComboEntryText
  {
    CLASSID key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mComboBox = new QComboBox();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mComboBox->addAction(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE), "");

    classsNamesChanged();

    SettingBase::connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxClassesOutTemplate::onValueChanged);
    // SettingBase::connect(mComboBox, &QComboBox::currentTextChanged, this, &SettingComboBoxClassesOutTemplate::onValueChanged);

    return mComboBox;
  }
  void setDefaultValue(CLASSID defaultVal)
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

  void addOptions(const std::map<CLASSID, QString> &dataIn)
  {
    mComboBox->blockSignals(true);
    auto actSelected = getValue();
    mComboBox->clear();

    std::map<std::string, std::multimap<std::string, CLASSID>> orderedClasses;
    for(const auto &[id, className] : dataIn) {
      orderedClasses[enums::getPrefixFromClassName(className.toStdString())].emplace(className.toStdString(), id);
    }

    for(const auto &[prefix, group] : orderedClasses) {
      for(const auto &[className, id] : group) {
        QVariant variant;
        variant = QVariant(toInt(id));
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), className.data(), variant);
      }
      mComboBox->insertSeparator(mComboBox->count());
    }

    auto removeLastSeparator = [this]() {
      int lastIndex = mComboBox->count() - 1;
      if(lastIndex >= 0) {
        mComboBox->removeItem(lastIndex);
      }
    };
    removeLastSeparator();
    setValue(actSelected);
    mComboBox->blockSignals(false);
  }

  void classsNamesChanged() override
  {
    auto *parent = getParent();
    if(parent != nullptr) {
      mComboBox->blockSignals(true);
      auto actSelected = getValue();
      mComboBox->clear();
      auto classes = getClasses();

      std::map<std::string, std::multimap<std::string, CLASSID>> orderedClasses;
      for(const auto &[id, className] : classes) {
        orderedClasses[enums::getPrefixFromClassName(className.toStdString())].emplace(className.toStdString(), (CLASSID) id);
      }

      if constexpr(std::same_as<CLASSID, enums::ClassIdIn>) {
        // Add this classs
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Default",
                           QVariant(toInt(enums::ClassIdIn::$)));

        // Add Temp
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 01",
                           QVariant(toInt(enums::ClassIdIn::TEMP_01)));
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 02",
                           QVariant(toInt(enums::ClassIdIn::TEMP_02)));
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 03",
                           QVariant(toInt(enums::ClassIdIn::TEMP_03)));
        mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), "Memory 04",
                           QVariant(toInt(enums::ClassIdIn::TEMP_04)));
        mComboBox->insertSeparator(mComboBox->count());
      }
      for(const auto &[prefix, group] : orderedClasses) {
        for(const auto &[className, id] : group) {
          QVariant variant;
          variant = QVariant(toInt(id));
          if constexpr(std::same_as<CLASSID, enums::ClassIdIn>) {
            if(((enums::ClassIdIn) id != enums::ClassIdIn::$) &&
               ((enums::ClassIdIn) id < enums::ClassIdIn::TEMP_01 || (enums::ClassIdIn) id > enums::ClassIdIn::TEMP_LAST)) {
              if(!SettingBase::getIcon().isNull()) {
                mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), className.data(),
                                   variant);
              } else {
                mComboBox->addItem(generateIcon("circle"), className.data(), variant);
              }
            }
          } else {
            if(!SettingBase::getIcon().isNull()) {
              mComboBox->addItem(QIcon(SettingBase::getIcon().pixmap(SettingBase::TXT_ICON_SIZE, SettingBase::TXT_ICON_SIZE)), className.data(),
                                 variant);
            } else {
              mComboBox->addItem(generateIcon("circle"), className.data(), variant);
            }
          }
        }
        mComboBox->insertSeparator(mComboBox->count());
      }

      auto removeLastSeparator = [this]() {
        int lastIndex = mComboBox->count() - 1;
        if(lastIndex >= 0) {
          mComboBox->removeItem(lastIndex);
        }
      };
      removeLastSeparator();
      auto idx = mComboBox->findData(toInt(actSelected));
      if(idx >= 0) {
        (mComboBox)->setCurrentIndex(idx);
      }
      SettingBase::triggerValueChanged(mComboBox->currentText(), false);
      mComboBox->blockSignals(false);
    }
  }
  QString getName(CLASSID key) const
  {
    auto idx = mComboBox->findData(toInt(key), Qt::UserRole + 1);
    if(idx >= 0) {
      return mComboBox->itemText(idx);
    }
    return "";
  }
  CLASSID getValue()
  {
    return fromInt(mComboBox->currentData().toUInt());
  }
  std::pair<CLASSID, std::string> getValueAndNames()
  {
    return {fromInt(mComboBox->currentData().toUInt()), mComboBox->currentText().toStdString()};
  }

  void setValue(const CLASSID &valueIn)
  {
    mComboBox->blockSignals(true);
    auto idx = mComboBox->findData(toInt(valueIn));
    if(idx >= 0) {
      (mComboBox)->setCurrentIndex(idx);
    }
    onValueChanged();
    mComboBox->blockSignals(false);
  }

  void connectWithSetting(CLASSID *setting)
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
  std::optional<CLASSID> mDefaultValue;
  QComboBox *mComboBox;
  CLASSID *mSetting = nullptr;

  static uint16_t toInt(const CLASSID &in)
  {
    return static_cast<uint16_t>(in);
  }

  static CLASSID fromInt(uint16_t in)
  {
    return static_cast<CLASSID>(in & 0xFFFF);
  }

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
      SettingBase::triggerValueChanged(mComboBox->currentText(), hasValueChanged, selectedIcon);
    } else {
      SettingBase::triggerValueChanged(mComboBox->currentText(), hasValueChanged);
    }
  }
};

using SettingComboBoxClassesOut  = SettingComboBoxClassesOutTemplate<enums::ClassIdIn>;
using SettingComboBoxClassesOutN = SettingComboBoxClassesOutTemplate<enums::ClassId>;

}    // namespace joda::ui::gui
