///
/// \file      store_to_db.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///

///

#pragma once

#include "backend/commands/command.hpp"
#include "backend/commands/objects_list.hpp"
#include "backend/processor/process_context.hpp"
#include <duckdb/main/appender.hpp>
#include "store_to_db_settings.hpp"

namespace joda::cmd {

///
/// \class      Classifier
/// \author     Joachim Danmayr
/// \brief      Classify objects in a grayscal image.
///             Each gray value is used for one object class.
///
class StoreToDb : public Command
{
public:
  /////////////////////////////////////////////////////
  StoreToDb(const StoreToDbSettings &);
  void execute(processor::ProcessContext &context, processor::ProcessorMemory &memory, cv::Mat &image,
               ObjectsListMap &result) override;

private:
  /////////////////////////////////////////////////////
  struct DetailReportAdder
  {
    std::shared_ptr<duckdb::Connection> connection;
    std::shared_ptr<duckdb::Appender> objects;
    std::shared_ptr<duckdb::Appender> imageStats;
  };

  /////////////////////////////////////////////////////
  auto prepareDetailReportAdding(processor::ProcessContext &context) -> DetailReportAdder;
  void appendToDetailReport(const DetailReportAdder &appender, processor::ProcessContext &context,
                            ObjectsListMap &result);

  void writePredatedData(const DetailReportAdder &adders);

  /////////////////////////////////////////////////////
  const StoreToDbSettings &mSettings;
};

}    // namespace joda::cmd
