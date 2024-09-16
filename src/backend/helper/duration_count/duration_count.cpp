

#include "duration_count.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

uint32_t DurationCount::start(std::string comment)
{
  totalCnt++;
  srand((unsigned) time(0) + totalCnt);
  uint32_t randNr           = (rand() % INT32_MAX) + 1;
  DurationCount::TimeDely d = TimeDely{.t_start = std::chrono::high_resolution_clock::now(), .mComment = comment};
  std::lock_guard<std::mutex> lock(mLock);
  mDelays[randNr] = d;
  return randNr;
}

void DurationCount::stop(uint32_t rand)
{
  auto t_end = std::chrono::high_resolution_clock::now();
  std::lock_guard<std::mutex> lock(mLock);
  auto durations         = t_end - mDelays[rand].t_start;
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();
  joda::log::logTrace(mDelays[rand].mComment + ": " + std::to_string(elapsed_time_ms) + " ms.");
  mStats[mDelays[rand].mComment].cnt++;
  mStats[mDelays[rand].mComment].timeCount += durations;
  mDelays.erase(rand);
}

void DurationCount::resetStats()
{
  std::lock_guard<std::mutex> lock(mLock);
  mStats.clear();
  mStartTime = std::chrono::high_resolution_clock::now();
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
  auto timeEnd = std::chrono::high_resolution_clock::now();

  auto durations         = timeEnd - mStartTime;
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();

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
  std::string filename = (outputDir / "profiling.json").string();

  // Open the file for writing
  std::ofstream outputFile(filename);

  if(outputFile.is_open()) {
    outputFile << statsJson.dump(2);

    // Close the file
    outputFile.close();

    std::cout << "String successfully written to file '" << filename << "'." << std::endl;
  } else {
    std::cerr << "Error: Unable to open file '" << filename << "' for writing." << std::endl;
  }
}
