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

TableWorkbook::TableWorkbook()
{
}

Table &TableWorkbook::getTableAt(joda::settings::ChannelIndex key, const std::string &channelName) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  if(!tables.contains(key)) {
    tables[key].setTableName(channelName);
  }

  return tables.at(key);
}

const Table &TableWorkbook::getTableAt(joda::settings::ChannelIndex key) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  if(tables.contains(key)) {
    return tables.at(key);
  }
  throw std::invalid_argument("Table does not exist!");
}

bool TableWorkbook::containsTable(joda::settings::ChannelIndex key) const
{
  std::lock_guard<std::mutex> lock(mAccessMutex);
  return tables.contains(key);
}

bool TableWorkbook::containsInvalidChannel() const
{
  for(const auto &ch : tables) {
    auto [valid, _] = ch.second.getTableValidity();
    if(valid != func::ResponseDataValidity::VALID) {
      return true;
    }
  }
  return false;
}

bool TableWorkbook::containsInvalidChannelWhereOneInvalidatesTheWholeImage() const
{
  for(const auto &[_, ch] : tables) {
    auto [valid, invalidAll] = ch.getTableValidity();
    if(valid != func::ResponseDataValidity::VALID && invalidAll) {
      return true;
    }
  }
  return false;
}

std::map<joda::settings::ChannelIndex, Table> &TableWorkbook::getTables() const
{
  return tables;
}

}    // namespace joda::results
