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
#include "backend/processor/process_context.hpp"
#include "backend/settings/setting.hpp"
#include <opencv2/core/mat.hpp>
#include "command.hpp"

namespace joda::cmd {

template <class T>
concept Command_t = ::std::is_base_of<Command, T>::value;

template <class T>
concept Setting_t = ::std::is_base_of<settings::Setting, T>::value;

template <Command_t CMD, Setting_t SETTING>
class Factory : public joda::cmd::Command
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

private:
  const SETTING &mSetting;
};

}    // namespace joda::cmd
