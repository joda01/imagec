///
/// \file      cereal_cv_mat.hpp
/// \author    Joachim Danmayr
/// \date      2025-10-12
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/enums/enums_classes.hpp"
#include "backend/enums/types.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/vector.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

namespace cv {

template <class Archive>
void save(Archive &ar, cv::Mat const &mat)
{
  int rows        = mat.rows;
  int cols        = mat.cols;
  int type        = mat.type();
  bool continuous = mat.isContinuous();

  // Serialize header information
  ar(rows, cols, type, continuous);

  if(continuous) {
    // For continuous data, serialize the whole data block at once
    const size_t data_size = rows * cols * mat.elemSize();
    // cereal::binary_data is key for raw binary data
    ar(cereal::binary_data(mat.ptr(), data_size));
  } else {
    // Handle non-continuous data row-by-row
    const size_t row_size = cols * mat.elemSize();
    for(int i = 0; i < rows; i++) {
      ar(cereal::binary_data(mat.ptr(i), row_size));
    }
  }
}

template <class Archive>
void load(Archive &ar, cv::Mat &mat)
{
  int rows;
  int cols;
  int type;
  bool continuous;

  // Deserialize header information
  ar(rows, cols, type, continuous);

  // Create the Mat object
  mat.create(rows, cols, type);

  // Load data
  if(continuous) {
    const size_t data_size = rows * cols * mat.elemSize();
    ar(cereal::binary_data(mat.ptr(), data_size));
  } else {
    const size_t row_size = cols * mat.elemSize();
    for(int i = 0; i < rows; i++) {
      ar(cereal::binary_data(mat.ptr(i), row_size));
    }
  }
}

/////////////////////////////////////////////////////

template <class Archive>
void save(Archive &ar, cv::Rect const &in)
{
  ar(in.x, in.y, in.width, in.height);
}

template <class Archive>
void load(Archive &ar, cv::Rect &in)
{
  ar(in.x, in.y, in.width, in.height);
}

template <class Archive>
void save(Archive &ar, cv::Point const &in)
{
  ar(in.x, in.y);
}

template <class Archive>
void load(Archive &ar, cv::Point &in)
{
  ar(in.x, in.y);
}

template <class Archive>
void save(Archive &ar, cv::Size const &in)
{
  ar(in.width, in.height);
}

template <class Archive>
void load(Archive &ar, cv::Size &in)
{
  ar(in.width, in.height);
}

}    // namespace cv

namespace joda::enums {

/////////////////////////////////////////////////////

template <class Archive>
void save(Archive &ar, joda::enums::PlaneId const &in)
{
  ar(in.tStack, in.zStack, in.cStack);
}

template <class Archive>
void load(Archive &ar, joda::enums::PlaneId &in)
{
  ar(in.tStack, in.zStack, in.cStack);
}

}    // namespace joda::enums

namespace joda::atom {

/////////////////////////////////////////////////////

}    // namespace joda::atom
