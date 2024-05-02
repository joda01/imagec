///
/// \file      reporting_container.cpp
/// \author    Joachim Danmayr
/// \date      2024-03-06
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "results_container.hpp"

namespace joda::results {

TableGroup::TableGroup()
{
}

TableGroup &TableGroup::operator=(const TableGroup &rhs)
{
  if(this == &rhs) {
    return *this;
  }
  channels = rhs.channels;
  meta     = rhs.meta;
  return *this;
}

Table &TableGroup::getChannelAt(joda::settings::ChannelIndex key, const std::string &channelName) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  if(!channels.contains(key)) {
    channels[key].setTableName(channelName);
  }

  return channels.at(key);
}

const Table &TableGroup::getChannelAt(joda::settings::ChannelIndex key) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  if(channels.contains(key)) {
    return channels.at(key);
  }
  throw std::invalid_argument("Table does not exist!");
}

bool TableGroup::containsTable(joda::settings::ChannelIndex key) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  return channels.contains(key);
}

bool TableGroup::containsInvalidChannel() const
{
  for(const auto &ch : channels) {
    auto [valid, _] = ch.second.getTableValidity();
    if(valid != func::ResponseDataValidity::VALID) {
      return true;
    }
  }
  return false;
}

bool TableGroup::containsInvalidChannelWhereOneInvalidatesTheWholeImage() const
{
  for(const auto &[_, ch] : channels) {
    auto [valid, invalidAll] = ch.getTableValidity();
    if(valid != func::ResponseDataValidity::VALID && invalidAll) {
      return true;
    }
  }
  return false;
}

std::map<joda::settings::ChannelIndex, Table> &TableGroup::getChannels() const
{
  return channels;
}

///
/// \brief      Save results to file
/// \author     Joachim Danmayr
///
void TableWorkBook::saveToFile(std::string filename) const
{
  if(!filename.empty()) {
    nlohmann::json json = *this;

    if(!filename.ends_with(".json")) {
      filename += ".json";
    }
    std::ofstream out(filename);
    out << json.dump(2);
    out.close();

    // Write BSON to file
    auto bsonDoc = nlohmann::json::to_bson(json);
    std::ofstream output_file(filename + ".bson", std::ios::binary);
    if(!output_file.is_open()) {
      std::cerr << "Error opening output file" << std::endl;
    }
    output_file.write(reinterpret_cast<const char *>(bsonDoc.data()), bsonDoc.size());
    output_file.close();
  }
}

///
/// \brief      Load results from file
/// \author     Joachim Danmayr
///
void TableWorkBook::loadFrom(const std::string &filename)
{
  std::ifstream ifs(filename);
  *this = nlohmann::json::parse(ifs);
}

}    // namespace joda::results
