///
/// \file      setting_generator.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <qwidget.h>
#include <type_traits>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"

#include "ui/gui/editor/widget_pipeline/widget_setting/setting_base.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_combobox_multi.hpp"
#include "ui/gui/editor/widget_pipeline/widget_setting/setting_spinbox.hpp"
#include "ui/gui/helper/icon_generator.hpp"

namespace joda::ui::gui {

template <class T>
concept ImageCStackCombo_t = std::is_base_of<SettingComboBoxMulti<int32_t>, T>::value || std::is_base_of<SettingComboBox<int32_t>, T>::value;

template <ImageCStackCombo_t T>
inline auto generateCStackCombo(const QString &helpText, QWidget *parent, const QString &thisText = "Default")
{
  auto dropBox = SettingBase::create<T>(parent, generateSvgIcon("irc-operator"), helpText);
  dropBox->addOptions({{-2, "Off", generateSvgIcon("irc-operator")},
                       {-1, thisText, generateSvgIcon("irc-operator")},
                       {0, "CH0", generateSvgIcon("irc-operator")},
                       {1, "CH1", generateSvgIcon("irc-operator")},
                       {2, "CH2", generateSvgIcon("irc-operator")},
                       {3, "CH3", generateSvgIcon("irc-operator")},
                       {4, "CH4", generateSvgIcon("irc-operator")},
                       {5, "CH5", generateSvgIcon("irc-operator")},
                       {6, "CH6", generateSvgIcon("irc-operator")},
                       {7, "CH7", generateSvgIcon("irc-operator")},
                       {8, "CH8", generateSvgIcon("irc-operator")},
                       {9, "CH9", generateSvgIcon("irc-operator")}});
  return dropBox;
}

inline auto generateZProjection(bool withThis, QWidget *parent) -> std::unique_ptr<SettingComboBox<enums::ZProjection>>
{
  auto zProjection = SettingBase::create<SettingComboBox<enums::ZProjection>>(parent, generateSvgIcon("layer-visible-on"), "Z-Projection");
  if(withThis) {
    zProjection->addOptions({{enums::ZProjection::$, "Default"},
                             {enums::ZProjection::NONE, "Single channel"},
                             {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                             {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                             {enums::ZProjection::AVG_INTENSITY, "Avg. intensity"},
                             {enums::ZProjection::TAKE_MIDDLE, "Take middle"}});
  } else {
    zProjection->addOptions({{enums::ZProjection::NONE, "Single channel"},
                             {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                             {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                             {enums::ZProjection::AVG_INTENSITY, "Avg. intensity"},
                             {enums::ZProjection::TAKE_MIDDLE, "Take middle"}});
  }
  return zProjection;
}

inline auto generateStackIndexCombo(bool withThis, const QString &helpText, QWidget *parent) -> std::unique_ptr<SettingSpinBox<int32_t>>
{
  auto channelSpinbox = SettingBase::create<SettingSpinBox<int32_t>>(parent, {}, helpText);
  int32_t min         = withThis ? -1 : 0;
  channelSpinbox->setDefaultValue(min);
  channelSpinbox->setMinMax(min, 65535);
  channelSpinbox->setUnit("");
  channelSpinbox->setShortDescription("");
  return channelSpinbox;
}

inline auto generateThresholdClass(const QString &helpText, QWidget *parent) -> std::unique_ptr<SettingComboBox<int32_t>>
{
  auto mGrayScaleValue = SettingBase::create<SettingComboBox<int32_t>>(parent, {}, helpText);
  mGrayScaleValue->setDefaultValue(65535);
  mGrayScaleValue->addOptions(
      {{65535, "TH 1"}, {65534, "TH 2"}, {65533, "TH 3"}, {65532, "TH 4"}, {65530, "TH 5"}, {65529, "TH 6"}, {65528, "TH 7"}, {65527, "TH 8"}});
  mGrayScaleValue->setUnit("");
  mGrayScaleValue->setShortDescription("");

  return mGrayScaleValue;
}

inline auto generateAiModelClass(const QString &helpText, QWidget *parent) -> std::unique_ptr<SettingComboBox<int32_t>>
{
  auto mGrayScaleValue = SettingBase::create<SettingComboBox<int32_t>>(parent, {}, helpText);
  mGrayScaleValue->setDefaultValue(0);
  mGrayScaleValue->addOptions({{-1, "Unset", generateSvgIcon("question")},
                               {0, "CL 1"},
                               {1, "CL 2"},
                               {2, "CL 3"},
                               {3, "CL 4"},
                               {4, "CL 5"},
                               {5, "CL 6"},
                               {6, "CL 7"},
                               {7, "CL 8"}});
  mGrayScaleValue->setUnit("");
  mGrayScaleValue->setShortDescription("");

  return mGrayScaleValue;
}

}    // namespace joda::ui::gui
