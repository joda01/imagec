

#include "plot_plate.hpp"
#include <string>

namespace joda::ui::gui {

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
auto preparePlateSurface(const joda::table::Table &table, int32_t rows, int32_t cols, int32_t colToDisplay, const PlotPlateSettings &settings)
    -> joda::table::Table
{
  if(rows == 0) {
    return {};
  }
  int32_t densityMapSize = settings.densityMapSize;
  if(rows > 50 && cols > 50) {
    densityMapSize = settings.densityMapSize * rows / 50;
    rows           = 50;
    cols           = 50;
  }

  std::vector<std::string> xLabels;
  std::vector<std::string> yLabels;

  for(int x = 0; x < cols; x++) {
    xLabels.emplace_back(std::to_string(x + 1));
  }

  for(int y = 0; y < rows; y++) {
    yLabels.emplace_back(numberToExcelColumn(y + 1));
  }

  double mMin = std::numeric_limits<double>::max();
  double mMax = std::numeric_limits<double>::min();

  struct Element
  {
    double val  = 0;
    int32_t cnt = 0;
    int32_t tblRow;
  };

  std::map<Pos, Element> densityMapVal;
  joda::table::Table data;
  data.init(cols, rows);

  for(int32_t tblRow = 0; tblRow < table.getNrOfRows(); tblRow++) {
    auto cellData    = table.data(tblRow, colToDisplay);
    uint32_t posX    = table.data(tblRow, colToDisplay).getPosX();
    uint32_t posY    = table.data(tblRow, colToDisplay).getPosY();
    uint32_t tStack  = table.data(tblRow, colToDisplay).getStackT();
    uint64_t groupId = table.data(tblRow, colToDisplay).getGroupId();
    if(tStack == 0) {
      posX--;    // The maps start counting at 1
      posY--;    // The maps start counting at 1
      if(densityMapSize > 0) {
        if(!cellData.isNAN() && cellData.isValid()) {
          posX = posX / densityMapSize;
          posY = posY / densityMapSize;
          densityMapVal[{posX, posY}].val += cellData.getVal();
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
    for(const auto &mapEntry : densityMapVal) {
      double val     = mapEntry.second.val / static_cast<double>(mapEntry.second.cnt);
      uint32_t posY  = mapEntry.first.posY;
      uint32_t posX  = mapEntry.first.posX;
      int32_t tblRow = mapEntry.second.tblRow;
      if(posX >= 0) {
        if(posY >= 0) {
          data.setData(posX, posY, {val, {.isValid = true}, {}});

          if(val < mMin) {
            mMin = val;
          }
          if(val > mMax) {
            mMax = val;
          }
        }
      }
    }
  }

  return data;
}

}    // namespace joda::ui::gui
