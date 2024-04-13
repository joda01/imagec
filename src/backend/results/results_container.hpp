

#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/settings/analze_settings.hpp"
#include "results.h"

struct JobMeta
{
  const std::string jobName;
  const std::chrono::system_clock::time_point timeStarted;
  const std::chrono::system_clock::time_point timeFinished;
};

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
  Table &getTableAt(joda::settings::ChannelIndex key, const std::string &channelName)
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(!mColumns.contains(key)) {
      mColumns[key].setTableName(channelName);
    }

    return mColumns.at(key);
  }

  const Table &getTableAt(joda::settings::ChannelIndex key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(mColumns.contains(key)) {
      return mColumns.at(key);
    }
    throw std::invalid_argument("Table does not exist!");
  }

  bool containsTable(joda::settings::ChannelIndex key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    return mColumns.contains(key);
  }

  static void flushReportToFile(const joda::settings::AnalyzeSettings &analyzeSettings,
                                const std::map<std::string, ReportingContainer> &containers,
                                const std::string &fileName, const JobMeta &meta, OutputFormat format,
                                bool writeRunMeta);

  mutable std::map<joda::settings::ChannelIndex, Table> mColumns;    // Each column is the representation of a channel

private:
  mutable std::mutex mAccessMutex;
};

}    // namespace joda::results
