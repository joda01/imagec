

#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "results.h"

namespace joda::settings::json {
class AnalyzeSettings;
}

namespace joda::results {

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
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(!mColumns.contains(key)) {
      mColumns[key].setTableName(channelName);
    }

    return mColumns.at(key);
  }

  const Table &getTableAt(int32_t key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(mColumns.contains(key)) {
      return mColumns.at(key);
    }
    throw std::invalid_argument("Table does not exist!");
  }

  bool containsTable(int32_t key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    return mColumns.contains(key);
  }

  static void flushReportToFile(const joda::settings::json::AnalyzeSettings &analyzeSettings,
                                const std::map<std::string, ReportingContainer> &containers,
                                const std::string &fileName, const std::string &jobName, OutputFormat format,
                                bool writeRunMeta);

  mutable std::map<int32_t, Table> mColumns;    // Each column is the representation of a channel

private:
  mutable std::mutex mAccessMutex;
};

}    // namespace joda::results
