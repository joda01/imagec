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
#include "ui/container/setting/setting_combobox _multi.hpp"
#include "ui/container/setting/setting_combobox.hpp"

namespace joda::ui {

template <class T>
concept ClusterCombo_t = std::is_base_of<SettingComboBoxMulti<enums::ClusterIdIn>, T>::value ||
                         std::is_base_of<SettingComboBox<enums::ClusterIdIn>, T>::value;

template <ClusterCombo_t T>
inline auto generateClusterDropDown(const QString &helpText, QWidget *parent)
{
  auto dropBox = SettingBase::create<T>(parent, "icons8-connection-50.png", helpText);
  dropBox->addOptions({
      {enums::ClusterIdIn::$, "THIS"},
      {enums::ClusterIdIn::NONE, "None"},
      {enums::ClusterIdIn::A, "A"},
      {enums::ClusterIdIn::B, "B"},
      {enums::ClusterIdIn::C, "C"},
      {enums::ClusterIdIn::D, "D"},
      {enums::ClusterIdIn::E, "E"},
      {enums::ClusterIdIn::F, "F"},
      {enums::ClusterIdIn::G, "G"},
      {enums::ClusterIdIn::H, "H"},
      {enums::ClusterIdIn::I, "I"},
      {enums::ClusterIdIn::J, "J"},
  });
  return dropBox;
}

template <class T>
concept ClassCombo_t = std::is_base_of<SettingComboBoxMulti<enums::ClassId>, T>::value ||
                       std::is_base_of<SettingComboBox<enums::ClassId>, T>::value;

template <ClassCombo_t T>
inline auto generateClassDropDown(const QString &helpText, QWidget *parent)
{
  auto dropBox = SettingBase::create<T>(parent, "icons8-connection-50.png", helpText);
  dropBox->addOptions({
      {enums::ClassId::NONE, "None"},
      {enums::ClassId::C0, "0"},
      {enums::ClassId::C1, "1"},
      {enums::ClassId::C2, "2"},
      {enums::ClassId::C3, "3"},
      {enums::ClassId::C4, "4"},
      {enums::ClassId::C5, "5"},
      {enums::ClassId::C6, "6"},
      {enums::ClassId::C7, "7"},
      {enums::ClassId::C8, "8"},
      {enums::ClassId::C8, "9"},
      {enums::ClassId::C10, "10"},
  });
  return dropBox;
}

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
