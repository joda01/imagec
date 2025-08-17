///
/// \file      exporter_xlsx.cpp
/// \author    Joachim Danmayr
/// \date      2025-07-03
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "exporter_xlsx.hpp"
#include <xlsxwriter/worksheet.h>
#include <string>
#include <variant>
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"

namespace joda::exporter::xlsx {

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Exporter::startExport(const std::vector<const Exportable *> &data, const settings::AnalyzeSettings &analyzeSettings, const std::string &jobName,
                           std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished,
                           const std::string &outputFileName)
{
  setlocale(LC_NUMERIC, "C");    // Needed for correct comma in libxlsx
  auto workbookSettings = createWorkBook(outputFileName);
  createAnalyzeSettings(workbookSettings, analyzeSettings, jobName, timeStarted, timeFinished);

  int32_t index = 1;
  for(const auto *dataToWrite : data) {
    writeWorkSheet(workbookSettings, dataToWrite, index);
    index++;
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
std::string Exporter::prepareSheetName(std::string name)
{
  if(name.size() > 28) {
    name = name.substr(0, 28);
  }
  name.erase(std::remove_if(name.begin(), name.end(),
                            [](char c) { return (std::isalnum(static_cast<unsigned char>(c)) == 0) && static_cast<unsigned char>(c) != ' '; }),
             name.end());
  helper::stringReplace(name, " ", "_");
  return name;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Exporter::writeWorkSheet(const Exporter::WorkBook &workbookSettings, const Exportable *data, int32_t index)
{
  std::string name = prepareSheetName(data->getTitle());
  name += "_" + std::to_string(index);
  auto *worksheet = workbook_add_worksheet(workbookSettings.workbook, name.data());
  if(worksheet == nullptr) {
    joda::log::logWarning("Could not export sheet >" + name + "<.");
    return;
  }

  //
  // Write table header
  //
  const auto &table = data->getTable();
  for(uint16_t n = 0; n < table.getNrOfCols(); n++) {
    worksheet_write_string(worksheet, 0, n + 1, table.getColHeader(n).createHeader().data(), workbookSettings.header);
  }

  for(uint32_t n = 0; n < table.getNrOfRows(); n++) {
    worksheet_write_string(worksheet, n + 1, 0, table.getRowHeader(n).data(), workbookSettings.header);
  }

  for(uint32_t row = 0; row < table.getNrOfRows(); row++) {
    for(uint16_t col = 0; col < table.getNrOfCols(); col++) {
      const auto &item = table.data(row, col);
      if(item == nullptr) {
        worksheet_write_blank(worksheet, row + 1, 1 + col, workbookSettings.numberFormatInvalid);
      } else {
        auto val = item->getValAsVariant(table.getColHeader(col).measureChannel);
        if(std::holds_alternative<std::string>(val)) {
          worksheet_write_string(worksheet, row + 1, 1 + col, std::get<std::string>(val).data(), workbookSettings.idFormat);
        } else {
          auto valD = std::get<double>(val);
          if(valD != valD) {
            worksheet_write_string(worksheet, row + 1, 1 + col, "", workbookSettings.idFormat);
          } else {
            worksheet_write_number(worksheet, row + 1, 1 + col, valD, workbookSettings.numberFormat);
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
Exporter::WorkBook Exporter::createWorkBook(std::string outputFileName)
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

  lxw_format *idFormat = workbook_add_format(workbook);
  format_set_bold(idFormat);
  format_set_font_size(idFormat, 10);

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
  format_set_font_strikeout(numberFormatInvalid);

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
                  idFormat,
                  fontNormal,
                  numberFormat,
                  numberFormatInvalid,
                  numberFormatScientific,
                  numberFormatInvalidScientific};
}

///
/// \brief      Add analyze settings to excel
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Exporter::createAnalyzeSettings(WorkBook &workbookSettings, const settings::AnalyzeSettings &settings, const std::string &jobName,
                                     std::chrono::system_clock::time_point timeStarted, std::chrono::system_clock::time_point timeFinished)
{
  auto *sheet = workbook_add_worksheet(workbookSettings.workbook, "Analyze");

  worksheet_set_column_pixels(sheet, 0, 0, 200, nullptr);
  worksheet_set_column_pixels(sheet, 1, 1, 400, nullptr);

  uint32_t rowOffset = 0;
  auto addTitle      = [&sheet, &rowOffset, &workbookSettings](const std::string &title) {
    rowOffset++;
    worksheet_merge_range(sheet, rowOffset, 0, rowOffset, 1, "-", workbookSettings.headerBold);
    worksheet_write_string(sheet, rowOffset, 0, title.data(), workbookSettings.headerBold);
    rowOffset++;
  };

  auto addElement = [&sheet, &rowOffset, &workbookSettings](const std::string &title, const std::string &value) {
    worksheet_write_string(sheet, rowOffset, 0, title.data(), workbookSettings.headerBold);
    worksheet_write_string(sheet, rowOffset, 1, value.data(), workbookSettings.fontNormal);
    rowOffset++;
  };

  addTitle("Date");
  addElement("Started at", joda::helper::timepointToIsoString(timeStarted));
  addElement("Finished at", joda::helper::timepointToIsoString(timeFinished));
  addElement("Duration at", joda::helper::getDurationAsString(timeStarted, timeFinished));

  addTitle("ImageC");
  addElement("Version", settings.imagecMeta.imagecVersion);
  addElement("Build", settings.imagecMeta.buildTime);

  addTitle("Classes");
  for(const auto &classs : settings.projectSettings.classification.classes) {
    nlohmann::json classIdStr = static_cast<enums::ClassId>(classs.classId);
    addElement(std::string(classIdStr), classs.name);
  }

  addTitle("Pipelines");
  for(const auto &pipeline : settings.pipelines) {
    addElement("Name", pipeline.meta.name);

    nlohmann::json classId = static_cast<enums::ClassId>(pipeline.pipelineSetup.defaultClassId);
    addElement("Default class", std::string(classId));
  }

  addTitle("Project settings");
  addElement("Scientist", settings.projectSettings.address.firstName + " " + settings.projectSettings.address.lastName);
  addElement("Organization", settings.projectSettings.address.organization);
  addElement("Experiment ID", settings.projectSettings.experimentSettings.experimentId);
  addElement("Experiment name", settings.projectSettings.experimentSettings.experimentName);
  addElement("Job name", jobName);
  addElement("Notes", settings.projectSettings.experimentSettings.notes);
  addElement("Working directory", settings.projectSettings.workingDirectory);

  const auto &plate = settings.projectSettings.plate;
  addTitle("Plate " + std::to_string(plate.plateId));
  addElement("Filename regex", plate.filenameRegex);
  addElement("Image folder", plate.imageFolder);
  addElement("Well order", joda::settings::vectorToString(plate.plateSetup.wellImageOrder));
  nlohmann::json groupBy = static_cast<enums::GroupBy>(plate.groupBy);
  addElement("Group by", std::string(groupBy));
}

}    // namespace joda::exporter::xlsx
