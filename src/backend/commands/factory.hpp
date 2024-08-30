///
/// \file      factory.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/global_enums.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/setting.hpp"
#include <opencv2/core/mat.hpp>
#include "command.hpp"

namespace joda::cmd {

template <class T>
concept Command_t = ::std::is_base_of<Command, T>::value;

template <class T>
concept Setting_t = ::std::is_base_of<joda::settings::SettingBase, T>::value;

class CommandFactory
{
public:
  virtual void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) = 0;
  virtual std::set<enums::ClusterIdIn> getInputClusters() const                                      = 0;
};

template <Command_t CMD, Setting_t SETTING>
class Factory : public CommandFactory    // public joda::cmd::Command, public settings::SettingBase
{
public:
  Factory(const SETTING &setting) : mSetting(setting)
  {
  }
  void execute(processor::ProcessContext &context, cv::Mat &image, atom::ObjectList &result) override
  {
    CMD func(mSetting);
    func(context, image, result);
  }
  [[nodiscard]] std::set<enums::ClusterIdIn> getInputClusters() const override
  {
    return mSetting.getInputClusters();
  }

private:
  const SETTING &mSetting;
};

}    // namespace joda::cmd
