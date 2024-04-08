

#pragma once

namespace joda::pipeline::reporting {

/////////////////////////////////////////////////////
enum class ColumnIndexDetailedReport : int
{
  CONFIDENCE                  = 0x0100,
  AREA_SIZE                   = 0x0200,
  PERIMETER                   = 0x0300,
  CIRCULARITY                 = 0x0400,
  VALIDITY                    = 0x0500,
  INVALIDITY                  = 0x0600,
  CENTER_OF_MASS_X            = 0x0700,
  CENTER_OF_MASS_Y            = 0x0800,
  INTENSITY_AVG               = 0x0900,    // Start of the dynamic section
  INTENSITY_MIN               = 0x0A00,
  INTENSITY_MAX               = 0x0B00,
  INTENSITY_AVG_CROSS_CHANNEL = 0x0C00,    // Start of the dynamic section
  INTENSITY_MIN_CROSS_CHANNEL = 0x0D00,
  INTENSITY_MAX_CROSS_CHANNEL = 0x0E00,
  INTERSECTION_CROSS_CHANNEL  = 0x0F00
};
}    // namespace joda::pipeline::reporting
