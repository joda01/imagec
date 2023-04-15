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
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include "helper/termbox/termbox2.h"
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
/// \brief      Print the main menu
/// \author     Joachim Danmayr
///
void Navigation::menuMain()
{
  int y = 10;

  std::string selection = "";
  do {
    y = 10;

    printLogo();

    tb_printf(0, y++, TB_DEFAULT, 0, "1... Set Input folder");
    if(!mSelectedInputFolder.empty()) {
      tb_printf(0, y++, TB_DEFAULT, 0, "2... Start analyzes");
    }
    tb_printf(0, y++, TB_DEFAULT, 0, "x... Exit");
    y++;
    tb_print(0, y, TB_DEFAULT, 0, ":");
    tb_present();
    selection = readFromConsole(y, "");
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

///
/// \brief      Read input folder and returns the selected folder.
///             If no folder selected, the previous selected is returned.
///             If folder does not exists user is asked to try again.
/// \author     Joachim Danmayr
/// \return     Selected folder
///
auto Navigation::menuGetInputFolder() -> std::string
{
  bool folderExists = false;
  std::string read;
  clearScreen();
  std::cout << "> Main/Select input folder\n" << std::endl;
  do {
    read = readFromConsole(10, mSelectedInputFolder);

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
  tb_printf(0, 0, TB_DEFAULT, 0, "rev.: v1.0.0-alpha1");
  tb_printf(0, 1, TB_GREEN, 0, "    _                            ______ ");
  tb_printf(0, 2, TB_GREEN, 0, "   (_)___ ___  ____ _____  ___  / ____/");
  tb_printf(0, 3, TB_GREEN, 0, "  / / __ `__ \\/ __ `/ __ `/ _ \\/ /   ");
  tb_printf(0, 4, TB_GREEN, 0, " / / / / / / / /_/ / /_/ /  __/ /___   ");
  tb_printf(0, 5, TB_GREEN, 0, "/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/ ");
  tb_printf(0, 6, TB_GREEN, 0, "                  /____/               ");
  tb_printf(0, 7, TB_GREEN, 0, "                                       ");
  tb_present();
}

std::string Navigation::readFromConsole(int y, const std::string &def)
{
  char input_buffer[256] = {0};
  int cursor_x           = 1;

  while(true) {
    struct tb_event event;
    tb_poll_event(&event);
    if(event.type == TB_EVENT_KEY) {
      //
      // Enter pressed
      //
      if(event.key == TB_KEY_ENTER) {
        if(cursor_x <= 1) {
          return def;
        }
        while(cursor_x > 1) {
          cursor_x--;
          // Erase the character from the screen
          tb_set_cell(cursor_x, y, ' ', TB_DEFAULT, 0);
        }

        // The user pressed Enter, so we're done reading input
        auto readString = std::string(input_buffer);
        cursor_x        = 1;
        memset(input_buffer, 0, 256);
        return readString;

      } else if(event.ch) {
        //
        // Convert the Unicode code point to a UTF-8 string
        //
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        std::string utf8_string = convert.to_bytes(event.ch);

        // Add the UTF-8 string to the input buffer
        strncat(input_buffer, utf8_string.c_str(), utf8_string.length());

        // Display the character on the screen
        tb_set_cell(cursor_x, y, event.ch, TB_DEFAULT, 0);
        cursor_x++;
      }
      //
      // Check if the user pressed the backspace key
      //
      if((event.type == TB_EVENT_KEY && event.key == TB_KEY_BACKSPACE) ||
         (event.type == TB_EVENT_KEY && event.key == TB_KEY_BACKSPACE2)) {
        // Delete the last character from the input buffer
        if(cursor_x > 0) {
          cursor_x--;
          input_buffer[cursor_x] = '\0';
          // Erase the character from the screen
          tb_set_cell(cursor_x, y, ' ', TB_DEFAULT, 0);
        }
      }
    }
    tb_present();
  }
}

///
/// \brief      Clear screen
/// \author     Joachim Danmayr
///
void Navigation::clearScreen()
{
  system("clear");
}
