///
/// \file      container_function.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-23
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     This container holds the settings for one function
///            It can either be just displayed or edited
///

#pragma once

#include <qcombobox.h>
#include <qlabel.h>
#include <qlocale.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvalidator.h>
#include <qwidget.h>
#include <QSizePolicy>
#include <QtWidgets>
#include <cstddef>
#include <string>
#include "container_function_base.hpp"

namespace joda::ui::qt {

template <typename T>
concept IntFloatConcept =
    std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, bool> || std::same_as<T, QString>;

template <IntFloatConcept VALUE_T>
class ContainerFunction : public ContainerFunctionBase
{
public:
  struct ComboEntry
  {
    VALUE_T key;
    QString label;
  };

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, VALUE_T minVal, VALUE_T maxVal, QWidget *parent = nullptr)

    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>
      : mUnit(unit), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal, minVal, maxVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText,
                    std::optional<VALUE_T> defaultVal, QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString>
      : mUnit(""), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int>
      : mUnit(unit), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, {}, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, const QString &unit,
                    std::optional<VALUE_T> defaultVal, const std::vector<ComboEntry> &options,
                    const std::vector<ComboEntry> &optionsSecond, QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int>
      : mUnit(unit), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, unit, options, optionsSecond, defaultVal);
    comboxEditingFinished();
  }

  ContainerFunction(const QString &icon, const QString &placeHolderText, const QString &helpText, bool defaultVal,
                    QWidget *parent = nullptr)
    requires std::same_as<VALUE_T, bool>
      : mUnit(""), mDefaultValue(defaultVal)
  {
    createDisplayAbleWidget(icon, placeHolderText, helpText);
    createEditableWidget(icon, placeHolderText, helpText, "", {{0, "Off"}, {1, "On"}}, {}, defaultVal);
    comboxEditingFinished();
  }

  QWidget *getLabelWidget()
  {
    return mDisplayable;
  }

  QWidget *getEditableWidget()
  {
    return mEditable;
  }

  bool hasValue()
  {
    if(mLineEdit != nullptr) {
      return !mLineEdit->text().isEmpty();
    }
    if(mComboBox != nullptr) {
      if constexpr(std::same_as<VALUE_T, int>) {
        return mComboBox->currentData().toInt() >= 0;
      }
      if constexpr(std::same_as<VALUE_T, QString>) {
        return mComboBox->currentData().toString() != "NONE";
      }
    }
    return true;
  }

  void resetToDefault()
  {
    if(mDefaultValue.has_value()) {
      setValue(mDefaultValue.value());
    } else {
      if(mComboBox != nullptr) {
        mComboBox->setCurrentIndex(0);
      }
      if(mLineEdit != nullptr) {
        mLineEdit->setText("");
      }
    }
    if(mComboBoxSecond != nullptr) {
      mComboBoxSecond->setCurrentIndex(0);
    }
  }

  void clearValue()
  {
    if(mComboBox != nullptr) {
      mComboBox->setCurrentIndex(0);
    }
    if(mLineEdit != nullptr) {
      mLineEdit->setText("");
    }
    if(mComboBoxSecond != nullptr) {
      mComboBoxSecond->setCurrentIndex(0);
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValue(VALUE_T newValue)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString>
  {
    if(mLineEdit != nullptr) {
      if constexpr(std::same_as<VALUE_T, QString>) {
        mLineEdit->setText(newValue);
      } else {
        mLineEdit->setText(QString::number(newValue));
      }
    }
    if(mComboBox != nullptr) {
      auto idx = mComboBox->findData(newValue);
      if(idx >= 0) {
        mComboBox->setCurrentIndex(idx);
      } else {
        mComboBox->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValueSecond(VALUE_T newValue)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString>
  {
    if(mComboBoxSecond != nullptr) {
      auto idx = mComboBoxSecond->findData(newValue);
      if(idx >= 0) {
        mComboBoxSecond->setCurrentIndex(idx);
      } else {
        mComboBoxSecond->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  void setValue(bool newValue)
    requires std::same_as<VALUE_T, bool>
  {
    if(mComboBox != nullptr) {
      if(newValue) {
        mComboBox->setCurrentIndex(1);
      } else {
        mComboBox->setCurrentIndex(0);
      }
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  int getValue()
    requires std::same_as<VALUE_T, int>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toInt();
      }
      if(mComboBox != nullptr) {
        return mComboBox->currentData().toInt();
      }
      return 0;
    } catch(const std::exception &) {
      return 0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  bool getValue()
    requires std::same_as<VALUE_T, bool>
  {
    try {
      return mComboBox->currentData().toBool();
    } catch(const std::exception &) {
      return false;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  float getValue()
    requires std::same_as<VALUE_T, float>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text().toFloat();
      }
      if(mComboBox != nullptr) {
        return mComboBox->currentData().toFloat();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  QString getValue()
    requires std::same_as<VALUE_T, QString>
  {
    try {
      if(mLineEdit != nullptr) {
        return mLineEdit->text();
      } else if(mComboBox != nullptr) {
        return mComboBox->currentData().toString();
      } else {
        return "";
      }
    } catch(const std::exception &) {
      return "";
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  float getValueSecond()
    requires std::same_as<VALUE_T, float>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toFloat();
      }
      return 0.0;
    } catch(const std::exception &) {
      return 0.0;
    }
  }

  ///
  /// \brief      Creates an editable element
  /// \author     Joachim Danmayr
  /// \return
  ///
  QString getValueSecond()
    requires std::same_as<VALUE_T, QString>
  {
    try {
      if(mComboBoxSecond != nullptr) {
        return mComboBoxSecond->currentData().toString();
      } else {
        return "";
      }
    } catch(const std::exception &) {
      return "";
    }
  }

private:
  /////////////////////////////////////////////////////
  void createDisplayAbleWidget(const QString &icon, const QString &placeHolderText, const QString &helpText)
  {
    mDisplayable = new QWidget();

    // Create a QLabel
    QLabel *label = new QLabel();
    mDisplayLabel = new QLabel();

    // Set text for the label
    mDisplayLabel->setText(mDisplayText);
    mDisplayLabel->setToolTip(helpText);

    // Create a QPixmap for the icon (you may need to adjust the path)
    QIcon bmp(":/icons/outlined/" + icon);

    // Set the icon for the label
    label->setPixmap(bmp.pixmap(16, 16));    // You can adjust the size of the icon as needed
    label->setToolTip(helpText);

    // Create a QHBoxLayout to arrange the text and icon horizontally
    QHBoxLayout *layout = new QHBoxLayout;
    mDisplayable->setLayout(layout);
    layout->addWidget(label);
    layout->addWidget(mDisplayLabel);
    layout->addStretch();
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            std::optional<VALUE_T> defaultVal, VALUE_T min = 0, VALUE_T max = 0)
    requires std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float> || std::same_as<VALUE_T, QString>
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QLineEdit { border-radius: 4px; border: 1px solid rgba(32, 27, 23, 0.6); padding-top: 10px; padding-bottom: "
        "10px;}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layout = new QVBoxLayout(mParent);
    layout->setContentsMargins(8, 8, 8, 0);

    const QIcon myIcon(":/icons/outlined/" + icon);

    mLineEdit = new QLineEdit();
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    mLineEdit->setFont(fontLineEdit);
    mLineEdit->setClearButtonEnabled(true);
    mLineEdit->addAction(myIcon, QLineEdit::LeadingPosition);
    mLineEdit->setPlaceholderText(placeHolderText);
    layout->addWidget(mLineEdit);
    // connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingFinished);
    connect(mLineEdit, &QLineEdit::editingFinished, this, &ContainerFunction::lineEditingChanged);
    connect(mLineEdit, &QLineEdit::returnPressed, this, &ContainerFunction::lineEditingChanged);

    if constexpr(std::same_as<VALUE_T, int> || std::same_as<VALUE_T, float>) {
      QValidator *validator;
      if constexpr(std::same_as<VALUE_T, int>) {
        validator = new QIntValidator(min, max, mLineEdit);
      }
      if constexpr(std::same_as<VALUE_T, float>) {
        validator = new QDoubleValidator(min, max, 2, mLineEdit);
        ((QDoubleValidator *) validator)->setLocale(QLocale::C);
      }
      mLineEdit->setValidator(validator);
      if(defaultVal.has_value()) {
        mLineEdit->setText(QString::number(defaultVal.value()));
      }
    } else {
      if(defaultVal.has_value()) {
        mLineEdit->setText(defaultVal.value());
      }
    }
    lineEditingChanged();

    createHelperText(layout, helpText);
    mEditable->setLayout(layout);
  }

  void createEditableWidget(const QString &icon, const QString &placeHolderText, const QString &helpText,
                            const QString &unit, const std::vector<ComboEntry> &options,
                            const std::vector<ComboEntry> &optionsSecond, const std::optional<VALUE_T> &defaultVal)
    requires std::same_as<VALUE_T, QString> || std::same_as<VALUE_T, int> || std::same_as<VALUE_T, bool>
  {
    mEditable = new QWidget();
    mEditable->setObjectName("panelFunction");
    mEditable->setStyleSheet(
        "QComboBox {"
        "   border: 1px solid rgba(32, 27, 23, 0.6);"
        "   border-radius: 4px;"
        "   padding-top: 10px;"
        "   padding-bottom: 10px;"
        "   padding-left: 10px;"
        "   min-width: 6em;"
        "   color: #333;"
        "   background-color: #fff;"
        "   selection-background-color: rgba(48,140,198,0.7);"
        "}"
        "QComboBox:editable {"
        "   background: #fff;"
        "   padding-left: 20px;"
        "}"

        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: right top;"
        "   width: 20px;"
        "   border-left: none;"
        "   border-radius: 4px 4px 4px 4px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow {"
        "   image: url(:/icons/outlined/icons8-sort-down-50.png);"
        "   width: 16px;"
        "   background: #fff;"
        "}"

        "QComboBox::down-arrow:on {"
        "   top: 1px;"
        "}"

        "QComboBox QAbstractItemView {"
        "   border: none;"
        "   background-color: #fff;"
        "}"
        "QWidget#panelFunction { background-color: rgba(0, 104, 117, 0);}");
    QVBoxLayout *layoutVertical = new QVBoxLayout(mParent);
    layoutVertical->setContentsMargins(8, 8, 8, 0);

    QWidget *horizontaContainer   = new QWidget();
    QHBoxLayout *layoutHorizontal = new QHBoxLayout(mParent);
    layoutHorizontal->setContentsMargins(0, 0, 0, 0);

    const QIcon myIcon(":/icons/outlined/" + icon);
    mComboBox = new QComboBox();
    mComboBox->addAction(myIcon, "");    // const QIcon &icon, const QString &text
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    for(const auto &data : options) {
      mComboBox->addItem(myIcon, data.label, QVariant(data.key));
    }
    mComboBox->setFont(fontLineEdit);
    mComboBox->setPlaceholderText(placeHolderText);
    layoutHorizontal->addWidget(mComboBox);
    connect(mComboBox, &QComboBox::currentIndexChanged, this, &ContainerFunction::comboxEditingFinished);
    int32_t idx = 0;
    if(defaultVal.has_value()) {
      idx = mComboBox->findData(defaultVal.value());
    }
    if(idx >= 0) {
      mComboBox->setCurrentIndex(idx);
    }

    if(!optionsSecond.empty()) {
      layoutHorizontal->addWidget(createSecondCombo(optionsSecond));
    }
    horizontaContainer->setLayout(layoutHorizontal);
    layoutVertical->addWidget(horizontaContainer);

    createHelperText(layoutVertical, helpText);
    mEditable->setLayout(layoutVertical);
  }

  void createHelperText(QVBoxLayout *layout, const QString &helpText)
  {
    auto *helperText = new QLabel();
    helperText->setObjectName("functionHelperText");
    helperText->setText(helpText);
    helperText->setContentsMargins(12, 0, 0, 0);
    // Create a QFont instance
    QFont font;
    font.setPixelSize(12);
    font.setItalic(true);
    font.setBold(false);
    font.setWeight(QFont::Light);
    helperText->setFont(font);
    helperText->setStyleSheet("QLabel#functionHelperText { color : #808080; }");
    layout->addWidget(helperText);
  }

  QComboBox *createSecondCombo(const std::vector<ComboEntry> &optionsSecond)
  {
    mComboBoxSecond = new QComboBox();
    mComboBoxSecond->setObjectName("SecondCombo");
    mComboBoxSecond->setStyleSheet(
        "QComboBox#SecondCombo {"
        "   border: 1px solid rgba(32, 27, 23, 0.6);"
        "   border-radius: 4px;"
        "   padding-top: 10px;"
        "   padding-bottom: 10px;"
        "   padding-left: 5px;"
        "   color: #333;"
        "   background-color: #fff;"
        "   selection-background-color: rgba(48,140,198,0.7);"
        "}");
    QFont fontLineEdit;
    fontLineEdit.setPixelSize(16);
    for(const auto &data : optionsSecond) {
      mComboBoxSecond->addItem(data.label, QVariant(data.key));
    }
    mComboBoxSecond->setFont(fontLineEdit);
    mComboBoxSecond->setPlaceholderText("");
    mComboBoxSecond->setMaximumWidth(50);
    return mComboBoxSecond;
  }

  void updateDisplayText()
  {
    mDisplayLabel->setText(mDisplayText);
  }

  /////////////////////////////////////////////////////
  QString mDisplayText = "";
  std::optional<VALUE_T> mDefaultValue;

  /////////////////////////////////////////////////////
  QLineEdit *mLineEdit       = nullptr;
  QComboBox *mComboBox       = nullptr;
  QComboBox *mComboBoxSecond = nullptr;
  QLabel *mDisplayLabel      = nullptr;
  QString mUnit;

  QWidget *mParent      = nullptr;
  QWidget *mDisplayable = nullptr;
  QWidget *mEditable    = nullptr;

private slots:
  void lineEditingChanged()
  {
    if(mLineEdit != nullptr) {
      if(!mLineEdit->text().isNull() && !mLineEdit->text().isEmpty()) {
        mDisplayText = mLineEdit->text() + " " + mUnit;
      } else {
        mDisplayText = "- " + mUnit;
      }
      updateDisplayText();
      ContainerFunction<VALUE_T>::triggerValueChanged();
    }
  }

  void comboxEditingFinished()
  {
    if(mComboBox != nullptr) {
      mDisplayText = mComboBox->currentText();
      updateDisplayText();
      ContainerFunction<VALUE_T>::triggerValueChanged();
    }
  }
};

}    // namespace joda::ui::qt
