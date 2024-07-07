#pragma once

#include <QString>

namespace joda::helper {

static QString getLoggedInUserName()
{
#ifdef Q_OS_WIN
  // On Windows, we can use the USERNAME environment variable
  return QString::fromLocal8Bit(qgetenv("USERNAME"));
#elif defined(Q_OS_UNIX)
  // On Unix-like systems (Linux/macOS), we can use the USER environment variable
  return QString::fromLocal8Bit(qgetenv("USER"));
#else
  // Fallback to using a system command
  QProcess process;
  process.start("whoami");
  process.waitForFinished();
  return process.readAllStandardOutput().trimmed();
#endif
}
}    // namespace joda::helper
