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
#include <qtextedit.h>
#include <optional>
#include "ui/gui/helper/clickablelineedit.hpp"
#include "setting_base.hpp"

namespace joda::ui::gui {

///
/// \class
/// \author
/// \brief
///
class SettingTextEdit : public SettingBase
{
public:
  using SettingBase::SettingBase;

  QWidget *createInputObject() override
  {
    mLineEdit = new QTextEdit();
    // mLineEdit->setClearButtonEnabled(true);

    connect(mLineEdit, &QTextEdit::textChanged, this, &SettingTextEdit::onValueChanged);
    // connect(mLineEdit, &QLineEdit::returnPressed, this, &SettingTextEdit::onValueChanged);
    //  connect(mLineEdit, &QLineEdit::textChanged, this, &SettingTextEdit::onValueChanged);
    return mLineEdit;
  }

  QTextEdit *getLineEdit()
  {
    return mLineEdit;
  }

  void setPlaceholderText(const QString &placeholderText)
  {
    mLineEdit->setPlaceholderText(placeholderText);
  }

  void setDefaultValue(std::string defaultVal)
  {
    mDefaultValue = defaultVal;
    reset();
  }

  void reset() override
  {
    if(mDefaultValue.has_value()) {
      mLineEdit->setText(mDefaultValue.value().data());
    }
  }

  void clear() override
  {
    mLineEdit->clear();
  }

  std::string getValue()
  {
    return mLineEdit->toMarkdown().toStdString();
  }

  void setValue(std::string value)
  {
    mLineEdit->blockSignals(true);
    mLineEdit->setText(value.data());
    onValueChanged();
    mLineEdit->blockSignals(false);
  }

  void connectWithSetting(std::string *setting)
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
  QTextEdit *mLineEdit = nullptr;
  std::optional<std::string> mDefaultValue;
  std::string *mSetting = nullptr;

private slots:
  void onValueChanged()
  {
    bool hasValueChanged = true;
    if(mSetting != nullptr) {
      hasValueChanged = *mSetting != getValue();
      *mSetting       = getValue();
    }
    triggerValueChanged(mLineEdit->toPlainText(), hasValueChanged);
  }
};

}    // namespace joda::ui::gui
