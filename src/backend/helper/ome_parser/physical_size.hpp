///
/// \file      physical_size.hpp
/// \author    Joachim Danmayr
/// \date      2025-08-17
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <stdexcept>
#include <tuple>
#include "backend/enums/enums_units.hpp"

namespace joda::ome {

class PhyiscalSize
{
public:
  static PhyiscalSize Pixels()
  {
    return {1, 1, 1, enums::Units::Pixels};
  }
  PhyiscalSize() = default;
  PhyiscalSize(double sizeX, double sizeY, double sizeZ, enums::Units unit)
  {
    if(sizeX <= 0 || sizeY <= 0) {
      mSizeX = std::numeric_limits<double>::quiet_NaN();    // Always um
      mSizeY = std::numeric_limits<double>::quiet_NaN();    // Always um
      mSizeZ = std::numeric_limits<double>::quiet_NaN();    // Always um
      return;
    }
    switch(unit) {
      case enums::Units::Pixels:
        mSizeX = 1;
        mSizeY = 1;
        mSizeZ = 1;
        break;
      case enums::Units::nm:
        mSizeX = sizeX / 1e3;
        mSizeY = sizeY / 1e3;
        mSizeZ = sizeZ / 1e3;
        break;
      case enums::Units::um:
        mSizeX = sizeX;
        mSizeY = sizeY;
        mSizeZ = sizeZ;
        break;
      case enums::Units::mm:
        mSizeX = sizeX * 1e3;
        mSizeY = sizeY * 1e3;
        mSizeZ = sizeZ * 1e3;
        break;
      case enums::Units::cm:
        mSizeX = sizeX * 1e4;
        mSizeY = sizeY * 1e4;
        mSizeZ = sizeZ * 1e4;
        break;
      case enums::Units::m:
        mSizeX = sizeX * 1e6;
        mSizeY = sizeY * 1e6;
        mSizeZ = sizeZ * 1e6;
        break;
      case enums::Units::km:
        mSizeX = sizeX * 1e9;
        mSizeY = sizeY * 1e9;
        mSizeZ = sizeZ * 1e9;
        break;
      case enums::Units::Undefined:
        break;
    }
  }

  [[nodiscard]] bool isSet() const
  {
    return mSizeX == mSizeX && mSizeY == mSizeY;
  }

  [[nodiscard]] auto getPixelSize(enums::Units unit) const -> std::tuple<double, double, double>
  {
    switch(unit) {
      case enums::Units::Pixels:
        return {1, 1, 1};
      case enums::Units::nm:
        return {mSizeX * 1e3, mSizeY * 1e3, mSizeZ * 1e3};
      case enums::Units::um:
        return {mSizeX, mSizeY, mSizeZ};
      case enums::Units::mm:
        return {mSizeX / 1e3, mSizeY / 1e3, mSizeZ / 1e3};
      case enums::Units::cm:
        return {mSizeX / 1e4, mSizeY / 1e4, mSizeZ / 1e4};
      case enums::Units::m:
        return {mSizeX / 1e6, mSizeY / 1e6, mSizeZ / 1e6};
      case enums::Units::km:
        return {mSizeX / 1e9, mSizeY / 1e9, mSizeZ / 1e9};
      case enums::Units::Undefined:
        break;
    }
    throw std::runtime_error("Not a valid unit!");
  }

private:
  double mSizeX = std::numeric_limits<double>::quiet_NaN();    // Always um
  double mSizeY = std::numeric_limits<double>::quiet_NaN();    // Always um
  double mSizeZ = std::numeric_limits<double>::quiet_NaN();    // Always um
};

}    // namespace joda::ome
