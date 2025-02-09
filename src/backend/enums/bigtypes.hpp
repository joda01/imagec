#pragma once

#include <cstdint>
#include <stdexcept>

namespace joda::stdi {

class uint128_t
{
public:
  uint128_t() = default;

  explicit uint128_t(uint32_t low) : low(low)
  {
  }

  explicit uint128_t(int32_t low) : low(low)
  {
  }

  explicit uint128_t(uint64_t low) : low(low)
  {
  }

  uint128_t(uint64_t high, uint64_t low) : high(high), low(low)
  {
  }

  // user-defined copy assignment (copy-and-swap idiom)
  uint128_t &operator=(uint128_t other)
  {
    std::swap(high, other.high);
    std::swap(low, other.low);
    return *this;
  }
  constexpr bool operator==(const uint128_t &other) const
  {
    return low == other.low && high == other.high;
  }

  constexpr bool operator==(const uint32_t &other) const
  {
    return low == other && high == 0;
  }

  // Overload subtraction operator
  uint128_t operator-(const uint128_t &other) const
  {
    uint128_t result;
    result.low  = low - other.low;
    result.high = high - other.high - (low < other.low);    // Borrow if needed
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
    if(divisor.high == 0 && divisor.low == 0) {
      throw std::runtime_error("Modulo by zero error");
    }

    uint128_t dividend  = *this;
    uint128_t remainder = {0, 0};

    for(int i = 127; i >= 0; --i) {
      // Shift remainder left by 1 bit and bring down the next bit from dividend
      remainder = remainder << 1;
      remainder.low |= (dividend.high >> 63);    // Bring down MSB from high
      dividend.high = (dividend.high << 1) | (dividend.low >> 63);
      dividend.low <<= 1;

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
      result.high = (high << shift) | (low >> (64 - shift));
      result.low  = low << shift;
    } else {    // shift >= 64
      result.high = low << (shift - 64);
      result.low  = 0;
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
      result.low  = (low >> shift) | (high << (64 - shift));
      result.high = high >> shift;
    } else {    // shift >= 64
      result.low  = high >> (shift - 64);
      result.high = 0;
    }
    return result;
  }

  // Overload less than operator
  bool operator<(const uint128_t &other) const
  {
    return (high < other.high) || (high == other.high && low < other.low);
  }

  // Overload greater than operator
  bool operator>(const uint128_t &other) const
  {
    return (high > other.high) || (high == other.high && low > other.low);
  }

  // Overload greater than operator
  bool operator>(const uint32_t &other) const
  {
    return (high > 0) || (high == 0 && low > other);
  }

  uint64_t lowRet() const
  {
    return low;
  }

  // Overload modulo operator
  uint128_t operator/=(const uint32_t &divisor)
  {
    return operator/=(uint128_t(0, divisor));
  }

  // Overload division operator (in-place division)
  uint128_t &operator/=(const uint128_t &divisor)
  {
    if(divisor.high == 0 && divisor.low == 0) {
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
      remainder.low |= (dividend.high >> 63);    // Bring down MSB from high
      dividend.high = (dividend.high << 1) | (dividend.low >> 63);
      dividend.low <<= 1;

      // If remainder >= divisor, subtract divisor and set quotient bit
      if(!(remainder < divisor)) {
        remainder = remainder - divisor;
        if(i >= 64) {
          quotient.high |= (1ULL << (i - 64));
        } else {
          quotient.low |= (1ULL << i);
        }
      }
    }

    *this = quotient;
    return *this;
  }

  // Overload bitwise OR operator
  uint128_t operator|(const uint32_t &other) const
  {
    return {high | 0, low | other};
  }

  // Overload bitwise OR operator
  uint128_t operator|(const uint128_t &other) const
  {
    return {high | other.high, low | other.low};
  }

private:
  uint64_t high = 0;
  uint64_t low  = 0;
};
}    // namespace joda::stdi
