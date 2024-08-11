#pragma once

#include <cstdint>
#include <set>
#include "backend/settings/anaylze_settings_enums.hpp"
#include <nlohmann/json.hpp>

namespace joda::settings {

class Calculator
{
public:
  enum class Operation
  {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    AND,
    OR,
    XOR,
    NOT
  };

  Slot slot2          = Slot::$;
  Operation operation = Operation::ADD;

private:
  NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Calculator, slot2, operation);
};

NLOHMANN_JSON_SERIALIZE_ENUM(Calculator::Operation, {{Calculator::Operation::ADD, "Add"},
                                                     {Calculator::Operation::ADD, "Subtract"},
                                                     {Calculator::Operation::ADD, "Multiply"},
                                                     {Calculator::Operation::ADD, "Divide"},
                                                     {Calculator::Operation::ADD, "AND"},
                                                     {Calculator::Operation::ADD, "OR"},
                                                     {Calculator::Operation::ADD, "XOR"},
                                                     {Calculator::Operation::ADD, "NOT"}});

}    // namespace joda::settings
