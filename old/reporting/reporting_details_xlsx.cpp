

#include <xlsxwriter/worksheet.h>
#include <string>
#include "backend/pipelines/reporting/reporting_defines.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/results/results_math.hpp"
#include "reporting_details.xlsx.hpp"

namespace joda::pipeline::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> DetailReport::writeReport(const joda::results::ReportingSettings &reportingSettings,
                                               const joda::results::Channel &results, int colOffset, int /*rowOffset*/,
                                               lxw_worksheet *worksheet, lxw_format *header, lxw_format *headerInvalid,
                                               lxw_format *merge_format, lxw_format *numberFormat,
                                               lxw_format *numberFormatInvalid)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  int ROW_OFFSET = 2;
  int ROW_STATS  = 6;
  int COL_OFFSET = colOffset + 1;

  auto stats = calcStats(results);

  std::map<results::MeasureChannelKey, uint64_t> colIndexes;
  //
  // Write header and stats
  //

  uint32_t nrOfWrittenCols = 0;
  {
    uint32_t sheetColIdx = 0;
    for(auto const &[measureCh, measureChMeta] : results.getMeasuredChannels()) {
      int rowIdxStat = 0;
      if(reportingSettings.detail.measureChannels.contains(measureCh.getMeasureChannel())) {
        if(results.getChannelMeta().valid == joda::func::ResponseDataValidity::VALID) {
          worksheet_write_string(worksheet, 1, sheetColIdx + COL_OFFSET, measureChMeta.name.data(), header);
        } else {
          worksheet_write_string(worksheet, 1, sheetColIdx + COL_OFFSET, measureChMeta.name.data(), headerInvalid);
        }

        colIndexes.emplace(measureCh, sheetColIdx);

        //
        // Write statistics
        //
        auto writeStat = [&rowIdxStat, &worksheet, &ROW_OFFSET, &header, &stats, &sheetColIdx, &COL_OFFSET,
                          &numberFormat](const results::MeasureChannelKey &measureCh, results::MeasureStat stat) {
          auto statKey = results::MeasureChannelKey{measureCh, stat};
          worksheet_write_string(worksheet, ROW_OFFSET + rowIdxStat, 0, statKey.measurementStatsToString().data(),
                                 header);
          auto avg = stats.measurements[statKey];
          worksheet_write_number(worksheet, ROW_OFFSET + rowIdxStat, sheetColIdx + COL_OFFSET, avg, numberFormat);
          rowIdxStat++;
        };
        writeStat(measureCh, results::MeasureStat::AVG);
        writeStat(measureCh, results::MeasureStat::MIN);
        writeStat(measureCh, results::MeasureStat::MAX);
        writeStat(measureCh, results::MeasureStat::SUM);
        writeStat(measureCh, results::MeasureStat::CNT);
        writeStat(measureCh, results::MeasureStat::STD_DEV);

        worksheet_set_column(worksheet, sheetColIdx + COL_OFFSET, sheetColIdx + COL_OFFSET, 10, NULL);
        sheetColIdx++;
      }
    }
    nrOfWrittenCols = sheetColIdx;
  }

  worksheet_set_column(worksheet, 0, 0, 10, NULL);

  //
  // Write name
  //
  worksheet_merge_range(worksheet, 0, COL_OFFSET, 0, COL_OFFSET + nrOfWrittenCols - 1, "-", merge_format);
  if(!results.getChannelMeta().name.empty()) {
    worksheet_write_string(worksheet, 0, COL_OFFSET, results.getChannelMeta().name.data(), header);
  } else {
    worksheet_write_string(worksheet, 0, COL_OFFSET, std::to_string(COL_OFFSET).data(), header);
  }

  //
  // Write table data
  //
  {
    int64_t rowIdx = ROW_STATS + 1;
    for(const auto &[objKey, obj] : results.getObjects()) {
      worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, 0, obj.getObjectMeta().name.data(), header);

      for(const auto &[measKey, val] : obj.getMeasurements()) {
        if(reportingSettings.detail.measureChannels.contains(measKey.getMeasureChannel())) {
          if(colIndexes.contains(measKey)) {
            auto sheetColIdx = colIndexes[measKey] + COL_OFFSET;

            auto *format = numberFormat;
            if(!obj.getObjectMeta().valid) {
              format = numberFormatInvalid;
            }
            if(std::holds_alternative<double>(val.getVal())) {
              worksheet_write_number(worksheet, ROW_OFFSET + rowIdx, sheetColIdx, std::get<double>(val.getVal()),
                                     format);

            } else if(std::holds_alternative<joda::func::ParticleValidity>(val.getVal())) {
              worksheet_write_string(worksheet, ROW_OFFSET + rowIdx, sheetColIdx,
                                     validityToString(std::get<joda::func::ParticleValidity>(val.getVal())).data(),
                                     NULL);
            }
          }
        }
      }
      rowIdx++;
    }
  }

  return {nrOfWrittenCols, 2};
}
}    // namespace joda::pipeline::reporting
