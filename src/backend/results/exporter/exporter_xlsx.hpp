///
/// \file      exporter_xlsx.hpp
/// \author    Joachim Danmayr
/// \date      2024-05-27
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     XLSX exporter
///

#pragma once

#include "backend/results/table/table.hpp"

namespace joda::results::exporter {

class ExporterXlsx
{
public:
  static void startExport(const joda::results::Table &table, std::string outputFileName);
};

}    // namespace joda::results::exporter
