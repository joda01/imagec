///
/// \file      console_logger.cpp
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

#include "console_logger.hpp"
#include <atomic>
#include <chrono>
#include <deque>
#include <mutex>
#include <regex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace joda::log {

// Color codes for console formatting
const std::string RESET_COLOR  = "\033[0m";
const std::string RED_COLOR    = "\033[31m";
const std::string GREEN_COLOR  = "\033[32m";
const std::string GRAY_COLOR   = "\033[37m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR   = "\033[34m";

LogLevel mLogLevel = LogLevel::TRACE;
std::mutex mWriteMutex;
float lastProgress = 0;
std::vector<std::string> logBuffer;

void enqueue(std::string_view msg, std::string_view tag, std::string_view color);

void setLogLevel(LogLevel logLevel)
{
  mLogLevel = logLevel;
}

auto getLogBuffer() -> const std::vector<std::string> &
{
  return logBuffer;
}

std::string escapeHtml(const std::string &text)
{
  std::string escaped;
  for(char c : text) {
    switch(c) {
      case '\n':
        escaped += "<br/>";
        break;
      case '&':
        escaped += "&amp;";
        break;
      case '<':
        escaped += "&lt;";
        break;
      case '>':
        escaped += "&gt;";
        break;
      default:
        escaped += c;
        break;
    }
  }
  return escaped;
}

std::string ansiToHtml(const std::string &input)
{
  std::unordered_map<int, std::string> ansiColorMap = {
      {30, "#000000"},    // black
      {31, "#ff0000"},    // red
      {32, "#00ff00"},    // green
      {33, "#ffff00"},    // yellow
      {34, "#0000ff"},    // blue
      {35, "#ff00ff"},    // magenta
      {36, "#00ffff"},    // cyan
      {37, "#a9a9a9"},    // gray
                          // Add more as needed
  };

  std::string output;
  std::regex ansiRegex("\033\\[(\\d+)m");
  std::smatch match;
  std::string::const_iterator searchStart(input.cbegin());
  bool spanOpen = false;

  while(std::regex_search(searchStart, input.cend(), match, ansiRegex)) {
    std::string textBefore = std::string(searchStart, match[0].first);
    output += escapeHtml(textBefore);    // escape HTML in plain text
    int code = std::stoi(match[1]);

    if(code == 0) {
      if(spanOpen) {
        output += "</span>";
        spanOpen = false;
      }
    } else if(ansiColorMap.count(code)) {
      if(spanOpen) {
        output += "</span>";
      }
      output += "<span style=\"color:" + ansiColorMap[code] + ";\">";
      spanOpen = true;
    }

    searchStart = match.suffix().first;
  }

  output += std::string(searchStart, input.cend());

  if(spanOpen) {
    output += "</span>";
  }

  return output;
}

auto logBufferToHtml() -> std::string
{
  auto joinWithNewlines = [](const std::vector<std::string> &lines) -> std::string {
    std::ostringstream oss;
    for(size_t i = 0; i < lines.size(); ++i) {
      oss << lines[i];
      if(i != lines.size() - 1) {
        oss << "\n";
      }
    }
    return oss.str();
  };

  std::string result = joinWithNewlines(logBuffer);
  return ansiToHtml(result);
}

std::string toPercentString(float ratio)
{
  auto percent = static_cast<int>(ratio * 100.0F);
  std::string percentString;
  if(percent < 10) {
    percentString = " " + std::to_string(percent) + "%";
  } else if(percent < 100) {
    percentString = std::to_string(percent) + "%";
  } else {
    percentString = "FIN";
  }
  return percentString;
}

void logError(const std::string &message)
{
  if(mLogLevel < LogLevel::ERROR_) {
    return;
  }
  enqueue(message, "ERR", RED_COLOR);
}

void logWarning(const std::string &message)
{
  if(mLogLevel < LogLevel::WARNING) {
    return;
  }
  enqueue(message, "WRN", YELLOW_COLOR);
}

void logInfo(const std::string &message)
{
  if(mLogLevel < LogLevel::INFO) {
    return;
  }
  enqueue(message, "INF", BLUE_COLOR);
}

void logDebug(const std::string &message)
{
  if(mLogLevel < LogLevel::DEBUG) {
    return;
  }
  enqueue(message, "DBG", GREEN_COLOR);
}

void logTrace(const std::string &message)
{
  if(mLogLevel < LogLevel::TRACE) {
    return;
  }
  enqueue(message, "TRC", GRAY_COLOR);
}

void logProgress(float ratio, const std::string &message)
{
  lastProgress = ratio;
  enqueue(message, "[" + toPercentString(ratio) + "]", GRAY_COLOR);
}

/////////////////////////////////////////////////////

static constexpr size_t QUEUE_SIZE = 4096;    // power of 2 -> faster mask

struct LogItem
{
  std::string text;    // final formatted string
  std::atomic<bool> ready{false};
};

std::array<LogItem, QUEUE_SIZE> buffer;
std::atomic<size_t> head{0};
std::atomic<size_t> tail{0};

std::atomic<bool> running{false};
std::thread worker;

inline std::string getCurrentDateTimeISO()
{
  auto now                = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm localTime       = *std::localtime(&currentTime);
  char bufferLocal[20];
  (void) std::strftime(bufferLocal, sizeof(bufferLocal), "%Y-%m-%dT%H:%M:%S", &localTime);
  return bufferLocal;
}

// --- FAST timestamp + string builder ---
static std::string buildLine(std::string_view color, std::string_view tag, std::string_view msg)
{
  auto ts = getCurrentDateTimeISO();

  std::string out;
  out.reserve(32 + ts.size() + msg.size());

  out.append(color);
  out.append("[");
  out.append(tag);
  out.append("]");
  out.append(RESET_COLOR);
  out.append("[");
  out.append(ts);
  out.append("] ");
  out.append(msg);

  return out;
}

// --- Zero-latency enqueue ---
void enqueue(std::string_view msg, std::string_view tag, std::string_view color)
{
  size_t h    = head.load(std::memory_order_relaxed);
  size_t next = (h + 1) & (QUEUE_SIZE - 1);
  size_t t    = tail.load(std::memory_order_acquire);

  if(next == t) {
    // queue full → drop the message (zero-latency guarantee)
    return;
  }

  LogItem &item = buffer[h];
  item.text     = buildLine(color, tag, msg);    // local allocation only
  item.ready.store(true, std::memory_order_release);
  head.store(next, std::memory_order_release);
}

// --- Worker thread prints everything ---
void workerLoop()
{
  while(running.load(std::memory_order_relaxed)) {
    size_t t = tail.load(std::memory_order_relaxed);
    size_t h = head.load(std::memory_order_acquire);

    if(t == h) {
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      continue;
    }

    LogItem &item = buffer[t];
    if(item.ready.load(std::memory_order_acquire)) {
      std::cout << item.text << '\n';
      logBuffer.emplace_back(item.text);
      item.ready.store(false, std::memory_order_relaxed);
      tail.store((t + 1) & (QUEUE_SIZE - 1), std::memory_order_release);
    }
  }

  // Flush remaining messages on shutdown
  size_t t = tail.load();
  size_t h = head.load();
  while(t != h) {
    LogItem &item = buffer[t];
    if(item.ready.load()) {
      std::cout << item.text << '\n';
      logBuffer.emplace_back(item.text);
    }
    t = (t + 1) & (QUEUE_SIZE - 1);
  }
}

void initLogger()
{
  running.store(true);
  worker = std::thread([] { workerLoop(); });
}

void joinLogger()
{
  running.store(false);
  worker.join();
}

}    // namespace joda::log
