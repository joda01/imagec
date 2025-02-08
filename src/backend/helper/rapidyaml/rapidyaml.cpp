///
/// \file      rapidyaml.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \cite      https://github.com/biojppm/rapidyaml/blob/master/samples/quickstart.cpp#L75
///

#include <ryml.hpp>

#include <iostream>
#include <stdexcept>
#include "backend/helper/helper.hpp"
#include <c4/format.hpp>    // needed for the examples below
#include <c4/yml/event_handler_tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/parse_engine.def.hpp>
#include <c4/yml/std/std.hpp>
#include <c4/yml/tree.hpp>
#include <nlohmann/json.hpp>
#include "rapidyaml.hpp"
#include <ryml_std.hpp>

/** this is an example error handler, required for some of the
 * quickstart examples. */
struct ErrorHandlerExample
{
  ryml::Callbacks callbacks();
  C4_NORETURN void on_error(const char *msg, size_t len, ryml::Location loc);
  C4_NORETURN static void s_error(const char *msg, size_t len, ryml::Location loc, void *this_);
  template <class Fn>
  C4_NODISCARD bool check_error_occurs(Fn &&fn) const;
  template <class Fn>
  C4_NODISCARD bool check_assertion_occurs(Fn &&fn) const;
  void check_effect(bool committed) const;
  ErrorHandlerExample() : defaults(ryml::get_callbacks())
  {
  }
  ryml::Callbacks defaults;
};

// checking
template <class Fn>
C4_NODISCARD bool ErrorHandlerExample::check_error_occurs(Fn &&fn) const
{
  bool expected_error_occurred = false;
  C4_IF_EXCEPTIONS_(try, if(setjmp(s_jmp_env) == 0))    // selectively picks based on availability of exceptions
  {
    fn();
  }
  C4_IF_EXCEPTIONS_(catch(...), else)
  {
    expected_error_occurred = true;
  }
  return expected_error_occurred;
}
template <class Fn>
C4_NODISCARD bool ErrorHandlerExample::check_assertion_occurs(Fn &&fn) const
{
#if RYML_USE_ASSERT
  return check_error_occurs(fn);
#else
  (void) fn;    // do nothing otherwise, as there would be undefined behavior
  return true;
#endif
}

/** this C-style callback is the one stored and used by ryml. It is a
 * trampoline function calling on_error() */
C4_NORETURN void ErrorHandlerExample::s_error(const char *msg, size_t len, ryml::Location loc, void *this_)
{
  (static_cast<ErrorHandlerExample *>(this_))->on_error(msg, len, loc);
}
/** this is the where the callback implementation goes. Remember that it must not return. */
C4_NORETURN void ErrorHandlerExample::on_error(const char *msg, size_t len, ryml::Location loc)
{
  std::string full_msg = ryml::formatrs<std::string>("{}:{}:{} ({}B): ERROR: {}", loc.name, loc.line, loc.col, loc.offset, ryml::csubstr(msg, len));
  std::cout << full_msg << std::endl;
  C4_IF_EXCEPTIONS(
      // this will execute if exceptions are enabled.
      throw std::runtime_error(full_msg);,
                                         // this will execute if exceptions are disabled. It will
                                         // jump to the function calling the corresponding setjmp().
                                         s_jmp_msg = full_msg;
      std::longjmp(s_jmp_env, 1););
}

/** a helper to create the Callbacks object with the custom error handler */
ryml::Callbacks ErrorHandlerExample::callbacks()
{
  return ryml::Callbacks(this, nullptr, nullptr, ErrorHandlerExample::s_error);
}

void ErrorHandlerExample::check_effect(bool committed) const
{
  ryml::Callbacks const &current = ryml::get_callbacks();
  if(committed) {
  } else {
  }
}

namespace joda::yaml {

nlohmann::json Yaml::convert(std::string yamlString)
{
  ErrorHandlerExample errh;
  nlohmann::json out;
  ryml::set_callbacks(errh.callbacks());
  errh.check_effect(/*committed*/ true);
  if(errh.check_error_occurs([&] {
       joda::helper::stringReplace(yamlString, "- inf", "- 'inf'");
       ryml::Tree tree        = ryml::parse_in_place(yamlString.data());
       std::string jsonString = ryml::emitrs_json<std::string>(tree);
       try {
         out = nlohmann::json::parse(jsonString);
       } catch(...) {
         std::cout << jsonString << std::endl;
       }
     })) {
    // throw std::runtime_error("Error parsing error!");
  } else {
  }
  ryml::set_callbacks(errh.defaults);    // restore defaults.
  errh.check_effect(/*committed*/ false);
  return out;
}
}    // namespace joda::yaml
