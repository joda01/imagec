

#include "reporting.h"

namespace joda::reporting {

void Table::appendValueToColumn(uint64_t colIdx, float value)
{
  mTable[colIdx][mTable[colIdx].size()] = value;
  mStatisitcs[colIdx].addValue(value);
}

auto Table::getTable() const -> const std::map<uint64_t, std::map<uint32_t, float>> &
{
  return mTable;
}
auto Table::getStatisitcs() const -> const std::map<uint64_t, Statistics> &
{
  return mStatisitcs;
}

}    // namespace joda::reporting
