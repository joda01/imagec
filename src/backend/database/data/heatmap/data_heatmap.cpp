

#include "data_heatmap.hpp"
#include <memory>
#include <optional>
#include <string>
#include "backend/enums/enum_measurements.hpp"
#include "backend/helper/table/table.hpp"

namespace joda::db::data {

struct Pos
{
  uint32_t posX = 0;
  uint32_t posY = 0;

  bool operator<(const Pos &in) const
  {
    uint64_t tmp  = static_cast<uint64_t>(posX) << 32 | posY;
    uint64_t tmp2 = static_cast<uint64_t>(in.posX) << 32 | in.posY;
    return tmp < tmp2;
  };
};

std::string numberToExcelColumn(int number)
{
  std::string result;

  while(number > 0) {
    number--;    // Excel columns are 1-based, but internally it's 0-based
    char ch = 'A' + (number % 26);
    result  = ch + result;
    number /= 26;
  }

  return result;
}

///
/// \brief
/// \author     Joachim Danmayr
/// \param[in]
/// \param[out]
/// \return
///
auto convertToHeatmap(const joda::table::Table *table, int32_t rows, int32_t cols, int32_t colToDisplay, int32_t tStackIn,
                      const PlotPlateSettings &settings) -> joda::table::Table
{
  if(rows == 0) {
    return {};
  }
  int32_t densityMapSize = settings.densityMapSize;
  if(rows > 100 || cols > 100) {
    densityMapSize = settings.densityMapSize * rows / 100;
    rows           = 100;
    cols           = 100;
  }

  struct Element
  {
    double val  = 0;
    int32_t cnt = 0;
    int32_t tblRow;
  };

  std::map<Pos, Element> densityMapVal;
  joda::table::Table data;
  data.init(cols, rows);

  for(int x = 0; x < cols; x++) {
    data.mutableColumns()[x].title = std::to_string(x + 1);
  }

  for(int y = 0; y < rows; y++) {
    if(nullptr == data.data(y, 0)) {
      data.setData(y, 0, std::make_shared<joda::table::TableCell>());
    }
    data.mutableData(y, 0)->setRowName(numberToExcelColumn(y + 1));
  }
  std::optional<joda::table::TableCell> cellTmp;    // Needed for the density map
  for(int32_t tblRow = 0; tblRow < table->getNrOfRows(); tblRow++) {
    auto cellData = std::make_shared<joda::table::TableCell>(table->data(tblRow, colToDisplay));
    if(cellData == nullptr) {
      continue;
    }
    uint32_t posX    = cellData->getPosX();
    uint32_t posY    = cellData->getPosY();
    uint32_t tStack  = cellData->getStackT();
    uint64_t groupId = cellData->getGroupId();
    cellData->setRowName(numberToExcelColumn(posY));
    if(tStack == tStackIn) {
      if(posX <= 0 || posY <= 0) {
        continue;
      }
      posX--;    // The maps start counting at 1
      posY--;    // The maps start counting at 1
      if(densityMapSize > 0) {
        if(!cellData->isNAN() && cellData->isValid()) {
          if(!cellTmp.has_value()) {
            cellTmp.emplace(cellData);
          }
          posX = std::round((double) posX / (double) densityMapSize);
          posY = std::round((double) posY / (double) densityMapSize);
          if(posX >= cols) {
            posX = cols - 1;
          }
          if(posY >= rows) {
            posY = rows - 1;
          }
          densityMapVal[{posX, posY}].val += cellData->getVal();
          densityMapVal[{posX, posY}].cnt++;
          densityMapVal[{posX, posY}].tblRow = tblRow;
        }
      } else {
        data.setData(posY, posX, cellData);
      }
    }
  }

  //
  // Calc mean and forward to vector
  //
  if(densityMapSize > 0) {
    for(const auto &[pos, value] : densityMapVal) {
      double val = value.val;
      if(table->columns().at(colToDisplay).colSettings.measureChannel != enums::Measurement::COUNT) {
        // Only calculate an average if not counting
        val /= static_cast<double>(value.cnt);
      }
      uint32_t posY  = pos.posY;
      uint32_t posX  = pos.posX;
      int32_t tblRow = value.tblRow;
      if(posX >= 0) {
        if(posY >= 0) {
          if(cellTmp.has_value()) {
            cellTmp->setRowName(numberToExcelColumn(posY + 1));
            cellTmp->setVal(val);
            data.setData(posY, posX, cellTmp.value());
          }
        }
      }
    }
  }

  return data;
}

}    // namespace joda::db::data
