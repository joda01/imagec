

#pragma once

#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include "backend/image_processing/detection/detection_response.hpp"
#include "backend/settings/analze_settings.hpp"
#include "results.h"

namespace joda::results {

class TableWorkbook
{
public:
  /////////////////////////////////////////////////////
  TableWorkbook();
  // void saveToFile(const std::string &fileName);
  // void loadFromFile(const std::string &fileName);

  Table &getTableAt(joda::settings::ChannelIndex key, const std::string &channelName) const;
  const Table &getTableAt(joda::settings::ChannelIndex key) const;
  std::map<joda::settings::ChannelIndex, Table> &getTables() const;
  bool containsTable(joda::settings::ChannelIndex key) const;
  bool containsInvalidChannel() const;
  bool containsInvalidChannelWhereOneInvalidatesTheWholeImage() const;

private:
  /////////////////////////////////////////////////////
  mutable std::map<joda::settings::ChannelIndex, Table>
      tables;    // Each column is the representation of a channel, each channel is a table of data

  std::string configSchema = "https://imagec.org/schemas/v1/results-workbook.json";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(TableWorkbook, configSchema, tables);

  mutable std::mutex mAccessMutex;
};

}    // namespace joda::results
