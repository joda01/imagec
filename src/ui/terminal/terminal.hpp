///
/// \file      terminal.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-10
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
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
  void exportData(const std::filesystem::path &pathToDatabasefile, const std::filesystem::path &outputPath);

private:
  /////////////////////////////////////////////////////
  ctrl::Controller *mController;
};

}    // namespace joda::ui::terminal
