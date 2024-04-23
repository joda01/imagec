

#include "duration_count.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

uint32_t DurationCount::start(std::string comment)
{
  totalCnt++;
  srand((unsigned) time(0) + totalCnt);
  uint32_t randNr = (rand() % INT32_MAX) + 1;
  auto d          = TimeDely{.t_start = std::chrono::high_resolution_clock::now(), .mComment = comment};
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
  // joda::log::logTrace(mDelays[rand].mComment + ": " + std::to_string(elapsed_time_ms) + " ms.");
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

inline std::string getCurrentDateTime()
{
  time_t now = time(0);
  struct tm timeinfo;
  char buffer[80];
  localtime_r(&now, &timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &timeinfo);
  return std::string(buffer);
}
void DurationCount::printStats(double nrOfImages)
{
  std::lock_guard<std::mutex> lock(mLock);
  auto timeEnd = std::chrono::high_resolution_clock::now();

  auto durations         = timeEnd - mStartTime;
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();

  std::string statsStr = "";
  for(const auto &[comment, stats] : mStats) {
    double totalMs = std::chrono::duration<double, std::milli>(stats.timeCount).count();
    statsStr       = statsStr + comment + "\t" + std::to_string(stats.cnt) + "\t" + std::to_string(totalMs) + "\t" +
               std::to_string(totalMs / stats.cnt) + "\n";
  }

  double totalMs = std::chrono::duration<double, std::milli>(durations).count();
  statsStr += "Duration\t" + std::to_string(nrOfImages) + "\t" + std::to_string(totalMs) + "\t" +
              std::to_string(totalMs / nrOfImages) + "\n";

  // Get the current date
  std::string currentDate = getCurrentDateTime();

  // Define the filename using the current date
  std::string filename = currentDate + "-profiling.txt";

  // Open the file for writing
  std::ofstream outputFile(filename);

  if(outputFile.is_open()) {
    outputFile << statsStr;

    // Close the file
    outputFile.close();

    std::cout << "String successfully written to file '" << filename << "'." << std::endl;
  } else {
    std::cerr << "Error: Unable to open file '" << filename << "' for writing." << std::endl;
  }
}
