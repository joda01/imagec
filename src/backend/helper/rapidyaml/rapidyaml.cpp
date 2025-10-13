///
/// \file      rapidyaml.hpp
/// \author    Joachim Danmayr
/// \date      2025-02-08
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///
///
/// \cite      https://github.com/biojppm/rapidyaml/blob/master/samples/quickstart.cpp#L75
///

#include <ryml.hpp>

#include <exception>
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
  C4_NODISCARD std::string check_error_occurs(Fn &&fn) const;
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
C4_NODISCARD std::string ErrorHandlerExample::check_error_occurs(Fn &&fn) const
{
  try {
    fn();
  } catch(const std::exception &ex) {
    return std::string(">" + std::string(ex.what()) + "<");
  }

  return "";
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
  auto full_msg = ryml::formatrs<std::string>("{}:{}:{} ({}B): ERROR: {}", loc.name, loc.line, loc.col, loc.offset, ryml::csubstr(msg, len));
  throw std::runtime_error(full_msg);
}

/** a helper to create the Callbacks object with the custom error handler */
ryml::Callbacks ErrorHandlerExample::callbacks()
{
  return ryml::Callbacks(this, nullptr, nullptr, ErrorHandlerExample::s_error);
}

void ErrorHandlerExample::check_effect(bool /*committed*/) const
{
  /*
  ryml::Callbacks const &current = ryml::get_callbacks();
  if(committed) {
  } else {
  }*/
}

namespace joda::yaml {

nlohmann::json Yaml::convert(std::string yamlString)
{
  ErrorHandlerExample errh;
  nlohmann::json out;
  ryml::set_callbacks(errh.callbacks());
  errh.check_effect(/*committed*/ true);
  auto ret = errh.check_error_occurs([&] {
    joda::helper::stringReplace(yamlString, "- inf", "- 'inf'");
    ryml::Tree tree = ryml::parse_in_place(yamlString.data());
    auto jsonString = ryml::emitrs_json<std::string>(tree);
    try {
      out = nlohmann::json::parse(jsonString);
    } catch(const std::string &ex) {
      throw std::runtime_error("Json parsing error >" + jsonString + "<");
    }
  });

  if(!ret.empty()) {
    throw std::runtime_error(ret);
  }
  ryml::set_callbacks(errh.defaults);    // restore defaults.
  errh.check_effect(/*committed*/ false);
  return out;
}
}    // namespace joda::yaml
