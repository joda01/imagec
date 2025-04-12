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

#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <vector>
#include "backend/enums/enums_file_endians.hpp"
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "user_home_dir.hpp"

namespace joda::user_settings {

struct UserSettings
{
public:
  struct Entry
  {
    std::string path;
    std::string title;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Entry, path, title);
  };

  static void save()
  {
    std::filesystem::path userSettingsPath = getUserHomeDir() / "userSettings.json";
    std::ofstream out(userSettingsPath);
    if(!out.is_open()) {
      throw std::runtime_error("Cannot open file >" + userSettingsPath.string() + "< for writing! Do you have write permissions?");
    }
    UserSettings set;
    nlohmann::json json = set;
    out << json.dump(2);
    if(out.bad()) {
      throw std::runtime_error("Cannot write data! Do you have write permissions and enough space left on your disk?");
    }
    out.close();
  }
  static void open()
  {
    std::filesystem::path userSettingsPath = getUserHomeDir() / "userSettings.json";
    std::ifstream ifs(userSettingsPath);
    std::string wholeFile = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();
    UserSettings set;
    set = nlohmann::json::parse(wholeFile);
  }

  static void addLastOpenedProject(const std::string &path)
  {
    addToVector(lastOpenedProjects, {path, ""});
    save();
  }

  static void addLastOpenedResult(const std::string &path, const std::string &title)
  {
    addToVector(lastOpenedResults, {path, title});
    save();
  }

  [[nodiscard]] static auto getLastOpenedProject() -> const std::vector<Entry> &
  {
    return lastOpenedProjects;
  }

  [[nodiscard]] static auto getLastOpenedResult() -> const std::vector<Entry> &
  {
    return lastOpenedResults;
  }

private:
  static void addToVector(std::vector<Entry> &vec, const Entry &entry)
  {
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const Entry &s) { return s.path == entry.path; }), vec.end());
    vec.insert(vec.begin(), entry);
    if(vec.size() > 6) {
      vec.resize(6);
    }
  }

  /////////////////////////////////////////////////////
  static inline std::vector<Entry> lastOpenedProjects;
  static inline std::vector<Entry> lastOpenedResults;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UserSettings, lastOpenedProjects, lastOpenedResults);
};

}    // namespace joda::user_settings
