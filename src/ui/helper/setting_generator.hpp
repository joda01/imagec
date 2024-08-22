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
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "ui/container/setting/setting_base.hpp"
#include "ui/container/setting/setting_combobox.hpp"

namespace joda::ui {

inline auto generateClusterDropDown(const QString &helpText, QWidget *parent)
{
  auto dropBox = SettingBase::create<SettingComboBox<enums::ClusterIdIn>>(parent, "icons8-connection-50.png", helpText);
  dropBox->addOptions({
      {enums::ClusterIdIn::$, "THIS"},
      {enums::ClusterIdIn::A, "Cluster A"},
      {enums::ClusterIdIn::B, "Cluster B"},
      {enums::ClusterIdIn::C, "Cluster C"},
      {enums::ClusterIdIn::D, "Cluster D"},
      {enums::ClusterIdIn::E, "Cluster E"},
      {enums::ClusterIdIn::F, "Cluster F"},
      {enums::ClusterIdIn::G, "Cluster G"},
      {enums::ClusterIdIn::H, "Cluster H"},
      {enums::ClusterIdIn::I, "Cluster I"},
      {enums::ClusterIdIn::J, "Cluster J"},
  });
  return dropBox;
}

inline auto generateClassDropDown(const QString &helpText, QWidget *parent)
{
  auto dropBox = SettingBase::create<SettingComboBox<enums::ClassId>>(parent, "icons8-connection-50.png", helpText);
  dropBox->addOptions({
      {enums::ClassId::NONE, "None"},
      {enums::ClassId::C0, "Class 0"},
      {enums::ClassId::C1, "Class 1"},
      {enums::ClassId::C2, "Class 2"},
      {enums::ClassId::C3, "Class 3"},
      {enums::ClassId::C4, "Class 4"},
      {enums::ClassId::C5, "Class 5"},
      {enums::ClassId::C6, "Class 6"},
      {enums::ClassId::C7, "Class 7"},
      {enums::ClassId::C8, "Class 8"},
      {enums::ClassId::C8, "Class 9"},
      {enums::ClassId::C10, "Class 10"},
  });
  return dropBox;
}

}    // namespace joda::ui
