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
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"
#include "ui/container/setting/setting_combobox_multi.hpp"

namespace joda::ui {

template <class T>
concept ImageCStackCombo_t =
    std::is_base_of<SettingComboBoxMulti<int32_t>, T>::value || std::is_base_of<SettingComboBox<int32_t>, T>::value;

template <ImageCStackCombo_t T>
inline auto generateCStackCombo(const QString &helpText, QWidget *parent)
{
  auto dropBox = SettingBase::create<T>(parent, "icons8-unknown-status-50.png", helpText);
  dropBox->addOptions({{-1, "None"},
                       {0, "CH0"},
                       {1, "CH1"},
                       {2, "CH2"},
                       {3, "CH3"},
                       {4, "CH4"},
                       {5, "CH5"},
                       {6, "CH6"},
                       {7, "CH7"},
                       {8, "CH8"},
                       {9, "CH9"}});
  return dropBox;
}

}    // namespace joda::ui
