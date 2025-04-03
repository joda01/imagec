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
#include <optional>
#include "ui/gui/helper/clickablelineedit.hpp"
#include "setting_base.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author
/// \brief
///
template <NumberOrString VALUE_T>
class SettingLineEdit : public SettingBase
{
public:
  SettingLineEdit(QWidget *parent, const QIcon &icon, const QString &description, int32_t maxTextLengthToDisplay = 100) :
      SettingBase(parent, icon, description, maxTextLengthToDisplay)
  {
    if constexpr(std::same_as<VALUE_T, int>) {
      mEmptyValue = -1;
    }
    if constexpr(std::same_as<VALUE_T, uint32_t>) {
      mEmptyValue = 0;
    }
    if constexpr(std::same_as<VALUE_T, uint16_t>) {
      mEmptyValue = 0;
    }
    if constexpr(std::same_as<VALUE_T, float>) {
      mEmptyValue = -1;
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      mEmptyValue = "";
    }
  }

  QWidget *createInputObject() override
  {
    mLineEdit = new ClickableLineEdit();
    // mLineEdit->setClearButtonEnabled(true);
    if(!getIcon().isNull()) {
      mLineEdit->addAction(getIcon().pixmap(TXT_ICON_SIZE, TXT_ICON_SIZE), QLineEdit::LeadingPosition);
    }
    connect(mLineEdit, &QLineEdit::editingFinished, this, &SettingLineEdit::onValueChanged);
    // connect(mLineEdit, &QLineEdit::returnPressed, this, &SettingLineEdit::onValueChanged);
    //  connect(mLineEdit, &QLineEdit::textChanged, this, &SettingLineEdit::onValueChanged);
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

  /// \todo Implement valifator
  void setMinMax(VALUE_T min, VALUE_T max)
    requires Number_t<VALUE_T>
  {
    // If the textfield is empty, no signal was emitted
    /*
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

        auto regexp    = QtCore.QRegExp('(^[0-9]+$|^$)');
        auto validator = QtGui.QRegExpValidator(regexp);
        mLineEdit->setValidator(validator);
        */
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
        return mEmptyValue;
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
        return mEmptyValue;
      }
      return mLineEdit->text().toFloat();
    }
    if constexpr(std::same_as<VALUE_T, std::string>) {
      return mLineEdit->text().toStdString();
    }
  }

  void setEmptyValue(VALUE_T val)
  {
    mEmptyValue = val;
  }

  void setValue(VALUE_T value)
  {
    mLineEdit->blockSignals(true);
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
    onValueChanged();
    mLineEdit->blockSignals(false);
  }

  void connectWithSetting(VALUE_T *setting)
  {
    mSetting = setting;
  }

  void blockComponentSignals(bool bl) override
  {
    if(nullptr != mLineEdit) {
      mLineEdit->blockSignals(bl);
    }
  }

private:
  /////////////////////////////////////////////////////
  ClickableLineEdit *mLineEdit = nullptr;
  std::optional<VALUE_T> mDefaultValue;
  VALUE_T *mSetting = nullptr;
  VALUE_T mEmptyValue;

private slots:
  void onValueChanged()
  {
    bool hasValueChanged = true;
    if(mSetting != nullptr) {
      hasValueChanged = *mSetting != getValue();
      *mSetting       = getValue();
    }
    triggerValueChanged(mLineEdit->text(), hasValueChanged);
  }
};

}    // namespace joda::ui::gui
