///
/// \file      setting_generator.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-20
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
///

#pragma once

#include <qwidget.h>
#include <type_traits>
#include "backend/enums/enum_images.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_multi.hpp"
#include "ui/helper/icon_generator.hpp"

namespace joda::ui {

template <class T>
concept ImageCStackCombo_t = std::is_base_of<SettingComboBoxMulti<int32_t>, T>::value || std::is_base_of<SettingComboBox<int32_t>, T>::value;

template <ImageCStackCombo_t T>
inline auto generateCStackCombo(const QString &helpText, QWidget *parent, const QString &thisText = "Default")
{
  auto dropBox = SettingBase::create<T>(parent, generateIcon("channel"), helpText);
  dropBox->addOptions({{-1, thisText, generateIcon("square")},
                       {0, "CH0", generateIcon("zero")},
                       {1, "CH1", generateIcon("one")},
                       {2, "CH2", generateIcon("2")},
                       {3, "CH3", generateIcon("three")},
                       {4, "CH4", generateIcon("four")},
                       {5, "CH5", generateIcon("5")},
                       {6, "CH6", generateIcon("six")},
                       {7, "CH7", generateIcon("seven")},
                       {8, "CH8", generateIcon("eight")},
                       {9, "CH9", generateIcon("nine")}});
  return dropBox;
}

inline auto generateZProjection(bool withThis, QWidget *parent) -> std::unique_ptr<SettingComboBox<enums::ZProjection>>
{
  auto zProjection = SettingBase::create<SettingComboBox<enums::ZProjection>>(parent, generateIcon("layer"), "Z-Projection");
  if(withThis) {
    zProjection->addOptions({{enums::ZProjection::$, "Default"},
                             {enums::ZProjection::NONE, "Off"},
                             {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                             {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                             {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}});
  } else {
    zProjection->addOptions({{enums::ZProjection::NONE, "Off"},
                             {enums::ZProjection::MAX_INTENSITY, "Max. intensity"},
                             {enums::ZProjection::MIN_INTENSITY, "Min. intensity"},
                             {enums::ZProjection::AVG_INTENSITY, "Avg'. intensity"}});
  }
  return zProjection;
}

inline auto generateThresholdClass(const QString &helpText, QWidget *parent) -> std::unique_ptr<SettingComboBox<int32_t>>
{
  auto mGrayScaleValue = SettingBase::create<SettingComboBox<int32_t>>(parent, {}, helpText);
  mGrayScaleValue->setDefaultValue(65535);
  mGrayScaleValue->addOptions({{65535, "TH 1", generateIcon("one-round")},
                               {65534, "TH 2"},
                               {65533, "TH 3"},
                               {65532, "TH 4"},
                               {65530, "TH 5"},
                               {65529, "TH 6"},
                               {65528, "TH 7"},
                               {65527, "TH 8"}});
  mGrayScaleValue->setUnit("");
  mGrayScaleValue->setShortDescription("");

  return mGrayScaleValue;
}

}    // namespace joda::ui
