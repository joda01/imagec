
#pragma once

#include <duckdb.h>
#include <vector>
#include <duckdb/common/types/value.hpp>
#include <duckdb/common/types/vector.hpp>
#include <duckdb/main/appender.hpp>

namespace joda::rle {

///
/// \brief      Flatten RLE encoded
/// \author
/// \param[in]
/// \param[out]
/// \return
///
inline duckdb::Value rle_encode(const std::vector<uint8_t> &data)
{
  duckdb::vector<duckdb::Value> key;
  duckdb::vector<duckdb::Value> value;

  uint32_t count     = 1;
  bool current_value = data[0] > 0;

  for(size_t i = 1; i < data.size(); ++i) {
    if((data[i] > 0) == current_value) {
      count++;
    } else {
      key.emplace_back(duckdb::Value::UINTEGER(count));
      value.emplace_back(duckdb::Value::BOOLEAN(current_value));
      count         = 1;
      current_value = data[i] > 0;
    }
  }
  key.emplace_back(duckdb::Value::UINTEGER(count));
  value.emplace_back(duckdb::Value::BOOLEAN(current_value));

  auto mask = duckdb::Value::MAP(duckdb::LogicalType(duckdb::LogicalTypeId::UINTEGER),
                                 duckdb::LogicalType(duckdb::LogicalTypeId::BOOLEAN), key, value);

  return mask;
}

inline std::vector<bool> rle_decode(const std::vector<std::pair<int, bool>> &encoded)
{
  std::vector<bool> decoded;

  for(const auto &pair : encoded) {
    for(int i = 0; i < pair.first; ++i) {
      decoded.push_back(pair.second);
    }
  }

  return decoded;
}

}    // namespace joda::rle
