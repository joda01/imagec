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
#include <optional>
#include "ui/helper/clickablelineedit.hpp"
#include "setting_base.hpp"

namespace joda::ui {

///
/// \class
/// \author
/// \brief
///
template <NumberOrString VALUE_T>
class SettingLineEdit : public SettingBase
{
public:
  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mLineEdit = new ClickableLineEdit();
    // mLineEdit->setClearButtonEnabled(true);
    if(!getIcon().isNull()) {
      mLineEdit->addAction(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE), QLineEdit::LeadingPosition);
    }
    connect(mLineEdit, &QLineEdit::editingFinished, this, &SettingLineEdit::onValueChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &SettingLineEdit::onValueChanged);
    connect(mLineEdit, &QLineEdit::textChanged, this, &SettingLineEdit::onValueChanged);
    return mLineEdit;
  }

  ClickableLineEdit *getLineEdit()
  {
    return mLineEdit;
  }

  void setPlaceholderText(const QString &placeholderText)
  {
    mLineEdit->setPlaceholderText(placeholderText);
  }

  void setMaxLength(int length)
  {
    mLineEdit->setMaxLength(length);
  }

  void setDefaultValue(VALUE_T defaultVal)
  {
    mDefaultValue = defaultVal;
    reset();
  }

  void setMinMax(VALUE_T min, VALUE_T max)
    requires Number_t<VALUE_T>
  {
    QValidator *validator;
    if constexpr(std::same_as<VALUE_T, int>) {
      validator = new QIntValidator(min, max, mLineEdit);
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      validator = new QIntValidator(min, max, mLineEdit);
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      validator = new QIntValidator(min, max, mLineEdit);
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      validator = new QDoubleValidator(min, max, 2, mLineEdit);
      ((QDoubleValidator *) validator)->setLocale(QLocale::C);
    }
    mLineEdit->setValidator(validator);
  }

  void reset() override
  {
    if(mDefaultValue.has_value()) {
      if constexpr(Number_t<VALUE_T>) {
        mLineEdit->setText(QString::number(mDefaultValue.value()));
      } else {
        mLineEdit->setText(mDefaultValue.value().data());
      }
    }
  }

  void clear() override
  {
    mLineEdit->clear();
  }

  VALUE_T getValue()
  {
    if constexpr(std::same_as<VALUE_T, int>) {
      if(mLineEdit->text().isEmpty()) {
        return -1;
      }
      return mLineEdit->text().toInt();
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      if(mLineEdit->text().isEmpty()) {
        return 0;
      }
      return mLineEdit->text().toUInt();
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      if(mLineEdit->text().isEmpty()) {
        return 0;
      }
      return mLineEdit->text().toUShort();
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      if(mLineEdit->text().isEmpty()) {
        return -1;
      }
      return mLineEdit->text().toFloat();
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      return mLineEdit->text().toStdString();
    }
  }

  void setValue(VALUE_T value)
  {
    if constexpr(std::same_as<VALUE_T, int>) {
      if(value >= 0) {
        mLineEdit->setText(QString::number(value));
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      if(value >= 0) {
        mLineEdit->setText(QString::number(value));
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      if(value >= 0) {
        mLineEdit->setText(QString::number(value));
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      if(value >= 0) {
        mLineEdit->setText(QString::number(value));
      } else {
        clear();
      }
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      mLineEdit->setText(value.data());
    }
    // onValueChanged();
  }

  void connectWithSetting(VALUE_T *setting)
  {
    mSetting = setting;
  }

private:
  /////////////////////////////////////////////////////
  ClickableLineEdit *mLineEdit = nullptr;
  std::optional<VALUE_T> mDefaultValue;
  VALUE_T *mSetting = nullptr;

private slots:
  void onValueChanged()
  {
    if(mSetting != nullptr) {
      *mSetting = getValue();
    }
    triggerValueChanged(mLineEdit->text());
  }
};

}    // namespace joda::ui
