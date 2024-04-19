

#include "duration_count.h"
#include <string>

uint32_t DurationCount::start(std::string comment)
{
  totalCnt++;
  srand((unsigned) time(0) + totalCnt);
  uint32_t randNr = (rand() % INT32_MAX) + 1;
  std::lock_guard<std::mutex> lock(mLock);
  mDelays[randNr] = {.t_start = std::chrono::high_resolution_clock::now(), .mComment = comment};
  return randNr;
}

void DurationCount::stop(uint32_t rand)
{
  auto t_end = std::chrono::high_resolution_clock::now();
  std::lock_guard<std::mutex> lock(mLock);
  auto durations = t_end - mDelays[rand].t_start;
  // double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();
  //  joda::log::logTrace(mDelays[rand].mComment + ": " + std::to_string(elapsed_time_ms) + " ms.");
  mStats[mDelays[rand].mComment].cnt++;
  mStats[mDelays[rand].mComment].timeCount += durations;
  mDelays.erase(rand);
}

void DurationCount::resetStats()
{
  mStats.clear();
  mStartTime = std::chrono::high_resolution_clock::now();
}

void DurationCount::printStats(double nrOfImages)
{
  std::lock_guard<std::mutex> lock(mLock);
  auto timeEnd = std::chrono::high_resolution_clock::now();

  auto durations         = timeEnd - mStartTime;
  double elapsed_time_ms = std::chrono::duration<double, std::milli>(durations).count();

  for(const auto &[comment, stats] : mStats) {
    double totalMs = std::chrono::duration<double, std::milli>(stats.timeCount).count();
    std::cout << comment << "\t" << std::to_string(stats.cnt) << "\t" << std::to_string(totalMs) << "\t"
              << std::to_string(totalMs / stats.cnt) << std::endl;
  }

  double totalMs = std::chrono::duration<double, std::milli>(durations).count();
  std::cout << "Duration"
            << "\t" << std::to_string(nrOfImages) << "\t" << std::to_string(totalMs) << "\t"
            << std::to_string(totalMs / nrOfImages) << std::endl;
}
