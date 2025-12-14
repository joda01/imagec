///
/// \file      iteration_context.cpp
/// \author    Joachim Danmayr
/// \date      2025-12-13
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "backend/processor/context/process_context.hpp"

namespace joda::processor {

void IterationContext::removeTemporaryObjects(ProcessContext *context)
{
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_01));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_02));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_03));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_04));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_05));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_06));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_07));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_08));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_09));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_10));
  getObjects().erase(context->getTemporaryClassId(enums::ClassIdIn::TEMP_11));
}
}    // namespace joda::processor
