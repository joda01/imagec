

#include "exporter.hpp"
#include <cstddef>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/enums/enums_classes.hpp"
#include "backend/enums/enums_clusters.hpp"
#include "backend/helper/database/plugins/helper.hpp"
#include "backend/helper/database/plugins/stats_for_image.hpp"
#include "backend/helper/database/plugins/stats_for_plate.hpp"
#include "backend/helper/database/plugins/stats_for_well.hpp"

namespace joda::db {

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::startExport(const Settings &settings, const std::string &outputFileName)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  auto workbookSettings = createWorkBook(outputFileName);
  switch(settings.exportType) {
    case Settings::ExportType::HEATMAP:
      createHeatmapSummary(workbookSettings, settings);
      break;
    case Settings::ExportType::LIST:
      createListSummary(workbookSettings, settings);
      break;
  }
  workbook_close(workbookSettings.workbook);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::createHeatmapSummary(WorkBook &workbookSettings, const Settings &settings)
{
  const size_t MAX_WORKSHETT_NAME_LENGTH = 30;

  for(const auto &[clusterId, imageChannel] : settings.clustersToExport) {
    std::string worksheetName       = imageChannel.name;
    std::string worksheetNameSuffix = "(" + std::to_string(static_cast<uint16_t>(clusterId)) + ")";
    // Excel only allows 31 Chars as worksheet name -> we have to limit this
    size_t leftChars = MAX_WORKSHETT_NAME_LENGTH - worksheetNameSuffix.size();
    if(worksheetName.size() > leftChars) {
      worksheetName.resize(leftChars);
    }

    lxw_worksheet *worksheet = workbook_add_worksheet(
        workbookSettings.workbook, static_cast<std ::string>(worksheetName + worksheetNameSuffix).data());

    Pos offsets;
    for(const auto &[classId, className] : imageChannel.classes) {
      for(const auto &[measureChannelId, statsIn] : imageChannel.measureChannels) {
        for(const auto stats : statsIn) {
          auto generate = [&, clusterId = clusterId, classId = classId, className = className,
                           measureChannelId =
                               measureChannelId](uint32_t cStack, const std::string &crossChannelChannelCName,
                                                 std::pair<enums::ClusterId, std::string> crossChannelCluster,
                                                 std::pair<enums::ClassId, std::string> crossChannelClass) {
            table::Table table;
            auto filter = joda::db::QueryFilter{.analyzer                = &settings.analyzer,
                                                .plateRows               = settings.plateRows,
                                                .plateCols               = settings.plateCols,
                                                .plateId                 = settings.plateId,
                                                .actGroupId              = settings.groupId,
                                                .actImageId              = settings.imageId,
                                                .classId                 = classId,
                                                .className               = className,
                                                .measurementChannel      = measureChannelId,
                                                .stats                   = stats,
                                                .wellImageOrder          = settings.wellImageOrder,
                                                .densityMapAreaSize      = settings.heatmapAreaSize,
                                                .crossChanelStack_c      = cStack,
                                                .crossChannelStack_cName = crossChannelChannelCName,
                                                .crossChannelClusterId   = crossChannelCluster.first,
                                                .crossChannelClusterName = crossChannelCluster.second,
                                                .crossChannelClassId     = crossChannelClass.first,
                                                .crossChannelClassName   = crossChannelClass.second};

            switch(settings.exportDetail) {
              case Settings::ExportDetail::PLATE:
                table = joda::db::StatsPerPlate::toHeatmap(filter);

                break;
              case Settings::ExportDetail::WELL:
                table = joda::db::StatsPerGroup::toHeatmap(filter);
                break;
              case Settings::ExportDetail::IMAGE:
                table = joda::db::StatsPerImage::toHeatmap(filter);
                break;
            }

            paintPlateBorder(worksheet, table.getRows(), table.getCols(), offsets.row, workbookSettings.header,
                             workbookSettings.merge_format, workbookSettings.numberFormat, createHeader(filter));
            offsets = paintHeatmap(workbookSettings, worksheet, table, offsets.row);
            offsets.row += 4;
          };

          if(getType(measureChannelId) == joda::db::MeasureType::INTENSITY) {
            for(const auto &[cStack, name] : settings.crossChannelStacksC) {
              generate(cStack, name, {}, {});
            }
          } else if(getType(measureChannelId) == joda::db::MeasureType::COUNT) {
            for(const auto &cluster : settings.crossChannelClusterIds) {
              for(const auto &classs : settings.crossChannelClassIds) {
                generate(0, "", cluster, classs);
              }
            }
          } else {
            generate(0, "", {}, {});
          }
        }
      }
    }
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::createListSummary(WorkBook &workbookSettings, const Settings &settings)
{
  const int ROW_OFFSET = 1;
  const int COL_OFFSET = 1;

  lxw_worksheet *worksheet = workbook_add_worksheet(workbookSettings.workbook, "overview");
  int colOffset            = 0;
  for(const auto &[clusterId, imageChannel] : settings.clustersToExport) {
    int tmpCols = 0;
    for(const auto &[classId, className] : imageChannel.classes) {
      for(const auto &[measureChannelId, statsIn] : imageChannel.measureChannels) {
        for(const auto stats : statsIn) {
          auto generate = [&, clusterId = clusterId, classId = classId, className = className,
                           measureChannelId =
                               measureChannelId](uint32_t cStack, const std::string &crossChannelChannelCName,
                                                 std::pair<enums::ClusterId, std::string> crossChannelCluster,
                                                 std::pair<enums::ClassId, std::string> crossChannelClass) {
            auto filter = joda::db::QueryFilter{.analyzer                = &settings.analyzer,
                                                .plateRows               = settings.plateRows,
                                                .plateCols               = settings.plateCols,
                                                .plateId                 = settings.plateId,
                                                .actGroupId              = settings.groupId,
                                                .actImageId              = settings.imageId,
                                                .classId                 = classId,
                                                .className               = className,
                                                .measurementChannel      = measureChannelId,
                                                .stats                   = stats,
                                                .wellImageOrder          = settings.wellImageOrder,
                                                .densityMapAreaSize      = settings.heatmapAreaSize,
                                                .crossChanelStack_c      = cStack,
                                                .crossChannelStack_cName = crossChannelChannelCName,
                                                .crossChannelClusterId   = crossChannelCluster.first,
                                                .crossChannelClusterName = crossChannelCluster.second,
                                                .crossChannelClassId     = crossChannelClass.first,
                                                .crossChannelClassName   = crossChannelClass.second};

            table::Table table;
            switch(settings.exportDetail) {
              case Settings::ExportDetail::PLATE:
                table = joda::db::StatsPerPlate::toTable(filter);
                break;
              case Settings::ExportDetail::WELL:
                table = joda::db::StatsPerGroup::toTable(filter);
                break;
              case Settings::ExportDetail::IMAGE:
                table = joda::db::StatsPerImage::toTable(filter);
                break;
            }

            for(int col = 0; col < table.getCols(); col++) {
              worksheet_write_string(worksheet, 1, colOffset + COL_OFFSET, table.getMutableColHeader()[col].data(),
                                     workbookSettings.header);

              for(int row = 0; row < table.getRows(); row++) {
                // Row header
                worksheet_write_string(worksheet, 1 + ROW_OFFSET + row, 0, table.getRowHeader(row).data(),
                                       workbookSettings.header);

                auto *format = workbookSettings.numberFormat;
                if(!table.data(row, col).isValid()) {
                  format = workbookSettings.numberFormatInvalid;
                }
                // Offset 2 because of title and plate numbering
                double val = table.data(row, col).getVal();
                worksheet_write_number(worksheet, 1 + ROW_OFFSET + row, colOffset + COL_OFFSET, val, format);
              }
              colOffset++;
              tmpCols++;
            }
          };

          if(getType(measureChannelId) == joda::db::MeasureType::INTENSITY) {
            for(const auto &[cStack, name] : settings.crossChannelStacksC) {
              generate(cStack, name, {}, {});
            }
          } else if(getType(measureChannelId) == joda::db::MeasureType::COUNT) {
            for(const auto &cluster : settings.crossChannelClusterIds) {
              for(const auto &classs : settings.crossChannelClassIds) {
                generate(0, "", cluster, classs);
              }
            }
          } else {
            generate(0, "", {}, {});
          }
        }
      }
    }
    worksheet_merge_range(worksheet, 0, colOffset + COL_OFFSET - tmpCols, 0, colOffset + COL_OFFSET - 1, "-",
                          workbookSettings.merge_format);
    worksheet_write_string(worksheet, 0, colOffset + COL_OFFSET - tmpCols, imageChannel.name.data(),
                           workbookSettings.header);
    colOffset++;
  }
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
BatchExporter::WorkBook BatchExporter::createWorkBook(std::string outputFileName)
{
  if(!outputFileName.ends_with(".xlsx")) {
    outputFileName += ".xlsx";
  }
  lxw_workbook *workbook = workbook_new(outputFileName.data());

  /* Add a format. */
  lxw_format *header = workbook_add_format(workbook);
  format_set_bold(header);
  format_set_pattern(header, LXW_PATTERN_SOLID);
  format_set_bg_color(header, 0x002242);
  format_set_font_color(header, 0xFFFFFF);
  format_set_border(header, LXW_BORDER_THIN);
  format_set_font_size(header, 10);

  // Had for cols with invalid data
  lxw_format *headerInvalid = workbook_add_format(workbook);
  format_set_bold(headerInvalid);
  format_set_pattern(headerInvalid, LXW_PATTERN_SOLID);
  format_set_bg_color(headerInvalid, 0x820000);
  format_set_font_color(headerInvalid, 0xFFFFFF);
  format_set_border(headerInvalid, LXW_BORDER_THIN);
  format_set_font_size(headerInvalid, 10);

  //
  lxw_format *imageHeaderHyperlinkFormat = workbook_add_format(workbook);
  format_set_bold(imageHeaderHyperlinkFormat);
  format_set_pattern(imageHeaderHyperlinkFormat, LXW_PATTERN_SOLID);
  format_set_bg_color(imageHeaderHyperlinkFormat, 0x002242);
  format_set_font_color(imageHeaderHyperlinkFormat, 0xFFFFFF);
  format_set_border(imageHeaderHyperlinkFormat, LXW_BORDER_THIN);
  format_set_underline(imageHeaderHyperlinkFormat, LXW_UNDERLINE_SINGLE);
  format_set_font_size(imageHeaderHyperlinkFormat, 10);

  //
  lxw_format *imageHeaderHyperlinkFormatInvalid = workbook_add_format(workbook);
  format_set_bold(imageHeaderHyperlinkFormatInvalid);
  format_set_pattern(imageHeaderHyperlinkFormatInvalid, LXW_PATTERN_SOLID);
  format_set_bg_color(imageHeaderHyperlinkFormatInvalid, 0x820000);
  format_set_font_color(imageHeaderHyperlinkFormatInvalid, 0xFFFFFF);
  format_set_border(imageHeaderHyperlinkFormatInvalid, LXW_BORDER_THIN);
  format_set_underline(imageHeaderHyperlinkFormatInvalid, LXW_UNDERLINE_SINGLE);
  format_set_font_size(imageHeaderHyperlinkFormatInvalid, 10);

  // Define the cell format for the merged cells.
  lxw_format *merge_format = workbook_add_format(workbook);
  format_set_align(merge_format, LXW_ALIGN_CENTER);
  format_set_align(merge_format, LXW_ALIGN_VERTICAL_CENTER);
  format_set_bold(merge_format);
  format_set_pattern(merge_format, LXW_PATTERN_SOLID);
  format_set_bg_color(merge_format, 0x002242);
  format_set_font_color(merge_format, 0xFFFFFF);
  format_set_border(merge_format, LXW_BORDER_THIN);
  format_set_font_size(merge_format, 10);

  lxw_format *headerBold = workbook_add_format(workbook);
  format_set_bold(headerBold);
  format_set_pattern(headerBold, LXW_PATTERN_SOLID);
  format_set_bg_color(headerBold, 0xFFFFFF);
  format_set_font_color(headerBold, 0x000000);
  format_set_border(headerBold, LXW_BORDER_THIN);
  format_set_bold(headerBold);
  format_set_font_size(headerBold, 10);

  lxw_format *fontNormal = workbook_add_format(workbook);
  format_set_font_size(fontNormal, 10);
  format_set_border(fontNormal, LXW_BORDER_THIN);
  format_set_pattern(fontNormal, LXW_PATTERN_SOLID);
  format_set_bg_color(fontNormal, 0xFFFFFF);
  format_set_font_color(fontNormal, 0x000000);

  // Number format
  lxw_format *numberFormat = workbook_add_format(workbook);
  format_set_num_format(numberFormat, "0.00");
  format_set_font_size(numberFormat, 10);

  // Number format invalid
  lxw_format *numberFormatInvalid = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalid, "0.00");
  format_set_font_size(numberFormatInvalid, 10);
  format_set_font_color(numberFormatInvalid, 0x820000);

  // Number format scientific
  lxw_format *numberFormatScientific = workbook_add_format(workbook);
  format_set_num_format(numberFormatScientific, "0.00E+00");
  format_set_font_size(numberFormatScientific, 10);
  format_set_align(numberFormatScientific, LXW_ALIGN_CENTER);
  format_set_align(numberFormatScientific, LXW_ALIGN_VERTICAL_CENTER);

  // Number format invalid scientific
  lxw_format *numberFormatInvalidScientific = workbook_add_format(workbook);
  numberFormatInvalidScientific             = workbook_add_format(workbook);
  format_set_num_format(numberFormatInvalidScientific, "0.00E+00");
  format_set_font_size(numberFormatInvalidScientific, 10);
  format_set_align(numberFormatInvalidScientific, LXW_ALIGN_CENTER);
  format_set_align(numberFormatInvalidScientific, LXW_ALIGN_VERTICAL_CENTER);
  // format_set_border(numberFormatInvalid, LXW_BORDER_THIN);                 // Set border style to thin
  format_set_diag_type(numberFormatInvalidScientific, LXW_DIAGONAL_BORDER_UP_DOWN);

  return WorkBook{workbook,
                  header,
                  headerInvalid,
                  imageHeaderHyperlinkFormat,
                  imageHeaderHyperlinkFormatInvalid,
                  merge_format,
                  headerBold,
                  fontNormal,
                  numberFormat,
                  numberFormatInvalid,
                  numberFormatScientific,
                  numberFormatInvalidScientific};
}

///
/// \brief      Paint the borders of the heatmap
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void BatchExporter::paintPlateBorder(lxw_worksheet *sheet, int64_t rows, int64_t cols, int32_t rowOffset,
                                     lxw_format *header, lxw_format *numberFormat, lxw_format *mergeFormat,
                                     const std::string &title)
{
  const int32_t ROW_OFFSET       = 2;
  const int32_t HEADER_CELL_SIZE = 15;
  const int32_t CELL_SIZE        = 60;

  ///////////////////////////7
  lxw_conditional_format *condFormat = new lxw_conditional_format();
  condFormat->type                   = LXW_CONDITIONAL_3_COLOR_SCALE;
  condFormat->format                 = numberFormat;
  condFormat->min_color              = 0x63BE7B;
  condFormat->min_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MINIMUM;
  condFormat->mid_color              = 0xFFEB84;
  condFormat->mid_rule_type          = LXW_CONDITIONAL_RULE_TYPE_PERCENTILE;
  condFormat->mid_value              = 50;
  condFormat->max_color              = 0xF8696B;
  condFormat->max_rule_type          = LXW_CONDITIONAL_RULE_TYPE_MAXIMUM;
  ///////////////////

  // Title
  worksheet_merge_range(sheet, rowOffset, 1, rowOffset, cols, "-", mergeFormat);
  worksheet_write_string(sheet, rowOffset, 1, title.data(), header);

  // Column
  worksheet_set_column_pixels(sheet, 0, 0, HEADER_CELL_SIZE, NULL);
  worksheet_set_column_pixels(sheet, cols + 1, cols + 1, HEADER_CELL_SIZE, NULL);

  for(int col = 1; col < cols + 1; col++) {
    worksheet_set_column_pixels(sheet, col, col, CELL_SIZE, NULL);
    worksheet_write_string(sheet, rowOffset + 1, col, std::to_string(col).data(), header);
    worksheet_write_string(sheet, rows + rowOffset + 2, col, std::to_string(col).data(), header);
  }

  // Row
  worksheet_set_row_pixels(sheet, rowOffset + ROW_OFFSET, HEADER_CELL_SIZE, NULL);
  for(int row = 0; row < rows; row++) {
    char toWrt[2];
    toWrt[0] = (row) + 'A';
    toWrt[1] = 0;

    worksheet_set_row_pixels(sheet, row + rowOffset + ROW_OFFSET, CELL_SIZE, NULL);
    worksheet_write_string(sheet, row + rowOffset + ROW_OFFSET, 0, toWrt, header);
    worksheet_write_string(sheet, row + rowOffset + ROW_OFFSET, cols + 1, toWrt, header);
  }

  worksheet_conditional_format_range(sheet, rowOffset + ROW_OFFSET, 1, rowOffset + rows + ROW_OFFSET, 1 + cols,
                                     condFormat);
}

BatchExporter::Pos BatchExporter::paintHeatmap(const WorkBook &workbookSettings, lxw_worksheet *worksheet,
                                               const joda::table::Table &table, uint32_t rowOffset)
{
  const int32_t ROW_OFFSET = 2;

  for(int row = 0; row < table.getRows(); row++) {
    for(int col = 0; col < table.getCols(); col++) {
      auto *format = workbookSettings.numberFormatScientific;
      if(!table.data(row, col).isValid()) {
        format = workbookSettings.numberFormatInvalidScientific;
      }
      // Offset 2 because of title and plate numbering
      worksheet_write_number(worksheet, ROW_OFFSET + row + rowOffset, 1 + col, table.data(row, col).getVal(), format);
    }
  }

  return BatchExporter::Pos{.row = table.getRows() + rowOffset + ROW_OFFSET, .col = table.getCols()};
}

}    // namespace joda::db
