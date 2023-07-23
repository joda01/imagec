#include "report_printer.h"
#include <fstream>
#include <iostream>
#include <sstream>    // added for std::istringstream
#include <string>
#include <vector>

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
