///
/// \file      Cli.cpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
///

#include "cli.hpp"
#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include "CLI/CLI.hpp"
#include "backend/database/query/filter.hpp"
#include "backend/helper/helper.hpp"
#include "backend/helper/logger/console_logger.hpp"
#include "backend/helper/random_name_generator.hpp"
#include "backend/settings/settings.hpp"
#include "controller/controller.hpp"
#include "ui/gui/helper/template_parser/template_parser.hpp"
#include <CLI/CLI.hpp>
#include "version.h"

namespace joda::ui::cli {

using namespace std::chrono_literals;

auto toFormatEnum(const std::string &type) -> exporter::xlsx::ExportSettings::ExportSettings::ExportFormat;
auto toStyleEnum(const std::string &format) -> exporter::xlsx::ExportSettings::ExportStyle;

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
Cli::Cli()
{
}

struct FileValidator : public CLI::Validator
{
  FileValidator(const std::string &endian)
  {
    name_ = "LOWER";
    func_ = [endian](const std::string &str) {
      int endianSize = endian.size();
      if(str.size() >= endianSize && str.substr(str.size() - endianSize) == endian) {
        return std::string();    // valid
      } else {
        return std::string("File must have a " + endian + " extension");
      }
    };
    description("(" + endian + ")");
  }
};

struct FileExistsValidator : public CLI::Validator
{
  FileExistsValidator()
  {
    name_ = "FILE";
    func_ = [&](const std::string &str) {
      auto ret = CLI::detail::check_path(str.c_str());
      if(ret == CLI::detail::path_type::nonexistent) {
        return "File does not exist.";
      }
      if(ret != CLI::detail::path_type::file) {
        return "This is not a file.";
      }
      return "";
    };
    description("FILE");
  }
};

struct DirectoryExistsValidator : public CLI::Validator
{
  DirectoryExistsValidator()
  {
    name_ = "DIRECTORY";
    func_ = [&](const std::string &str) {
      auto ret = CLI::detail::check_path(str.c_str());
      if(ret == CLI::detail::path_type::nonexistent) {
        return "Directory does not exist.";
      }
      if(ret != CLI::detail::path_type::directory) {
        return "This is not a directory.";
      }
      return "";
    };
    description("DIRECTORY");
  }
};

class NoOptionOptsFormatter : public CLI::Formatter
{
public:
  // Override and return an empty string to remove option metadata
  std::string make_option_opts(const CLI::Option *) const override
  {
    return "";
  }

  // std::string make_description(const CLI::App *app) const override
  // {
  //   return app->get_description() + "\n\n";    // no added newline
  // }
  //
  // std::string make_usage(const CLI::App *app, std::string name) const override
  // {
  //   return "Usage: " + name + " " + app->get_usage() + "[OPTIONS]";
  // }

  // std::string make_group(std::string group, bool is_positional, std::vector<const CLI::Option *> opts) const override
  //{
  //   if(group == "OPTIONS") {
  //     return "options:";    // lowercase
  //   }
  //   return CLI::Formatter::make_group(group, is_positional, opts);    // fallback to default
  // }
};

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
int Cli::startCommandLineController(int argc, char *argv[])
{
  std::cout << Version::getLogo();
  CLI::App app{"High throughput image processing application.", "imagec"};
  app.formatter(std::make_shared<NoOptionOptsFormatter>());
  // app.get_formatter()->column_width(40);
  // app.get_formatter()->label("REQUIRED", "*");

  // =====================================
  // Options
  // =====================================
  std::string logLevelStr = "info";
  app.add_option("-l,--log-level", logLevelStr, "Set log level (trace, debug, info, warning, error)")
      ->check(CLI::IsMember({"trace", "debug", "info", "warning", "error"}))
      ->default_val("info");

  // =====================================
  // Run subcommand
  // =====================================
  std::string projectFilePath;
  std::string workingDirectory;
  std::string jobName;
  auto *run = app.add_subcommand("run", "Run an analyzes");
  run->add_option("-p,--project", projectFilePath, "ImageC project settings file (*.icproj)")
      ->check(FileExistsValidator())
      ->check(FileValidator(".icproj"))
      ->required();
  run->add_option("-i,--input-folder", workingDirectory, "Images folder")->check(DirectoryExistsValidator())->required();
  run->add_option("-n,--job-name", jobName, "Job name (optional)");

  // =====================================
  // Export subcommand
  // =====================================
  std::string infile;
  std::string outfile;
  std::string format = "xlsx";
  std::string style  = "table";
  std::string outputTemplate;

  auto *export_cmd = app.add_subcommand("export", "Export processed data");
  export_cmd->add_option("-i,--infile", infile, "Input database file (*.icdb)")
      ->check(FileExistsValidator())
      ->check(FileValidator(".icdb"))
      ->required();
  export_cmd->add_option("-o,--outpath", outfile, "Output folder");
  export_cmd->add_option("-f,--format", format, "Output format (xlsx, r) [xlsx].")->check(CLI::IsMember({"xlsx", "r"}))->default_val("xlsx");
  export_cmd->add_option("-s,--style", style, "Output style (table, heatmap) [table].")
      ->check(CLI::IsMember({"table", "heatmap"}))
      ->default_val("table");
  export_cmd->add_option("-c,--columns", outputTemplate, "Output columns template file (*.ictemplcc)")
      ->check(FileExistsValidator())
      ->check(FileValidator(".ictemplcc"));
  auto *plateCmd = export_cmd->add_subcommand("plate", "Export plate view");
  auto *wellCmd  = export_cmd->add_subcommand("well", "Export well view");
  std::string wellId;
  wellCmd->add_option("--id", wellId, "Id of the well to export (0, 1, 2, 3,...)")->required()->default_str("0");

  auto *listCmd = export_cmd->add_subcommand("image", "Export list view");
  std::string imageName;
  std::string tStack = "0";
  listCmd->add_option("--name", imageName, "Name of the image to export the data for")->required();
  listCmd->add_option("--tstack", tStack, "Time stack index to export (0, 1, 2, 3,...) [0]")->default_str("0");

  // =====================================
  // Database view
  // =====================================
  std::string target;
  auto *databaseCmd = app.add_subcommand("database", "View database content");
  databaseCmd->add_option("-i,--infile", infile, "Input database file (*.icdb)")
      ->check(FileExistsValidator())
      ->check(FileValidator(".icdb"))
      ->required();
  auto *dbView = databaseCmd->add_subcommand("view", "View some content of the database");
  dbView->add_option("target", target, "Must be either 'wells' or 'images'")->required()->check(CLI::IsMember({"wells", "images"}));

  CLI11_PARSE(app, argc, argv);

  // =====================================
  // Set log level
  // =====================================
  setLogLevel(logLevelStr);

  // =====================================
  // Execute command
  // =====================================
  joda::ctrl::Controller::initApplication();
  mController = std::make_unique<joda::ctrl::Controller>();

  if(run->parsed()) {
    // Run logic
    startAnalyze(std::filesystem::path(projectFilePath), workingDirectory, jobName);
  } else if(export_cmd->parsed()) {
    // Export logic
    exporter::xlsx::ExportSettings::ExportView toExport;
    if(plateCmd->parsed()) {
      toExport = exporter::xlsx::ExportSettings::ExportView::PLATE;
    } else if(wellCmd->parsed()) {
      toExport = exporter::xlsx::ExportSettings::ExportView::WELL;
    } else if(listCmd->parsed()) {
      toExport = exporter::xlsx::ExportSettings::ExportView::IMAGE;
    }
    exportData(std::filesystem::path(infile), std::filesystem::path(outfile), toFormatEnum(format), toStyleEnum(style), toExport, wellId, tStack,
               imageName, outputTemplate);
  } else if(databaseCmd->parsed()) {
    if(dbView->parsed()) {
      viewData(infile, target);
    }
  }

  return 0;
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Cli::startAnalyze(const std::filesystem::path &pathToSettingsFile, const std::optional<std::string> &imagedInputFolder, std::string jobName)
{
  joda::settings::AnalyzeSettings analyzeSettings;

  // ==========================
  // Open settings file
  // ==========================
  try {
    analyzeSettings = joda::settings::Settings::openSettings(pathToSettingsFile);
  } catch(const std::exception &ex) {
    joda::log::logError("Could not load settings file >" + std::string(ex.what()) + "<!");
    std::exit(1);
  }

  // ==========================
  // Prepare and check settings
  // ==========================
  auto foundErrors = analyzeSettings.checkForErrors();
  bool hasError    = false;
  for(const auto &[pipeline, messages] : foundErrors) {
    for(const auto &msg : messages) {
      if(msg.severity == SettingParserLog::Severity::JODA_ERROR) {
        hasError = true;
      }
      msg.print();
    }
  }
  if(hasError) {
    joda::log::logError("Configuration has errors!");
    std::exit(0);
  }

  if(imagedInputFolder.has_value()) {
    analyzeSettings.projectSettings.workingDirectory  = imagedInputFolder.value();
    analyzeSettings.projectSettings.plate.imageFolder = imagedInputFolder.value();
  }

  // ==========================
  // Start job
  // ==========================
  if(jobName.empty()) {
    jobName = joda::helper::RandomNameGenerator::GetRandomName();
  }
  mController->start(analyzeSettings, {}, jobName);
  joda::log::logInfo("Job >" + jobName + "< started!");

  // ==========================
  // Running
  // ==========================
  float totalTiles    = 0;
  float finishedTiles = 0;
  try {
    while(true) {
      std::this_thread::sleep_for(2.5s);

      const auto &jobState = mController->getState();
      if(jobState.isFinished()) {
        break;
      }

      finishedTiles = static_cast<float>(jobState.finishedTiles());
      totalTiles    = static_cast<float>(jobState.totalTiles());
      if(totalTiles > 0) {
        joda::log::logProgress(finishedTiles / totalTiles, "Analyze running");
      } else {
        joda::log::logProgress(0, "Progress");
      }
    }
  } catch(...) {
  }
  joda::log::logProgress(1, "Completed");
  joda::log::logInfo("Job >" + jobName + "< finished!");
  std::exit(0);
}

///
/// \brief
/// \author
/// \param[in]
/// \param[out]
/// \return
///
void Cli::exportData(const std::filesystem::path &pathToDatabasefile, std::filesystem::path outputPath,
                     exporter::xlsx::ExportSettings::ExportSettings::ExportFormat format, exporter::xlsx::ExportSettings::ExportStyle style,
                     const exporter::xlsx::ExportSettings::ExportView &view, const std::string &wellId, const std::string &tStackIn,
                     const std::string &imageFileName, const std::string &classExportTemplate)
{
  int32_t tStack             = 0;
  int32_t groupId            = 0;
  std::string fileNameSuffix = "_plate";
  if(view == exporter::xlsx::ExportSettings::ExportView::WELL) {
    fileNameSuffix = "_well_" + wellId;
    try {
      groupId = std::stoi(wellId);
    } catch(...) {
      joda::log::logError("Well ID must be a number!");
      std::exit(1);
    }
  } else if(view == exporter::xlsx::ExportSettings::ExportView::IMAGE) {
    fileNameSuffix = "_" + imageFileName;
    helper::stringReplace(fileNameSuffix, ".", "");
    try {
      tStack = std::stoi(tStackIn);
    } catch(...) {
      joda::log::logError("Time stack must be a number!");
      std::exit(1);
    }
  }

  if(outputPath.empty()) {
    std::string fileName = pathToDatabasefile.filename().string();
    helper::stringReplace(fileName, ".icdb", "");

    if(format == exporter::xlsx::ExportSettings::ExportSettings::ExportFormat::XLSX) {
      fileName += fileNameSuffix + ".xlsx";
    }
    if(format == exporter::xlsx::ExportSettings::ExportSettings::ExportFormat::R) {
      fileName += fileNameSuffix + ".R";
    }

    outputPath = pathToDatabasefile.parent_path() / fileName;
  }

  std::optional<std::list<settings::Class>> classes = std::nullopt;
  if(!classExportTemplate.empty()) {
    try {
      joda::settings::Classification settings = joda::templates::TemplateParser::loadTemplate(std::filesystem::path(classExportTemplate));
      classes.emplace(settings.classes);
    } catch(const std::exception &ex) {
      joda::log::logError("Could not load template >" + classExportTemplate + "<. What: " + std::string(ex.what()));
      exit(1);
    }
  }

  try {
    const int32_t plateId = 0;
    mController->exportData(pathToDatabasefile, joda::exporter::xlsx::ExportSettings{style, format, view, {plateId, groupId, tStack, imageFileName}},
                            outputPath, classes);
  } catch(const std::exception &ex) {
    joda::log::logError(ex.what());
    std::exit(1);
  }
  std::exit(0);
}

///
/// \brief      Init logger
///             allowed inputs are: [off, error, warning, info, debug, trace]
/// \author     Joachim Danmayr
///
void Cli::setLogLevel(const std::string &logLevel)
{
  if(logLevel == "off") {
    joda::log::setLogLevel(joda::log::LogLevel::OFF);
  } else if(logLevel == "error") {
    joda::log::setLogLevel(joda::log::LogLevel::ERROR_);

  } else if(logLevel == "warning") {
    joda::log::setLogLevel(joda::log::LogLevel::WARNING);

  } else if(logLevel == "info") {
    joda::log::setLogLevel(joda::log::LogLevel::INFO);

  } else if(logLevel == "debug") {
    joda::log::setLogLevel(joda::log::LogLevel::DEBUG);

  } else if(logLevel == "trace") {
    joda::log::setLogLevel(joda::log::LogLevel::TRACE);
  } else {
    std::cout << "Wrong parameter for loglevel!" << std::endl;
    std::exit(1);
  }
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
void Cli::viewData(const std::filesystem::path &pathToDatabasefile, const std::string &target)
{
  auto analyzer = std::make_unique<joda::db::Database>();
  analyzer->openDatabase(std::filesystem::path(pathToDatabasefile.string()));

  if(target == "wells") {
    auto groups = analyzer->selectGroups();
    // Header
    std::cout << std::left << std::setw(10) << "ID" << std::setw(20) << "Name"
              << "\n";
    std::cout << std::string(30, '-') << "\n";
    for(const auto &[key, value] : groups) {
      std::cout << std::left << std::setw(10) << key << std::setw(20) << value << "\n";
    }
  }

  if(target == "images") {
    auto images = analyzer->selectImages();
    // Header
    std::cout << std::left << std::setw(30) << "ID" << std::setw(40) << "Name"
              << "\n";
    std::cout << std::string(50, '-') << "\n";
    for(const auto &image : images) {
      std::cout << std::left << std::setw(30) << std::to_string(image.imageId) << std::setw(40) << image.filename << "\n";
    }
  }
}

auto toFormatEnum(const std::string &type) -> exporter::xlsx::ExportSettings::ExportSettings::ExportFormat
{
  exporter::xlsx::ExportSettings::ExportSettings::ExportFormat typeEnum;
  if(type == "xlsx") {
    typeEnum = exporter::xlsx::ExportSettings::ExportSettings::ExportFormat::XLSX;
  } else if(type == "r") {
    typeEnum = exporter::xlsx::ExportSettings::ExportFormat::R;
  } else {
    joda::log::logError("Invalid export type!");
    std::exit(1);
  }
  return typeEnum;
}

auto toStyleEnum(const std::string &format) -> exporter::xlsx::ExportSettings::ExportStyle
{
  exporter::xlsx::ExportSettings::ExportStyle formatEnum;
  if(format == "table") {
    formatEnum = exporter::xlsx::ExportSettings::ExportStyle::LIST;
  } else if(format == "heatmap") {
    formatEnum = exporter::xlsx::ExportSettings::ExportStyle::HEATMAP;
  } else {
    joda::log::logError("Invalid export format!");
    std::exit(1);
  }
  return formatEnum;
}

}    // namespace joda::ui::cli
