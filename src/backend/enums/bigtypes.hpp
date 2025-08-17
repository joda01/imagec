#pragma once

#include <cstdint>
#include <stdexcept>

namespace joda::stdi {

class uint128_t
{
public:
  uint128_t()                  = default;
  uint128_t(const uint128_t &) = default;

  explicit uint128_t(uint32_t low) : mLow(low)
  {
  }

  explicit uint128_t(int32_t low) : mLow(static_cast<uint64_t>(low))
  {
  }

  explicit uint128_t(uint64_t low) : mLow(low)
  {
  }

  uint128_t(uint64_t high, uint64_t low) : mHigh(high), mLow(low)
  {
  }

  // user-defined copy assignment (copy-and-swap idiom)
  uint128_t &operator=(uint128_t other)
  {
    std::swap(mHigh, other.mHigh);
    std::swap(mLow, other.mLow);
    return *this;
  }
  constexpr bool operator==(const uint128_t &other) const
  {
    return mLow == other.mLow && mHigh == other.mHigh;
  }

  constexpr bool operator==(const uint32_t &other) const
  {
    return mLow == other && mHigh == 0;
  }

  // Overload subtraction operator
  uint128_t operator-(const uint128_t &other) const
  {
    return {mHigh - other.mHigh - static_cast<uint64_t>(mLow < other.mLow), mLow - other.mLow};
  }

  uint128_t operator+(const uint64_t other) const
  {
    uint128_t result;
    result.mLow = mLow + other;
    if(result.mLow < mLow) {
      result.mHigh++;
    }
    return result;
  }

  // Overload modulo operator
  uint128_t operator%(const uint32_t &divisor) const
  {
    return operator%(uint128_t(0, divisor));
  }

  // Overload modulo operator
  uint128_t operator%(const uint128_t &divisor) const
  {
    if(divisor.mHigh == 0 && divisor.mLow == 0) {
      throw std::runtime_error("Modulo by zero error");
    }

    uint128_t dividend  = *this;
    uint128_t remainder = {0, 0};

    for(int i = 127; i >= 0; --i) {
      // Shift remainder left by 1 bit and bring down the next bit from dividend
      remainder = remainder << 1;
      remainder.mLow |= (dividend.mHigh >> 63);    // Bring down MSB from high
      dividend.mHigh = (dividend.mHigh << 1) | (dividend.mLow >> 63);
      dividend.mLow <<= 1;

      // Subtract divisor if remainder >= divisor
      if(!(remainder < divisor)) {
        remainder = remainder - divisor;
      }
    }
    return remainder;
  }

  // Left shift operator overload
  uint128_t operator<<(uint8_t shift) const
  {
    uint128_t result;
    if(shift == 0) {
      result = *this;
    } else if(shift < 64) {
      result.mHigh = (mHigh << shift) | (mLow >> (64 - shift));
      result.mLow  = mLow << shift;
    } else {    // shift >= 64
      result.mHigh = mLow << (shift - 64);
      result.mLow  = 0;
    }
    return result;
  }

  // Right shift operator overload
  uint128_t operator>>(uint8_t shift) const
  {
    uint128_t result;
    if(shift == 0) {
      result = *this;
    } else if(shift < 64) {
      result.mLow  = (mLow >> shift) | (mHigh << (64 - shift));
      result.mHigh = mHigh >> shift;
    } else {    // shift >= 64
      result.mLow  = mHigh >> (shift - 64);
      result.mHigh = 0;
    }
    return result;
  }

  // Overload less than operator
  bool operator<(const uint128_t &other) const
  {
    return (mHigh < other.mHigh) || (mHigh == other.mHigh && mLow < other.mLow);
  }

  // Overload greater than operator
  bool operator>(const uint128_t &other) const
  {
    return (mHigh > other.mHigh) || (mHigh == other.mHigh && mLow > other.mLow);
  }

  // Overload greater than operator
  bool operator>(const uint32_t &other) const
  {
    return (mHigh > 0) || (mHigh == 0 && mLow > other);
  }

  [[nodiscard]] uint64_t lowRet() const
  {
    return mLow;
  }

  // Overload modulo operator
  uint128_t operator/=(const uint32_t &divisor)
  {
    return operator/=(uint128_t(0, divisor));
  }

  // Overload division operator (in-place division)
  uint128_t &operator/=(const uint128_t &divisor)
  {
    if(divisor.mHigh == 0 && divisor.mLow == 0) {
      throw std::runtime_error("Division by zero error");
    }
    if(*this < divisor) {
      *this = {0, 0};    // If dividend is smaller than divisor, result is 0
      return *this;
    }

    uint128_t quotient  = {0, 0};
    uint128_t remainder = {0, 0};
    uint128_t dividend  = *this;

    for(int i = 127; i >= 0; --i) {
      // Shift remainder left by 1 bit and bring down the next bit from dividend
      remainder = remainder << 1;
      remainder.mLow |= (dividend.mHigh >> 63);    // Bring down MSB from high
      dividend.mHigh = (dividend.mHigh << 1) | (dividend.mLow >> 63);
      dividend.mLow <<= 1;

      // If remainder >= divisor, subtract divisor and set quotient bit
      if(!(remainder < divisor)) {
        remainder = remainder - divisor;
        if(i >= 64) {
          quotient.mHigh |= (1ULL << (i - 64));
        } else {
          quotient.mLow |= (1ULL << i);
        }
      }
    }

    *this = quotient;
    return *this;
  }

  // Overload bitwise OR operator
  uint128_t operator|(const uint32_t &other) const
  {
    return {mHigh | 0, mLow | other};
  }

  // Overload bitwise OR operator
  uint128_t operator|(const uint128_t &other) const
  {
    return {mHigh | other.mHigh, mLow | other.mLow};
  }

  [[nodiscard]] uint64_t highBytes() const
  {
    return mHigh;
  }

private:
  uint64_t mHigh = 0;
  uint64_t mLow  = 0;
};
}    // namespace joda::stdi
