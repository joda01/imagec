///
/// \file      pipeline_indexes.hpp
/// \author    Joachim Danmayr
/// \date      2024-08-11
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include "backend/helper/helper.hpp"
#include <nlohmann/json.hpp>

namespace joda::enums {
enum class ClassIdIn : uint16_t
{
  C0  = 0,
  C1  = 1,
  C2  = 2,
  C3  = 3,
  C4  = 4,
  C5  = 5,
  C6  = 6,
  C7  = 7,
  C8  = 8,
  C9  = 9,
  C10 = 10,
  C11 = 11,
  C12 = 12,
  C13 = 13,
  C14 = 14,
  C15 = 15,
  C16 = 16,
  C17 = 17,
  C18 = 18,
  C19 = 19,
  C20 = 20,
  C21 = 21,
  C22 = 22,
  C23 = 23,
  C24 = 24,
  C25 = 25,
  C26 = 26,
  C27 = 27,
  C28 = 28,
  C29 = 29,
  C30 = 30,
  C31 = 31,
  C32 = 32,
  C33 = 33,
  C34 = 34,
  C35 = 35,
  C36 = 36,
  C37 = 37,
  C38 = 38,
  C39 = 39,
  C40 = 40,
  C41 = 41,
  C42 = 42,
  C43 = 43,
  C44 = 44,
  C45 = 45,
  C46 = 46,
  C47 = 47,
  C48 = 48,
  C49 = 49,
  // Reserved for temporary classes
  RESERVED_FOR_TEMP_START = 0x400,
  RESERVED_FOR_TEMP_END   = 0x710,
  // Ende
  TEMP_01   = 0xFFF0,
  TEMP_02   = 0xFFF1,
  TEMP_03   = 0xFFF2,
  TEMP_04   = 0xFFF3,
  TEMP_LAST = 0xFFF4,
  NONE      = 0xFFFD,
  UNDEFINED = 0xFFFE,
  $         = 0xFFFF
};

enum class ClassId : uint16_t
{
  C0   = 0,
  C1   = 1,
  C2   = 2,
  C3   = 3,
  C4   = 4,
  C5   = 5,
  C6   = 6,
  C7   = 7,
  C8   = 8,
  C9   = 9,
  C10  = 10,
  C11  = 11,
  C12  = 12,
  C13  = 13,
  C14  = 14,
  C15  = 15,
  C16  = 16,
  C17  = 17,
  C18  = 18,
  C19  = 19,
  C20  = 20,
  C21  = 21,
  C22  = 22,
  C23  = 23,
  C24  = 24,
  C25  = 25,
  C26  = 26,
  C27  = 27,
  C28  = 28,
  C29  = 29,
  C30  = 30,
  C31  = 31,
  C32  = 32,
  C33  = 33,
  C34  = 34,
  C35  = 35,
  C36  = 36,
  C37  = 37,
  C38  = 38,
  C39  = 39,
  C40  = 40,
  C41  = 41,
  C42  = 42,
  C43  = 43,
  C44  = 44,
  C45  = 45,
  C46  = 46,
  C47  = 47,
  C48  = 48,
  C49  = 49,
  CMAX = 50,
  // Reserved for temporary classes
  RESERVED_FOR_TEMP_START = 0x400,
  RESERVED_FOR_TEMP_END   = 0x710,
  NONE                    = 0xFFFD,
  UNDEFINED               = 0xFFFE,
};

inline auto getPrefixFromClassName(const std::string &className) -> std::string
{
  auto areas = joda::helper::split(joda::helper::trim(className), {'@'});
  if(areas.size() > 1) {
    return joda::helper::trim(areas[0]);
  }
  return "";
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClassId, {
                                          {ClassId::NONE, "None"}, {ClassId::UNDEFINED, "Undefined"},
                                          {ClassId::C0, "0"},      {ClassId::C1, "1"},
                                          {ClassId::C2, "2"},      {ClassId::C3, "3"},
                                          {ClassId::C4, "4"},      {ClassId::C5, "5"},
                                          {ClassId::C6, "6"},      {ClassId::C7, "7"},
                                          {ClassId::C8, "8"},      {ClassId::C9, "9"},
                                          {ClassId::C10, "10"},    {ClassId::C11, "11"},
                                          {ClassId::C12, "12"},    {ClassId::C13, "13"},
                                          {ClassId::C14, "14"},    {ClassId::C15, "15"},
                                          {ClassId::C16, "16"},    {ClassId::C17, "17"},
                                          {ClassId::C18, "18"},    {ClassId::C19, "19"},
                                          {ClassId::C20, "20"},    {ClassId::C21, "21"},
                                          {ClassId::C22, "22"},    {ClassId::C23, "23"},
                                          {ClassId::C24, "24"},    {ClassId::C25, "25"},
                                          {ClassId::C26, "26"},    {ClassId::C27, "27"},
                                          {ClassId::C28, "28"},    {ClassId::C29, "29"},
                                          {ClassId::C30, "30"},    {ClassId::C31, "31"},
                                          {ClassId::C32, "32"},    {ClassId::C33, "33"},
                                          {ClassId::C34, "34"},    {ClassId::C35, "35"},
                                          {ClassId::C36, "36"},    {ClassId::C37, "37"},
                                          {ClassId::C38, "38"},    {ClassId::C39, "39"},
                                          {ClassId::C40, "40"},    {ClassId::C41, "41"},
                                          {ClassId::C42, "42"},    {ClassId::C43, "43"},
                                          {ClassId::C44, "44"},    {ClassId::C45, "45"},
                                          {ClassId::C46, "46"},    {ClassId::C47, "47"},
                                          {ClassId::C48, "48"},    {ClassId::C49, "49"},
                                      });

NLOHMANN_JSON_SERIALIZE_ENUM(ClassIdIn, {
                                            {ClassIdIn::NONE, "None"},   {ClassIdIn::UNDEFINED, "Undefined"},
                                            {ClassIdIn::$, "$"},         {ClassIdIn::C0, "0"},
                                            {ClassIdIn::C1, "1"},        {ClassIdIn::C2, "2"},
                                            {ClassIdIn::C3, "3"},        {ClassIdIn::C4, "4"},
                                            {ClassIdIn::C5, "5"},        {ClassIdIn::C6, "6"},
                                            {ClassIdIn::C7, "7"},        {ClassIdIn::C8, "8"},
                                            {ClassIdIn::C9, "9"},        {ClassIdIn::C10, "10"},
                                            {ClassIdIn::C11, "11"},      {ClassIdIn::C12, "12"},
                                            {ClassIdIn::C13, "13"},      {ClassIdIn::C14, "14"},
                                            {ClassIdIn::C15, "15"},      {ClassIdIn::C16, "16"},
                                            {ClassIdIn::C17, "17"},      {ClassIdIn::C18, "18"},
                                            {ClassIdIn::C19, "19"},      {ClassIdIn::C20, "20"},
                                            {ClassIdIn::C21, "21"},      {ClassIdIn::C22, "22"},
                                            {ClassIdIn::C23, "23"},      {ClassIdIn::C24, "24"},
                                            {ClassIdIn::C25, "25"},      {ClassIdIn::C26, "26"},
                                            {ClassIdIn::C27, "27"},      {ClassIdIn::C28, "28"},
                                            {ClassIdIn::C29, "29"},      {ClassIdIn::C30, "30"},
                                            {ClassIdIn::C31, "31"},      {ClassIdIn::C32, "32"},
                                            {ClassIdIn::C33, "33"},      {ClassIdIn::C34, "34"},
                                            {ClassIdIn::C35, "35"},      {ClassIdIn::C36, "36"},
                                            {ClassIdIn::C37, "37"},      {ClassIdIn::C38, "38"},
                                            {ClassIdIn::C39, "39"},      {ClassIdIn::C40, "40"},
                                            {ClassIdIn::C41, "41"},      {ClassIdIn::C42, "42"},
                                            {ClassIdIn::C43, "43"},      {ClassIdIn::C44, "44"},
                                            {ClassIdIn::C45, "45"},      {ClassIdIn::C46, "46"},
                                            {ClassIdIn::C47, "47"},      {ClassIdIn::C48, "48"},
                                            {ClassIdIn::C49, "49"},      {ClassIdIn::TEMP_01, "M01"},
                                            {ClassIdIn::TEMP_02, "M02"}, {ClassIdIn::TEMP_03, "M03"},
                                            {ClassIdIn::TEMP_04, "M04"},
                                        });

}    // namespace joda::enums
