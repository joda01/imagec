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

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
class SettingComboBoxString : public SettingBase
{
public:
  struct ComboEntry
  {
    std::string key;
    QString label;
    QIcon icon;
  };

  struct ComboEntryText
  {
    std::string key;
    QString label;
  };

  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mComboBox = new QComboBox();
    mComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    mComboBox->addAction(getIcon(), "");

    connect(mComboBox, &QComboBox::currentIndexChanged, this, &SettingComboBoxString::onValueChanged);

    return mComboBox;
  }

  void setDefaultValue(std::string defaultVal)
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
    for(const auto &data : options) {
      QVariant variant;
      variant = QVariant(data.key.data());
      if(data.icon.isNull()) {
        mComboBox->addItem(getIcon(), data.label, variant);
      } else {
        mComboBox->addItem(data.icon, data.label, variant);
      }
    }
  }

  void changeOptionText(const std::map<std::string, QString> &options)
  {
    auto findItem = [this](std::string key) -> int {
      int count = mComboBox->count();
      for(int i = 0; i < count; ++i) {
        auto item = mComboBox->itemData(i);
        if(QString(key.data()) == item.toString()) {
          return i;
        }
      }
      return -1;
    };

    auto act = getValue();
    mComboBox->clear();
    for(const auto &[key, label] : options) {
      QVariant variant;
      variant = QVariant(key.data());
      mComboBox->addItem(QIcon(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE)), label, variant);
    }
    setValue(act);
  }

  std::string getValue()
  {
    return mComboBox->currentData().toString().toStdString();
  }

  void setValue(std::string value)
  {
    mComboBox->blockSignals(true);

    int idx = mComboBox->findData(QString(value.data()));

    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    } else {
      mComboBox->setCurrentIndex(0);
    }
    onValueChanged();
    mComboBox->blockSignals(false);
  }

  void connectWithSetting(std::string *setting)
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
  std::optional<std::string> mDefaultValue;
  QComboBox *mComboBox  = nullptr;
  std::string *mSetting = nullptr;

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

}    // namespace joda::ui
