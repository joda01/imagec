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
#include <optional>

namespace joda::ctrl {
class Controller;
};

namespace joda::ui::terminal {

class Terminal
{
public:
  /////////////////////////////////////////////////////
  Terminal(ctrl::Controller *);
  void startAnalyze(const std::filesystem::path &pathToSettingsFile, std::optional<std::string> &imagedInputFolder);
  void exportData(const std::filesystem::path &pathToDatabasefile, const std::filesystem::path &outputPath,
                  const std::filesystem::path &pathToQueryFilter, const std::string &type, const std::string &format, const std::string &view,
                  const std::string &exportFilter);

private:
  /////////////////////////////////////////////////////
  ctrl::Controller *mController;
};

}    // namespace joda::ui::terminal
