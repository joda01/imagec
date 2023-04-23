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
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <future>
#include <iostream>
#include <ratio>
#include <string>
#include <thread>
#include "helper/termbox/termbox2.h"
#include "image_processor/image_processor.hpp"
#include "image_processor/image_processor_base.hpp"
#include "pipelines/nucleus_count/nucleus_count.hpp"
#include "reporting/report_printer.h"
#include "version.h"

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
  int y;

  std::string selection = "";
  do {
    tb_clear();
    tb_present();
    y = printLogo();

    if(mSelectedInputFolder.empty()) {
      tb_printf(0, y++, TB_DEFAULT, 0, "1... Set Input folder");
    } else {
      std::string toPrint = "1... Set Input folder (" + mSelectedInputFolder + ")";
      tb_printf(0, y++, TB_DEFAULT, 0, toPrint.c_str());
      tb_printf(0, y++, TB_DEFAULT, 0, "2... Start analyzes");
    }
    if(!mLastReport.empty()) {
      std::string toPrint = "3... Show result (" + mLastReport + ")";
      tb_printf(0, y++, TB_DEFAULT, 0, toPrint.c_str());
    }
    tb_printf(0, y++, TB_DEFAULT, 0, "x... Exit");
    y++;
    selection = readFromConsole(y, "Select: ", "");
    //
    // Select folder
    //
    if("1" == selection) {
      mSelectedInputFolder = menuGetInputFolder();
    }

    //
    // Start analysis
    //
    if("2" == selection && !mSelectedInputFolder.empty()) {
      menuStartAnalyzes();
    }

    //
    // Show report result
    //
    if("3" == selection && !mLastReport.empty()) {
      menuReportResult();
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
/// \brief      Start analyzes menu
/// \author     Joachim Danmayr
///
void Navigation::menuStartAnalyzes()
{
  tb_clear();
  tb_present();
  joda::processor::ImageProcessor<::joda::pipeline::NucleusCounter> processor(mSelectedInputFolder,
                                                                              mSelectedInputFolder + "/result");

  auto startTime    = std::chrono::high_resolution_clock::now();
  auto workerThread = processor.start();

  bool stopPaintProgress = false;

  //
  // Paint progress
  //
  auto paintProgress = [&](const joda::processor::ImageProcessorBase *processor) {
    auto [total, img] = processor->getProgress();

    auto t_end              = std::chrono::high_resolution_clock::now();
    double elapsedTimeMs    = std::chrono::duration<double, std::milli>(t_end - startTime).count();
    std::string timeElapsed = std::to_string(elapsedTimeMs / 1000.0) + " s";

    tb_printf(0, 0, TB_DEFAULT | TB_ITALIC, 0, timeElapsed.c_str());

    drawProgressBar(0, 2, 50, total.finished, total.total);
    drawProgressBar(0, 3, 50, img.finished, img.total);
    tb_present();
  };
  auto paintProgressLoop = [&](const joda::processor::ImageProcessorBase *processor) {
    while(!stopPaintProgress) {
      paintProgress(processor);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  };
  std::future<void> paintProgressThread = std::async(std::launch::async, paintProgressLoop, &processor);

  //
  // Allow to stop analyzing
  //
  auto waitForEsc = [&](joda::processor::ImageProcessorBase *processor) {
    std::string result;
    mReadFromConsoleCanceled = false;
    while(result != "stop" && !mReadFromConsoleCanceled) {
      result = readFromConsole(5, "Type [stop] to stop analysis: ", "");
    }
    if(result == "stop") {
      tb_printf(0, 5, TB_DEFAULT | TB_ITALIC, 0, "Stopping ...                            ");
      processor->stop();
    }
  };
  std::future<void> waitForEscThread = std::async(std::launch::async, waitForEsc, &processor);

  //
  // Wait until thread has been finished
  //
  workerThread.wait();
  stopPaintProgress = true;
  paintProgressThread.wait();
  cancelReadFromConsole();
  //
  // Work finished
  //
  paintProgress(&processor);
  tb_printf(0, 5, TB_DEFAULT | TB_ITALIC, 0, "Finished!                            ");
  mLastReport = processor.getReportFilePath();
  readFromConsole(7, "Press ENTER to continue ", "");

  //
  // Everything finished
  //
  cancelReadFromConsole();
  waitForEscThread.wait();
}

void Navigation::drawProgressBar(int x, int y, int width, float act, float total)
{
  // Calculate the width of the progress bar
  float progress = act / total;
  int bar_width  = (int) (progress * (width - 2));

  // Draw the progress bar and frame
  for(int i = 0; i < width; i++) {
    if(i == 0 || i == width - 1) {
      tb_set_cell(x + i, y, '|', TB_WHITE, TB_DEFAULT);
    } else if(i <= bar_width) {
      tb_set_cell(x + i, y, '=', TB_WHITE, TB_DEFAULT);
    } else {
      tb_set_cell(x + i, y, ' ', TB_WHITE, TB_DEFAULT);
    }
  }

  // Draw the progress percentage
  std::string txt = std::to_string((uint32_t) act) + "/" + std::to_string((uint32_t) total);
  tb_printf(x + width + 2, y, TB_WHITE, TB_DEFAULT, txt.c_str());
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
  tb_clear();
  tb_present();
  bool folderExists = false;
  std::string read;
  do {
    read = readFromConsole(2, "Input folder: ", mSelectedInputFolder);

    if(std::filesystem::exists(read) && std::filesystem::is_directory(read)) {
      folderExists = true;
    } else {
      tb_printf(0, 4, TB_RED | TB_ITALIC, 0, "Not a valid input folder!");
    }
  } while(!folderExists);
  return read;
}

///
/// \brief      Show report result
/// \author     Joachim Danmayr
///
void Navigation::menuReportResult()
{
  ReportPrinter::printTable(mLastReport);
  struct tb_event event;
  tb_poll_event(&event);
}

///
/// \brief      Print logo imageC
/// \author     Joachim Danmayr
///
int Navigation::printLogo()
{
  std::string header = "rev.  : " + Version::getVersion() + " | (c) 2023 Joachim Danmayr | Licensed under GPL-3.0";
  std::string hash   = "sha256: " + Version::getHash();
  tb_printf(0, 0, TB_DEFAULT | TB_ITALIC, 0, header.c_str());
  tb_printf(0, 1, TB_DEFAULT | TB_ITALIC, 0, hash.c_str());
  tb_printf(0, 2, TB_DEFAULT, 0, "    _                            ______ ");
  tb_printf(0, 3, TB_DEFAULT, 0, "   (_)___ ___  ____ _____  ___  / ____/");
  tb_printf(0, 4, TB_DEFAULT, 0, "  / / __ `__ \\/ __ `/ __ `/ _ \\/ /   ");
  tb_printf(0, 5, TB_DEFAULT, 0, " / / / / / / / /_/ / /_/ /  __/ /___   ");
  tb_printf(0, 6, TB_DEFAULT, 0, "/_/_/ /_/ /_/\\__,_/\\__, /\\___/\\____/ ");
  tb_printf(0, 7, TB_DEFAULT, 0, "                  /____/               ");
  tb_printf(0, 8, TB_DEFAULT, 0, "_______________________________________");
  tb_present();
  return 10;
}

///
/// \brief          Read a string from the console and returns the read string.
///                 If an empty string was given, def is returned.
/// \author         Joachim Danmayr
/// \param[in]      y  Y-Position to paint to input
/// \param[in]      desc Description to write in front of
/// \param[in]      def  Default value if empty string has been entered
/// \return
///
std::string Navigation::readFromConsole(int y, const std::string &desc, const std::string &def)
{
  char input_buffer[256] = {0};
  tb_print(0, y, TB_DEFAULT, 0, desc.c_str());
  tb_set_cursor(desc.size(), y);
  mReadFromConsoleCanceled = false;
  int cursor_x             = desc.size();
  tb_present();
  while(!mReadFromConsoleCanceled) {
    struct tb_event event;
    tb_peek_event(&event, 200);
    if(event.type == TB_EVENT_KEY) {
      //
      // Enter pressed
      //
      if(event.key == TB_KEY_ENTER) {
        if(cursor_x <= desc.size()) {
          return def;
        }
        while(cursor_x > desc.size()) {
          cursor_x--;
          // Erase the character from the screen
          tb_set_cell(cursor_x, y, ' ', TB_DEFAULT, 0);
        }

        // The user pressed Enter, so we're done reading input
        auto readString = std::string(input_buffer);
        cursor_x        = desc.size();
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
        tb_set_cursor(cursor_x, y);
      }
      //
      // Check if the user pressed the backspace key
      //
      if((event.type == TB_EVENT_KEY && event.key == TB_KEY_BACKSPACE) ||
         (event.type == TB_EVENT_KEY && event.key == TB_KEY_BACKSPACE2)) {
        // Delete the last character from the input buffer
        if(cursor_x > desc.size()) {
          cursor_x--;
          input_buffer[cursor_x] = '\0';
          // Erase the character from the screen
          tb_set_cell(cursor_x, y, ' ', TB_DEFAULT, 0);
          tb_set_cursor(cursor_x, y);
        }
      }
    }
    tb_present();
  }
  return def;
}

///
/// \brief      Clear screen
/// \author     Joachim Danmayr
///
void Navigation::clearScreen()
{
  system("clear");
}
