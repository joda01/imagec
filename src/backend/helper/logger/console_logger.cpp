#include "console_logger.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace joda::log {

// --- Constants & State ---
const std::string RESET_COLOR  = "\033[0m";
const std::string RED_COLOR    = "\033[31m";
const std::string GREEN_COLOR  = "\033[32m";
const std::string GRAY_COLOR   = "\033[37m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR   = "\033[34m";

static std::atomic<LogLevel> mLogLevel{LogLevel::TRACE};
static std::mutex mBufferMutex;    // Protects logBuffer
static std::vector<std::string> logBuffer;
static std::atomic<float> lastProgress{0.0f};

// --- Ring Buffer Structures ---
static constexpr size_t QUEUE_SIZE = 4096;
struct LogItem
{
  std::string text;
  std::atomic<bool> ready{false};
};

static std::array<LogItem, QUEUE_SIZE> buffer;
static std::atomic<size_t> head{0};
static std::atomic<size_t> tail{0};
static std::atomic<bool> running{false};
static std::thread worker;

// --- Internal Helpers ---

// Thread-safe ISO Timestamp
inline std::string getCurrentDateTimeISO()
{
  auto now                = std::chrono::system_clock::now();
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
  std::tm localTime;
#ifdef _WIN32
  localtime_s(&localTime, &currentTime);
#else
  localtime_r(&currentTime, &localTime);
#endif
  char bufferLocal[20];
  std::strftime(bufferLocal, sizeof(bufferLocal), "%Y-%m-%dT%H:%M:%S", &localTime);
  return bufferLocal;
}

static std::string buildLine(std::string_view color, std::string_view tag, std::string_view msg)
{
  auto ts = getCurrentDateTimeISO();
  std::string out;
  out.reserve(64 + msg.size());    // Pre-allocate memory
  out += color;
  out += "[";
  out += tag;
  out += "]";
  out += RESET_COLOR;
  out += "[";
  out += ts;
  out += "] ";
  out += msg;
  return out;
}

// --- API Implementation ---

void setLogLevel(LogLevel logLevel)
{
  mLogLevel.store(logLevel);
}

void enqueue(std::string_view msg, std::string_view tag, std::string_view color)
{
  size_t current_head = head.load(std::memory_order_relaxed);
  size_t next_head;

  // Multi-producer "claim" a slot
  do {
    next_head = (current_head + 1) & (QUEUE_SIZE - 1);
    if(next_head == tail.load(std::memory_order_acquire)) {
      return;    // Buffer full: drop message
    }
  } while(!head.compare_exchange_weak(current_head, next_head, std::memory_order_release));

  // 'current_head' is now uniquely ours
  LogItem &item = buffer[current_head];
  item.text     = buildLine(color, tag, msg);
  item.ready.store(true, std::memory_order_release);
}

// --- Worker Loop ---

void workerLoop()
{
  while(running.load(std::memory_order_relaxed)) {
    size_t t = tail.load(std::memory_order_relaxed);
    if(t == head.load(std::memory_order_acquire)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      continue;
    }

    LogItem &item = buffer[t];
    if(item.ready.load(std::memory_order_acquire)) {
      // Write to console and shared history
      std::cout << item.text << "\n";
      {
        std::lock_guard<std::mutex> lock(mBufferMutex);
        logBuffer.emplace_back(item.text);
      }

      item.ready.store(false, std::memory_order_relaxed);
      tail.store((t + 1) & (QUEUE_SIZE - 1), std::memory_order_release);
    }
  }
}

// --- Public Interface ---

void initLogger()
{
  if(!running.exchange(true)) {
    worker = std::thread(workerLoop);
  }
}

void joinLogger()
{
  if(running.exchange(false)) {
    if(worker.joinable())
      worker.join();
    std::cout.flush();
  }
}

// Ensure thread-safe access to the log vector
auto getLogBuffer() -> std::vector<std::string>
{
  std::lock_guard<std::mutex> lock(mBufferMutex);
  return logBuffer;
}

// Log wrappers
void logError(const std::string &m)
{
  if(mLogLevel >= LogLevel::ERROR_)
    enqueue(m, "ERR", RED_COLOR);
}
void logWarning(const std::string &m)
{
  if(mLogLevel >= LogLevel::WARNING)
    enqueue(m, "WRN", YELLOW_COLOR);
}
void logInfo(const std::string &m)
{
  if(mLogLevel >= LogLevel::INFO)
    enqueue(m, "INF", BLUE_COLOR);
}
void logDebug(const std::string &m)
{
  if(mLogLevel >= LogLevel::DEBUG)
    enqueue(m, "DBG", GREEN_COLOR);
}
void logTrace(const std::string &m)
{
  if(mLogLevel >= LogLevel::TRACE)
    enqueue(m, "TRC", GRAY_COLOR);
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

void logProgress(float ratio, const std::string &message)
{
  lastProgress = ratio;
  enqueue(message, toPercentString(ratio), GRAY_COLOR);
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

}    // namespace joda::log
