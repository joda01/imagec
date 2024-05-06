

#include "reporting_overview_xlsx.hpp"
#include <xlsxwriter/worksheet.h>
#include <string>
#include <unordered_map>
#include "backend/image_processing/roi/roi.hpp"
#include "backend/pipelines/reporting/reporting_defines.hpp"
#include "backend/results/results_defines.hpp"
#include "backend/results/results_math.hpp"
#include "xlsxwriter.h"

namespace joda::pipeline::reporting {

///
/// \brief      Writes the report to a CSV file
/// \author     Joachim Danmayr
/// \param[in]  fileName  Name of the output report file
///
std::tuple<int, int> OverviewReport::writeReport(const joda::results::ReportingSettings &reportingSettings,
                                                 const joda::results::Channel &results, const std::string &headerText,
                                                 const std::string &jobName, int /*sheetRowOffset*/, int rowOffset,
                                                 int startRow, lxw_worksheet *worksheet, lxw_format *header,
                                                 lxw_format *headerInvalid, lxw_format *merge_format,
                                                 lxw_format *numberFormat, lxw_format *numberFormatInvalid,
                                                 lxw_format *imageHeaderHyperlinkFormat,
                                                 lxw_format *imageHeaderHyperlinkFormatInvalid)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx

  //
  // Calc stats
  //
  auto stats = calcStats(results);

  //
  // Write header text
  //
  if(rowOffset == startRow) {
    worksheet_merge_range(worksheet, rowOffset, 0, rowOffset, 1, "-", merge_format);
    worksheet_write_string(worksheet, rowOffset, 0, headerText.data(), header);
  }

  //
  // Write statistics data
  //
  std::map<results::MeasureChannelKey, uint64_t> rowIndexes;
  const int SHEET_COL_OFFSET      = 2;
  const int SHEET_COL_OFFSET_STAT = 4;

  int sheetRowIdx = 1;

  for(auto const &[measureCh, measureChMeta] : results.getMeasuredChannels()) {
    int sheetColumnIdx = 0;
    if(reportingSettings.overview.measureChannels.contains(measureCh.getMeasreChannelAndStatsCombo())) {
      worksheet_write_string(worksheet, rowOffset + sheetRowIdx, 1, measureChMeta.name.data(), header);
      worksheet_set_column(worksheet, rowOffset + sheetRowIdx, 1, 15, NULL);

      rowIndexes.emplace(measureCh, sheetRowIdx);

      //
      // Write statistics
      //
      auto writeStat = [&sheetColumnIdx, &rowOffset, &startRow, &worksheet, &sheetRowIdx, &SHEET_COL_OFFSET, &header,
                        &stats, &numberFormat](const results::MeasureChannelKey &measureCh, results::MeasureStat stat) {
        auto statKey = results::MeasureChannelKey{measureCh, measureCh.getMeasureStats01(), stat};

        if(stats.measurements.contains(statKey)) {
          if(rowOffset == startRow) {
            worksheet_write_string(worksheet, rowOffset, sheetColumnIdx + SHEET_COL_OFFSET,
                                   statKey.measurementStatsToString().data(), header);
          }

          auto val = stats.measurements[statKey];
          worksheet_write_number(worksheet, rowOffset + sheetRowIdx, sheetColumnIdx + SHEET_COL_OFFSET, val,
                                 numberFormat);
          sheetColumnIdx++;
        }
      };
      writeStat(measureCh, results::MeasureStat::AVG);
      writeStat(measureCh, results::MeasureStat::MIN);
      writeStat(measureCh, results::MeasureStat::MAX);
      // writeStat(measureCh, results::MeasureStat::SUM);
      // writeStat(measureCh, results::MeasureStat::CNT);
      writeStat(measureCh, results::MeasureStat::STD_DEV);

      sheetRowIdx++;
    }
  }

  //
  // Write image data
  //
  //
  int headerColumnRowOffset = SHEET_COL_OFFSET_STAT + SHEET_COL_OFFSET + 1;
  for(const auto &[imageKey, image] : results.getObjects()) {
    auto imgMeta = image.getImageMeta();
    if(imgMeta.has_value()) {
      //
      // Write header (image name)
      //
      if(rowOffset == startRow) {
        auto rowName         = imgMeta->imageFileName;
        std::string filePath = "external:.\\images/" + rowName + "/results_image_" + jobName + ".xlsx";
        lxw_format *format   = imageHeaderHyperlinkFormat;
        worksheet_write_url(worksheet, rowOffset, headerColumnRowOffset, filePath.data(), format);
        worksheet_write_string(worksheet, rowOffset, headerColumnRowOffset, rowName.data(), format);
        worksheet_set_column(worksheet, rowOffset, headerColumnRowOffset, 15, NULL);
      }

      //
      // Write image table data
      //
      for(const auto &[measureCh, val] : image.getMeasurements()) {
        if(reportingSettings.overview.measureChannels.contains(measureCh.getMeasreChannelAndStatsCombo())) {
          if(rowIndexes.contains(measureCh)) {
            auto sheetRowIdxOfVal = rowIndexes[measureCh];

            auto *format = numberFormat;
            if(!image.getObjectMeta().valid) {
              format = numberFormatInvalid;
            }
            if(std::holds_alternative<double>(val.getVal())) {
              worksheet_write_number(worksheet, rowOffset + sheetRowIdxOfVal, headerColumnRowOffset,
                                     std::get<double>(val.getVal()), format);

            } else if(std::holds_alternative<joda::func::ParticleValidity>(val.getVal())) {
              worksheet_write_string(worksheet, rowOffset + sheetRowIdxOfVal, headerColumnRowOffset,
                                     validityToString(std::get<joda::func::ParticleValidity>(val.getVal())).data(),
                                     NULL);
            }
          }
        }
      }
      headerColumnRowOffset++;
    }
  }

  //
  // Write channel Name
  //
  //
  worksheet_merge_range(worksheet, rowOffset + 1, 0, rowOffset + sheetRowIdx - 1, 0, "-", merge_format);
  if(!results.getChannelMeta().name.empty()) {
    worksheet_write_string(worksheet, rowOffset + 1, 0, results.getChannelMeta().name.data(), merge_format);
  } else {
    worksheet_write_string(worksheet, rowOffset + 1, 0, std::to_string(rowOffset + 1).data(), merge_format);
  }

  return {headerColumnRowOffset, rowOffset + sheetRowIdx};
}
}    // namespace joda::pipeline::reporting
