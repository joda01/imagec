///
/// \file      enums_file_endians.hpp
/// \author    Joachim Danmayr
/// \date      2024-09-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#pragma once

#include <string>

namespace joda::fs {

static inline std::string EXT_DATABASE                         = ".icdb";
static inline std::string EXT_PROJECT                          = ".icproj";
static inline std::string EXT_PROJECT_TEMPLATE                 = ".ictemplproj";
static inline std::string EXT_PIPELINE_TEMPLATE                = ".ictempl";
static inline std::string EXT_CLASS_CLASS_TEMPLATE             = ".ictemplcc";
static inline std::string MASCHINE_LEARNING_OPCEN_CV_XML_MODEL = ".icmlmcv";
static inline std::string EXT_ANNOTATION                       = ".icroibin";
static inline std::string WORKING_DIRECTORY_PROJECT_PATH       = "imagec";
static inline std::string USER_SETTINGS_PATH                   = "imagec";

}    // namespace joda::fs
