///
/// \file      navigation.cpp
/// \author    Joachim Danmayr
/// \date      2023-04-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            All rights reserved! This file is subject
///            to the terms and conditions defined in file
///            LICENSE.txt, which is part of this package.
///
/// \brief     A short description what happens here.
///

#include "navigation.hpp"
#include <stdio.h>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include "image_processor/image_processor.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"

Navigation::Navigation()
{
}

void Navigation::start()
{
  menuMain();
}

///
/// \brief      Print a menu
/// \author     Joachim Danmayr
///
void Navigation::menuMain()
{
  std::string selection = "";
  do {
    printLogo();

    std::cout << "1... Set Input folder";
    if(!mSelectedInputFolder.empty()) {
      std::cout << " (" << mSelectedInputFolder << ")" << std::endl;
    } else {
      std::cout << std::endl;
    }
    if(!mSelectedInputFolder.empty()) {
      std::cout << "s... Start analyzes" << std::endl;
    }
    std::cout << "x... Exit" << std::endl;
    std::cout << std::endl;
    selection = readFromConsole(":", "");

    //
    // Select folder
    //
    if("1" == selection) {
      mSelectedInputFolder = menuGetInputFolder();
    }

    //
    // Start analysis
    //
    if("s" == selection) {
      joda::processor::ImageProcessor<::joda::pipeline::NucleusCounter> processor(mSelectedInputFolder,
                                                                                  mSelectedInputFolder + "/result");
      processor.start();
    }

    //
    // Exit application
    //
    if("x" == selection) {
      return;
    }

  } while(selection != "x");
}

auto Navigation::menuGetInputFolder() -> std::string
{
  bool folderExists = false;
  std::string read;
  clearScreen();
  std::cout << "> Main/Select input folder\n" << std::endl;
  do {
    read = readFromConsole("(" + mSelectedInputFolder + ") :", mSelectedInputFolder);

    if(std::filesystem::exists(read) && std::filesystem::is_directory(read)) {
      folderExists = true;
    } else {
      std::cout << "This is not a valid folder!" << std::endl;
    }
  } while(!folderExists);
  return read;
}

///
/// \brief      Print logo imageC
/// \author     Joachim Danmayr
///
void Navigation::printLogo()
{
  clearScreen();
  std::cout << "rev.: v1.0.0-alpha1" << std::endl;

  std::cout << "\
  \n \
   _                            ______ \n\
   (_)___ ___  ____ _____  ___  / ____/\n\
  / / __ `__ \\/ __ `/ __ `/ _ \\/ /     \n\
 / / / / / / / /_/ / /_/ /  __/ /___   \n\
/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/   \n\
                  /____/               \n\
                                       \
" << std::endl;
}

std::string Navigation::readFromConsole(const std::string &text, const std::string &def)
{
  std::cout << text;
  std::string input;
  std::getline(std::cin, input);    // read a line of input
  if(input.empty()) {
    std::cout << "You did not enter anything." << std::endl;
    return def;
  }
  return input;
}

void Navigation::clearScreen()
{
  system("clear");
}
