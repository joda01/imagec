#pragma once

#include <exception>
#include <string>

namespace joda::exceptions {

class RuntimeException : public std::exception
{
public:
  RuntimeException(const std::string &what) : mWhat(what)
  {
  }

  char *what()
  {
    return mWhat.data();
  }

private:
  std::string mWhat;
}

}    // namespace joda::exceptions
