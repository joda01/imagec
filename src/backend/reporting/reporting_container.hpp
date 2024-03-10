

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
    /// \todo Not thread safe
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

  static void flushReportToFile(const std::map<std::string, ReportingContainer> &containers,
                                const std::string &fileName, OutputFormat format);

  std::map<int32_t, Table> mColumns;    // Each column is the representation of a channel
};

}    // namespace joda::reporting
