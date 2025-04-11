///
/// \file      user_settings.hpp
/// \author    Joachim Danmayr
/// \date      2025-04-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include <vector>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include "user_home_dir.hpp"

namespace joda::user_settings {

struct UserSettings
{
  void save() const
  {
    auto userSettingsPath = getUserHomeDir() / "userSettings.json";
  }
  void open()
  {
    auto userSettingsPath = getUserHomeDir() / "userSettings.json";
  }

  void addLastOpenedProject(const std::string &path)
  {
    addToVector(lastOpenedProjects, path);
  }

  void addLastOpenedResult(const std::string &path)
  {
    addToVector(lastOpenedResults, path);
  }

  auto addLastOpenedProject() const -> const std::vector<std::string> &
  {
    return lastOpenedProjects;
  }

  auto addLastOpenedResult() const -> const std::vector<std::string> &
  {
    return lastOpenedResults;
  }

private:
  static void addToVector(std::vector<std::string> &vec, const std::string &path)
  {
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const std::string &s) { return s == path; }), vec.end());
    vec.insert(vec.begin(), path);
    if(vec.size() > 6) {
      vec.resize(6);
    }
  }

  /////////////////////////////////////////////////////
  std::vector<std::string> lastOpenedProjects;
  std::vector<std::string> lastOpenedResults;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UserSettings, lastOpenedProjects, lastOpenedResults);
};

}    // namespace joda::user_settings
