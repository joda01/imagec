///
/// \file      results.cpp
/// \author    Joachim Danmayr
/// \date      2024-05-18
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "analyzer.hpp"
namespace joda::results {

Analyzer::Analyzer(const std::filesystem::path &databasePath) : mDatabase(databasePath)
{
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getImagesForJob(const std::string &jobId) -> std::vector<db::ImageMeta>
{
  {
    std::unique_ptr<duckdb::QueryResult> result = mDatabase.select("SELECT * FROM job");
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    std::cout << result->ToString() << std::endl;
    while(true) {
      auto dataChunk = result->Fetch();

      if(!dataChunk || dataChunk->size() == 0) {
        break;    // No more data
      }
      std::cout << dataChunk->ToString() << std::endl;
    }
  }

  {
    std::unique_ptr<duckdb::QueryResult> result =
        mDatabase.select("SELECT * FROM image WHERE job_id=?", duckdb::Value::UUID(jobId));
    if(result->HasError()) {
      throw std::invalid_argument(result->GetError());
    }

    std::cout << result->ToString() << std::endl;
    while(true) {
      auto dataChunk = result->Fetch();

      if(!dataChunk || dataChunk->size() == 0) {
        break;    // No more data
      }
      std::cout << dataChunk->ToString() << std::endl;
    }
  }
  return {};
}

///
/// \brief      Create control image
/// \author     Joachim Danmayr
///
auto Analyzer::getChannelsForImage(const std::string &jobId, uint32_t imageId) -> std::vector<db::ChannelMeta>
{
  return {};
}

}    // namespace joda::results
