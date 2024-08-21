///
/// \file      container_function_base.hpp
/// \author    Joachim Danmayr
/// \date      2024-02-25
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <QtWidgets>

namespace joda::ui {

template <typename T>
struct is_enum
{
  static constexpr bool value = std::is_enum<T>::value;
};

template <typename T>
concept NumberOrString = std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> ||
                         std::same_as<T, float> || std::same_as<T, bool> || std::same_as<T, std::string>;

template <typename T>
concept IntFloatEnumConcept =
    std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> ||
    std::same_as<T, bool> || std::same_as<T, std::string> || std::is_enum<T>::value;

///
/// \class      SettingBase
/// \author     Joachim Danmayr
/// \brief
///
class SettingBase : public QWidget
{
  Q_OBJECT

public:
  /////////////////////////////////////////////////////
  SettingBase(const QString &icon, const QString &description);
  ~SettingBase() = default;

  virtual QWidget *getEditableWidget() = 0;
  static QWidget *createDisplayAbleWidgetPlaceholder();

  void setUnit(const QString &unit);
  void setDisplayIconVisible(bool visible);
  void setShortDescription(const QString &description);

  QWidget *getDisplayLabelWidget();
  [[nodiscard]] QString getDisplayLabelText() const;

signals:
  /////////////////////////////////////////////////////
  void valueChanged();

protected:
  /////////////////////////////////////////////////////
  static constexpr int32_t TXT_ICON_SIZE  = 16;
  static constexpr int32_t DISP_ICON_SIZE = 16;
  static constexpr int32_t HELP_ICON_SIZE = 8;

  /////////////////////////////////////////////////////
  void triggerValueChanged(const QString &newValue);

private:
  /////////////////////////////////////////////////////
  void updateDisplayLabel();
  void createDisplayAbleWidget(const QString &icon, const QString &tooltip);

  /////////////////////////////////////////////////////
  QWidget *mDisplayable     = nullptr;
  QLabel *mDisplayLabel     = nullptr;
  QLabel *mDisplayLabelIcon = nullptr;
  QString mShortDescription;
  QString mDisplayValue;
  QString mUnit;
};

}    // namespace joda::ui
