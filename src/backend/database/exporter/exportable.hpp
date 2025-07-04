///
/// \file      exportable.hpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <qtablewidget.h>
#include <xlsxwriter/workbook.h>
#include <string>

namespace joda::exporter {

class Exportable
{
public:
  [[nodiscard]] virtual const std::string &getTitle() const           = 0;
  [[nodiscard]] virtual auto getTable() const -> const QTableWidget & = 0;
};
}    // namespace joda::exporter
