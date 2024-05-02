

#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/analze_settings.hpp"
#include "results.h"

namespace joda::results {

class ReportingContainer
{
public:
  ReportingContainer();
  Table &getTableAt(joda::settings::ChannelIndex key, const std::string &channelName)
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(!tables.contains(key)) {
      tables[key].setTableName(channelName);
    }

    return tables.at(key);
  }

  const Table &getTableAt(joda::settings::ChannelIndex key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    if(tables.contains(key)) {
      return tables.at(key);
    }
    throw std::invalid_argument("Table does not exist!");
  }

  bool containsTable(joda::settings::ChannelIndex key) const
  {
    std::lock_guard<std::mutex> lock(mAccessMutex);
    return tables.contains(key);
  }

  bool containsInvalidChannel() const
  {
    for(const auto &ch : tables) {
      auto [valid, _] = ch.second.getTableValidity();
      if(valid != func::ResponseDataValidity::VALID) {
        return true;
      }
    }
    return false;
  }

  bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const
  {
    for(const auto &[_, ch] : tables) {
      auto [valid, invalidAll] = ch.getTableValidity();
      if(valid != func::ResponseDataValidity::VALID && invalidAll) {
        return true;
      }
    }
    return false;
  }

private:
  mutable std::mutex mAccessMutex;
  mutable std::map<joda::settings::ChannelIndex, Table> tables;    // Each column is the representation of a channel
};

}    // namespace joda::results
