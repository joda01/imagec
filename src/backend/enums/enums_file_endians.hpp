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

static inline std::string EXT_DATABASE             = ".icdb";
static inline std::string EXT_PROJECT              = ".icproj";
static inline std::string EXT_PROJECT_TEMPLATE     = ".ictemplproj";
static inline std::string EXT_PIPELINE_TEMPLATE    = ".ictempl";
static inline std::string EXT_CLASS_CLASS_TEMPLATE = ".ictemplcc";
static inline std::string EXT_ANNOTATION           = ".icroi";
static inline std::string USER_SETTINGS_PATH       = "imagec";

static inline std::string RESULTS_PATH                        = "results";
static inline std::string WORKING_DIRECTORY_TEMP_PROJECT_PATH = "tmp";
static inline std::string WORKING_DIRECTORY_PROJECT_PATH      = "imagec";
static inline std::string WORKING_DIRECTORY_MODELS_PATH       = "models";
static inline std::string WORKING_DIRECTORY_IMAGE_DATA_PATH   = "data";

static inline std::string FILE_NAME_PROJECT_DEFAULT  = "settings";
static inline std::string FILE_NAME_RESULTS_DATABASE = "results";
static inline std::string FILE_NAME_ANNOTATIONS      = "annotations";
static inline std::string FILE_NAME_image_meta       = "meta";

static inline std::string MASCHINE_LEARNING_OPCEN_CV_XML_MODEL = ".icmlcv";
static inline std::string MASCHINE_LEARNING_MLPACK_JSON_MODEL  = ".icmlmlp";
static inline std::string MASCHINE_LEARNING_PYTORCH_JSON_MODEL = ".icmlpt";

}    // namespace joda::fs
