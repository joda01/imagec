

#pragma once

#include <cstddef>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/analze_settings.hpp"
#include "results.h"

namespace joda::results {

class TableGroup
{
public:
  /////////////////////////////////////////////////////
  struct GroupMeta
  {
    std::string name;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GroupMeta, name);
  };

  /////////////////////////////////////////////////////
  TableGroup();
  TableGroup(const TableGroup &rhs) : meta(rhs.meta), channels(rhs.channels)
  {
  }

  TableGroup &operator=(const TableGroup &rhs);
  Table &getChannelAt(joda::settings::ChannelIndex key, const std::string &channelName) const;
  const Table &getChannelAt(joda::settings::ChannelIndex key) const;
  std::map<joda::settings::ChannelIndex, Table> &getChannels() const;
  bool containsTable(joda::settings::ChannelIndex key) const;
  bool containsInvalidChannel() const;
  bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const;

private:
  /////////////////////////////////////////////////////
  void swap(TableGroup &rhs);

  /////////////////////////////////////////////////////
  // Each column is the representation of a channel, each channel is a table of data
  GroupMeta meta;
  mutable std::map<joda::settings::ChannelIndex, Table> channels;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TableGroup, meta, channels);

  mutable std::mutex mAccessMutex;
};

class TableWorkBook
{
public:
  /////////////////////////////////////////////////////
  TableWorkBook() = default;

  void saveToFile(std::string) const;
  void loadFrom(const std::string &);

  [[nodiscard]] size_t size() const
  {
    return groups.size();
  }

  [[nodiscard]] auto begin() const
  {
    return groups.begin();
  }

  [[nodiscard]] auto end() const
  {
    return groups.end();
  }

  auto &operator[](const std::string &key)
  {
    return groups[key];
  }

private:
  /////////////////////////////////////////////////////
  std::string schema = "https://imagec.org/schemas/v1/results.json";

  std::map<std::string, TableGroup> groups;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TableWorkBook, schema, groups);
};

}    // namespace joda::results
