

#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include "reporting.h"

namespace joda::reporting {

class ReportingContainer
{
public:
  enum class OutputFormat
  {
    VERTICAL,
    HORIZONTAL
  };

  ReportingContainer();
  Table &getTableAt(int32_t key, const std::string &channelName)
  {
    if(!mColumns.contains(key)) {
      mColumns[key].setTableName(channelName);
    }

    return mColumns.at(key);
  }

  const Table &getTableAt(int32_t key) const
  {
    if(mColumns.contains(key)) {
      return mColumns.at(key);
    }
    throw std::invalid_argument("Table does not exist!");
  }

  void flushReportToFile(std::string_view fileName, OutputFormat format);

  std::map<int32_t, Table> mColumns;
};

}    // namespace joda::reporting
