

#include "duration_count.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

void DurationCount::resetStats()
{
  std::lock_guard<std::mutex> lock(mLock);
  mStats.clear();
  mStartTime = std::chrono::steady_clock::now();
}

std::string getCurrentDateTime()
{
  auto now      = std::chrono::system_clock::now();
  auto now_time = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d_%H-%M-%S");
  return ss.str();
}
void DurationCount::printStats(double nrOfImages, const std::filesystem::path &outputDir)
{
  std::lock_guard<std::mutex> lock(mLock);
  auto timeEnd = std::chrono::steady_clock::now();

  auto durations = timeEnd - mStartTime;
  // double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();

  nlohmann::json statsJson;
  for(const auto &[comment, stats] : mStats) {
    double totalMs                         = std::chrono::duration<double, std::milli>(stats.timeCount).count();
    statsJson["stats"][comment]["cnt"]     = stats.cnt;
    statsJson["stats"][comment]["totalMs"] = totalMs;
    statsJson["stats"][comment]["avgMs"]   = static_cast<float>(totalMs) / static_cast<float>(stats.cnt);
  }

  double totalMs = std::chrono::duration<double, std::milli>(durations).count();

  statsJson["duration"]["cnt"]     = nrOfImages;
  statsJson["duration"]["totalMs"] = totalMs;
  statsJson["duration"]["avgMs"]   = static_cast<float>(totalMs) / static_cast<float>(nrOfImages);

  // Get the current date
  std::string currentDate = getCurrentDateTime();

  // Define the filename using the current date
  std::string filename = (outputDir / "profiling.json").generic_string();

  // Open the file for writing
  std::ofstream outputFile(filename);

  if(outputFile.is_open()) {
    outputFile << statsJson.dump(2);

    // Close the file
    outputFile.close();
  } else {
    std::cerr << "Error: Unable to open file '" << filename << "' for writing." << std::endl;
  }
}
