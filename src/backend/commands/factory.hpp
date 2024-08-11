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
/// \brief     A short description what happens here.
///

#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/global_enums.hpp"
#include "backend/processor/process_context.hpp"
#include <opencv2/core/mat.hpp>
#include "command.hpp"
#include "setting.hpp"

namespace joda::cmd {

template <class T>
concept Command_t = ::std::is_base_of<Command, T>::value;

template <class T>
concept Setting_t = ::std::is_base_of<Setting, T>::value;

template <Command_t CMD, Setting_t SETTING>
class Factory : public joda::cmd::Command
{
public:
  Factory(const SETTING &setting) : mSetting(setting)
  {
  }
  void execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
               cmd::ObjectsListMap &result) override
  {
    CMD func(mSetting);
    func.execute(context, memory, image, result);
  }

private:
  const SETTING &mSetting;
};

}    // namespace joda::cmd
