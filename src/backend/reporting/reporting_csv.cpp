#include "reporting.h"



namespace joda::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
void Table::flushReportToFile(std::string_view fileName) const {
  // Set local to C to force use dots
  try {
    auto *set = std::setlocale(LC_NUMERIC, "C");
  } catch (const std::exception &ex) {
    std::cout << "Could not set local" << std::endl;
  }

  std::ofstream outFile;
  outFile.open(fileName.data());

  //
  // Write column header
  //
  std::string rowBuffer;
  int64_t columns =
      std::max(getNrOfColumns(), static_cast<int64_t>(mColumnName.size()));
  for (int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if (0 == colIdx) {
      rowBuffer += CSV_SEPARATOR;
    }

    if (mColumnName.contains(colIdx)) {
      rowBuffer += std::string(mColumnName.at(colIdx)) + CSV_SEPARATOR;
    } else {
      rowBuffer += std::to_string(colIdx) + CSV_SEPARATOR;
    }
  }
  if (!rowBuffer.empty()) {
    rowBuffer.pop_back(); // Remove trailing CSV_SEPARATOR
  }
  rowBuffer += "\n";
  outFile << rowBuffer;

  //
  // Write table data
  //
  for (int64_t rowIdx = 0; rowIdx < getNrOfRows(); rowIdx++) {
    std::string rowBuffer = "";
    for (int64_t colIdx = 0; colIdx < columns; colIdx++) {
      //
      // Write row data
      //
      if (0 == colIdx) {
        //
        // Write row header
        //
        if (mRowNames.contains(rowIdx)) {
          rowBuffer += std::string(mRowNames.at(rowIdx)) + CSV_SEPARATOR;
        } else {
          rowBuffer += std::to_string(rowIdx) + CSV_SEPARATOR;
        }
      }

      if (mTable.contains(colIdx) && mTable.at(colIdx).contains(rowIdx)) {
        if (!mTable.at(colIdx).at(rowIdx).validity.has_value()) {
          rowBuffer += std::to_string(mTable.at(colIdx).at(rowIdx).value) +
                       CSV_SEPARATOR;
        } else {
          rowBuffer +=
              validityToString(mTable.at(colIdx).at(rowIdx).validity.value()) +
              CSV_SEPARATOR;
        }
      } else {
        // Empty table entry
        rowBuffer += CSV_SEPARATOR;
      }
    }

    if (!rowBuffer.empty()) {
      rowBuffer.pop_back(); // Remove trailing CSV_SEPARATOR
    }
    rowBuffer += "\n";
    outFile << rowBuffer;
  }

  //
  // Write separator
  //
  rowBuffer = "";
  for (int64_t colIdx = 0; colIdx < columns; colIdx++) {
    if (0 == colIdx) {
      rowBuffer += CSV_SEPARATOR;
    }
    // Delimiter before summary stars
    rowBuffer += std::string("   ") + CSV_SEPARATOR;
  }
  if (!rowBuffer.empty()) {
    rowBuffer.pop_back(); // Remove trailing CSV_SEPARATOR
  }
  rowBuffer += "\n";
  outFile << rowBuffer;

  //
  // Write table statistics
  //
  for (int n = 0; n < Statistics::NR_OF_VALUE; n++) {
    std::string rowBuffer = Statistics::getStatisticsTitle()[n] + CSV_SEPARATOR;
    for (int64_t colIdx = 0; colIdx < columns; colIdx++) {
      if (mStatistics.contains(colIdx)) {
        auto statistics = mStatistics.at(colIdx);
        rowBuffer +=
            std::to_string(statistics.getStatistics()[n]) + CSV_SEPARATOR;
      } else {
        rowBuffer += CSV_SEPARATOR;
      }
    }

    rowBuffer.pop_back(); // Remove trailing CSV_SEPARATOR
    rowBuffer += "\n";
    outFile << rowBuffer;
  }

  outFile.close();
}

}
