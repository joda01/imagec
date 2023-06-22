#include "report_printer.h"
#include <fstream>
#include <iostream>
#include <sstream>    // added for std::istringstream
#include <string>
#include <vector>
#include "helper/termbox/termbox2.h"

// Function to split a string into a vector of strings
std::vector<std::string> split_string(const std::string &str, char delimiter)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream token_stream(str);
  while(std::getline(token_stream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// Function to read a CSV file into a vector of vector of strings
std::vector<std::vector<std::string>> read_csv_file(const std::string &filename, char delimiter)
{
  std::vector<std::vector<std::string>> rows;
  std::ifstream input_file(filename);
  if(input_file.is_open()) {
    std::string line;
    while(std::getline(input_file, line)) {
      rows.push_back(split_string(line, delimiter));
      line = "";
    }
    input_file.close();
  }
  return rows;
}

// Function to draw a table on the screen
void draw_table(int x, int y, int width, int height, const std::vector<std::vector<std::string>> &rows)
{
  int num_rows    = rows.size();
  int num_columns = num_rows > 0 ? rows[0].size() : 0;
  int cell_width  = 30;    // width / num_columns;
  int cell_height = 2;     // height / num_rows;
  // Draw horizontal lines
  for(int i = 0; i <= num_rows; i++) {
    int line_y = y + i * cell_height;
    for(int j = 0; j < num_columns * cell_width; j++) {
      char sign = '-';
      if(line_y == y) {
        sign = ' ';
      }

      tb_set_cell(x + j, line_y, sign, TB_WHITE, TB_DEFAULT);
    }
  }
  // Draw vertical lines
  for(int i = 0; i <= num_columns; i++) {
    int line_x = x + i * cell_width;

    int m = num_rows * cell_height + 1;
    for(int j = 0; j < m; j++) {
      char sign = '|';
      if(j == 0) {
        sign = ' ';
      } else if(j == m - 1) {
        sign = '+';
      }
      tb_set_cell(line_x, y + j, sign, TB_WHITE, TB_DEFAULT);
    }
  }
  // Draw cells

  for(int i = 0; i < num_rows; i++) {
    for(int j = 0; j < num_columns; j++) {
      std::string cell_text = rows[i][j];
      int cell_x            = x + j * cell_width + 1;
      int cell_y            = y + i * cell_height + 1;
      for(int k = 0; k < cell_width - 2; k++) {
        if(k < cell_text.size()) {
          if(i == 0 || j == 0) {
            tb_set_cell(cell_x + k, cell_y, cell_text[k], TB_CYAN | TB_BOLD, TB_DEFAULT);
          } else {
            tb_set_cell(cell_x + k, cell_y, cell_text[k], TB_DEFAULT, TB_DEFAULT);
          }
        } else {
          tb_set_cell(cell_x + k, cell_y, ' ', TB_WHITE, TB_DEFAULT);
        }
      }
    }
  }
}

void ReportPrinter::printTable(const std::string &csvFileName)
{
  // Read the CSV file
  std::vector<std::vector<std::string>> rows = read_csv_file(csvFileName, ',');

  // Get the screen size
  int screen_width  = tb_width();
  int screen_height = tb_height();

  // Clear the screen
  tb_clear();

  // Draw the table
  tb_printf(0, 0, TB_DEFAULT | TB_ITALIC, 0, csvFileName.c_str());
  draw_table(0, 1, screen_width, screen_height, rows);

  // Refresh the screen
  tb_present();
}
