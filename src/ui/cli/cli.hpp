///
/// \file      terminal.hpp
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

#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include "backend/database/exporter/xlsx/exporter_xlsx.hpp"

namespace joda::ctrl {
class Controller;
};

namespace joda::ui::cli {

class Cli
{
public:
  /////////////////////////////////////////////////////
  Cli();
  int startCommandLineController(int argc, char *argv[]);
  void startAnalyze(const std::filesystem::path &pathToSettingsFile, const std::optional<std::string> &imagedInputFolder, std::string jobName);

  void exportData(const std::filesystem::path &pathToDatabasefile, std::filesystem::path outputPath,
                  exporter::xlsx::ExportSettings::ExportSettings::ExportFormat type, exporter::xlsx::ExportSettings::ExportStyle formatEnum,
                  const exporter::xlsx::ExportSettings::ExportView &view, const std::string &wellId, const std::string &tStack,
                  const std::string &imageName);
  static void setLogLevel(const std::string &logLevel);

private:
  /////////////////////////////////////////////////////
  std::unique_ptr<ctrl::Controller> mController;
};

}    // namespace joda::ui::cli
