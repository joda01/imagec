///
/// \file      debugging.hpp
/// \author    Joachim Danmayr
/// \date      2025-11-28
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#ifdef _WIN32

#define CHECK_GUI_THREAD(obj)

#elif defined(__APPLE__)

#include <qthread.h>

#define CHECK_GUI_THREAD(obj)                                                    \
  if((obj)->thread() != QThread::currentThread()) {                              \
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;                       \
    qWarning() << "GUI accessed from wrong thread:" << QThread::currentThread(); \
  }

#else

#include <qthread.h>

#define CHECK_GUI_THREAD(obj)                                                    \
  if((obj)->thread() != QThread::currentThread()) {                              \
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;                       \
    qWarning() << "GUI accessed from wrong thread:" << QThread::currentThread(); \
  }
#endif
