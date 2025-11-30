///
/// \file      factory.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "backend/global_enums.hpp"
#include "backend/processor/context/process_context.hpp"
#include "backend/settings/pipeline/pipeline_step.hpp"
#include "backend/settings/setting.hpp"
#include <opencv2/core/mat.hpp>
#include "command.hpp"

namespace joda::ui::gui {

template <class T>
concept Command_t = ::std::is_base_of<joda::ui::gui::Command, T>::value;

template <Command_t CMD, class SETTING>
class Factory : public CMD
{
public:
  Factory(joda::settings::AnalyzeSettings *analyzeSettings, settings::PipelineStep &step, SETTING &setting, QWidget *parent = nullptr) :
      CMD(analyzeSettings, step, setting, parent)
  {
  }

private:
};

}    // namespace joda::ui::gui
