///
/// \file      graph_qt_backend.cpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#include "graph_qt_backend.hpp"
#include <matplot/backend/gnuplot.h>
#include <matplot/util/common.h>
#include <matplot/util/popen.h>
#include <qnamespace.h>
#include <QFile>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QPainter>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <regex>
#include <string>
#include <thread>

#include "backend/helper/logger/console_logger.hpp"

#ifdef __has_include
#if __has_include(<filesystem>)
#include <filesystem>
#else
#define CXX_NO_FILESYSTEM
#endif
#else
#define CXX_NO_FILESYSTEM
#endif

#ifdef MATPLOT_HAS_FBUFSIZE

#include <stdio_ext.h>

static size_t gnuplot_pipe_capacity(FILE *f)
{
  size_t sz = __fbufsize(f);
  return sz != 0 ? sz : matplot::backend::QtBackend::pipe_capacity_worst_case;
}

#else

namespace joda::ui::gui {

void QtBackend::paintEvent(QPaintEvent *event)
{
  QWidget::paintEvent(event);
  std::lock_guard<std::mutex> lock(mPaintMutex);
  if((svgRenderer != nullptr) && svgRenderer->isValid()) {
    QPainter painter(this);

    QSizeF svgSize    = svgRenderer->defaultSize();    // original SVG size
    QSizeF widgetSize = size();

    // Calculate scaled size preserving aspect ratio
    QSizeF scaledSize = svgSize;
    scaledSize.scale(widgetSize, Qt::KeepAspectRatio);

    // Center the SVG in the widget
    targetRect = QRectF((widgetSize.width() - scaledSize.width()) / 2.0, (widgetSize.height() - scaledSize.height()) / 2.0, scaledSize.width(),
                        scaledSize.height());

    svgRenderer->render(&painter, targetRect);

    //
    // This is a grid used for mouse events
    //
    QPen redPen(Qt::darkGray);
    redPen.setWidth(1);               // Optional: set line width
    redPen.setStyle(Qt::DashLine);    // <-- Make it dashed
    painter.setPen(redPen);
    float offsetX = (targetRect.width() / 7.7);
    float offsetY = (targetRect.height() / 13.3);
    float width   = ((targetRect.width() - 2.05 * offsetX) / mCols);
    float height  = ((targetRect.height() - 2.5 * offsetY) / mRows);
    mRects.clear();
    for(float col = 0; col < mCols; col++) {
      for(float row = 0; row < mRows; row++) {
        float startY = offsetY + targetRect.y() + row * height;
        float startX = offsetX + targetRect.x() + col * width;
        auto rect    = QRectF(startX, startY, width, height);
        mRects.push_back({rect, QPoint{static_cast<int>(row), static_cast<int>(col)}});
        painter.drawRect(rect);
      }
    }
  }
}

void QtBackend::mousePressEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
  QPoint pos = event->pos();
  for(const auto &[rect, pt] : mRects) {
    if(rect.contains(pos)) {
      std::cout << "Pressed " << std::to_string(pt.x()) << "|" << std::to_string(pt.y()) << std::endl;
    }
  }
}

////////////////////////////////////////////////////////

//
// Created by Alan Freitas on 26/08/20.
//

static size_t gnuplot_pipe_capacity(FILE *)
{
  return matplot::backend::gnuplot::pipe_capacity_worst_case;
}

#endif    // MATPLOT_HAS_FBUFSIZE

bool QtBackend::consumes_gnuplot_commands()
{
  return true;
}

QtBackend::QtBackend(const std::string &terminal, QWidget *parent) : QWidget(parent)
{
  if(!terminal.empty()) {
    if(terminal_is_available(terminal)) {
      terminal_ = terminal;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
  } else if(terminal_is_available("wxt")) {
    // 2nd option: wxt on windows, even if not default
    terminal_ = "wxt";
#endif
  } else if(terminal_is_available("qt")) {
    // 3rd option: qt
    terminal_ = "qt";
  } else {
    // 4rd option: default terminal type
    terminal_ = default_terminal_type();
  }

  // Open the gnuplot pipe_
  int perr;
  if constexpr(windows_should_persist_by_default) {
    perr = pipe_.open("gnuplot --persist");
  } else {
    perr = pipe_.open("gnuplot");
  }

  // Check if everything is OK
  if(perr != 0 || !pipe_.opened()) {
    std::cerr << "Opening the gnuplot failed: ";
    std::cerr << pipe_.error() << std::endl;
    std::cerr << "Please install gnuplot 5.2.6+: http://www.gnuplot.info" << std::endl;
  }

  setMouseTracking(true);    // <--- This line is crucial
}

QtBackend::~QtBackend()
{
  if constexpr(dont_let_it_close_too_fast) {
    auto time_since_last_flush = std::chrono::high_resolution_clock::now() - last_flush_;
    if(time_since_last_flush < std::chrono::seconds(5)) {
      std::this_thread::sleep_for(std::chrono::seconds(5) - time_since_last_flush);
    }
  }

  // Remove temp files
  std::filesystem::path svgFile(output());
  if(std::filesystem::exists(svgFile)) {
    std::filesystem::remove(svgFile);
  }

  flush_commands();
  run_command("exit");
  flush_commands();
}

bool QtBackend::is_interactive()
{
  return output_.empty();
}

const std::string &QtBackend::output()
{
  return output_;
}

const std::string &QtBackend::output_format()
{
  return terminal_;
}

#ifdef STRING_VIEW_CONSTEXPR_BUG
#define SV_CONSTEXPR
#else
#define SV_CONSTEXPR constexpr
#endif

bool QtBackend::output(const std::string &filename)
{
  if(filename.empty()) {
    output_   = filename;
    terminal_ = default_terminal_type();
    return true;
  }

  // look at the extension
#ifndef CXX_NO_FILESYSTEM
  namespace fs = std::filesystem;
  fs::path p{filename};
  std::string ext = p.extension().string();
#else
  std::string ext = filename.substr(filename.find_last_of('.'));
#endif

  // check terminal for that extension
  SV_CONSTEXPR auto exts = extension_terminal();
  auto it                = std::find_if(exts.begin(), exts.end(), [&](const auto &e) { return e.first == ext; });

  // if there is a terminal
  if(it != exts.end()) {
    // terminal name is the file format
    output(filename, std::string(it->second));
    return true;
  } else {
    // set file format to dumb
    std::cerr << "No gnuplot terminal for " << ext << " files" << std::endl;
    std::cerr << "Setting terminal to \"dumb\"" << std::endl;
    output(filename, "dumb");
    return false;
  }
}

bool QtBackend::output(const std::string &filename, const std::string &format)
{
  // If filename is empty - format should be interactive
  // We don't check extension_terminal because that's only
  // for non-interactive terminal
  if(filename.empty()) {
    output_   = filename;
    terminal_ = format;
    return true;
  }

  // Check if file format is valid
  SV_CONSTEXPR auto exts = extension_terminal();
  auto it                = std::find_if(exts.begin(), exts.end(), [&](const auto &e) { return e.second == format; });

  if(it == exts.end()) {
    std::cerr << format << " format does not exist for gnuplot backend" << std::endl;
    return false;
  }

  // Create file if it does not exist
#ifndef CXX_NO_FILESYSTEM
  namespace fs = std::filesystem;
  fs::path p{filename};
  if(!p.parent_path().empty() && !fs::exists(p.parent_path())) {
    fs::create_directory(p.parent_path());
    if(!fs::exists(p.parent_path())) {
      std::cerr << "Could not find or create " << p.parent_path() << std::endl;
      return false;
    }
  }
#endif

  output_   = filename;
  terminal_ = format;

  // Append extension if needed
#ifndef CXX_NO_FILESYSTEM
  std::string ext = p.extension().string();
#else
  std::string ext = filename.substr(filename.find_last_of('.'));
#endif
  if(ext.empty()) {
    output_ += it->first;
  }
  return true;
}

unsigned int QtBackend::width()
{
  return position_[2];
}

unsigned int QtBackend::height()
{
  return position_[3];
}

void QtBackend::width(unsigned int new_width)
{
  position_[2] = new_width;
  if(terminal_has_position_option(terminal_)) {
    run_command("set terminal " + terminal_ + " position " + matplot::num2str(position_[0]) + "," + matplot::num2str(position_[1]));
  }
  if(terminal_ == "dumb") {
    run_command("set terminal dumb " + matplot::num2str(position_[2]) + " " + matplot::num2str(position_[3]));
  } else {
    if(terminal_has_size_option(terminal_)) {
      run_command("set terminal " + terminal_ + " size " + matplot::num2str(position_[2]) + "," + matplot::num2str(position_[3]));
    }
  }
}

void QtBackend::height(unsigned int new_height)
{
  position_[3] = new_height;
  if(terminal_has_position_option(terminal_)) {
    run_command("set terminal " + terminal_ + " position " + matplot::num2str(position_[0]) + "," + matplot::num2str(position_[1]));
  }
  if(terminal_ == "dumb") {
    run_command("set terminal dumb " + matplot::num2str(position_[2]) + " " + matplot::num2str(position_[3]));
  } else {
    if(terminal_has_size_option(terminal_)) {
      run_command("set terminal " + terminal_ + " size " + matplot::num2str(position_[2]) + "," + matplot::num2str(position_[3]));
    }
  }
}

unsigned int QtBackend::position_x()
{
  return position_[0];
}

unsigned int QtBackend::position_y()
{
  return position_[1];
}

void QtBackend::position_x(unsigned int new_position_x)
{
  position_[0] = new_position_x;
  if(terminal_has_position_option(terminal_)) {
    run_command("set terminal " + terminal_ + " position " + matplot::num2str(position_[0]) + "," + matplot::num2str(position_[1]));
  }
  if(terminal_has_size_option(terminal_)) {
    run_command("set terminal " + terminal_ + " size " + matplot::num2str(position_[2]) + "," + matplot::num2str(position_[3]));
  }
}

void QtBackend::position_y(unsigned int new_position_y)
{
  position_[1] = new_position_y;
  if(terminal_has_position_option(terminal_)) {
    run_command("set terminal " + terminal_ + " position " + matplot::num2str(position_[0]) + "," + matplot::num2str(position_[1]));
  }
  if(terminal_has_size_option(terminal_)) {
    run_command("set terminal " + terminal_ + " size " + matplot::num2str(position_[2]) + "," + matplot::num2str(position_[3]));
  }
}

bool QtBackend::new_frame()
{
  // always accept starting a new frame
  return true;
}

bool QtBackend::render_data()
{
  using namespace std::chrono_literals;
  bool okay = flush_commands();

  //
  // We plot the graph to svg and then plot this svg in our qt widget
  //
  auto start = std::chrono::steady_clock::now();
  std::filesystem::path svgFile(output());
  do {
    std::this_thread::sleep_for(25ms);
    if(std::chrono::steady_clock::now() - start > 5s) {
      joda::log::logWarning("Could not plot graph: Timeout");
      return false;    // timeout expired
    }
    if(std::filesystem::exists(svgFile)) {
      auto size = std::filesystem::file_size(svgFile);
      if(size <= 0) {
        continue;
      }
      std::lock_guard<std::mutex> lock(mPaintMutex);
      delete svgRenderer;
      QFile file(svgFile.string().data());
      if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        joda::log::logDebug("Could not open graph file. Retry ...");
        file.close();
        continue;
      }
      QByteArray fileData = file.readAll();
      if(!fileData.contains("</svg>")) {
        file.close();
        continue;
      }
      joda::log::logDebug("Read file");

      svgRenderer = new QSvgRenderer(fileData, this);
      file.close();
      update();
      std::filesystem::remove(svgFile);
      break;
    }
  } while(true);

  return okay;
}

bool QtBackend::flush_commands()
{
  if constexpr(dont_let_it_close_too_fast) {
    last_flush_ = std::chrono::high_resolution_clock::now();
  }
  pipe_.flush("\n");
  if constexpr(trace_commands) {
    std::cout << "\n\n\n\n" << std::endl;
  }
  return true;
}

bool QtBackend::supports_fonts()
{
  return terminal_has_font_option(terminal_);
}

void QtBackend::run_command(const std::string &command)
{
  if(!pipe_.opened()) {
    return;
  }
  size_t pipe_capacity = gnuplot_pipe_capacity(pipe_.file());
  if(command.size() + bytes_in_pipe_ > pipe_capacity) {
    flush_commands();
    bytes_in_pipe_ = 0;
  }
  if(!command.empty()) {
    pipe_.write(command);
  }
  // proc_write(&pipe_, "; ");
  pipe_.write("\n");
  bytes_in_pipe_ += command.size();
  if constexpr(trace_commands) {
    std::cout << command << std::endl;
  }
}

void QtBackend::include_comment(const std::string &comment)
{
  if(include_comments_) {
    run_command("# " + comment);
  }
}

std::string QtBackend::default_terminal_type()
{
  static std::string terminal_type;
  const bool dont_know_term_type = terminal_type.empty();
  if(dont_know_term_type) {
    terminal_type                        = matplot::run_and_get_output("gnuplot -e \"show terminal\" 2>&1");
    terminal_type                        = std::regex_replace(terminal_type, std::regex("[^]*terminal type is ([^ ]+)[^]*"), "$1");
    const bool still_dont_know_term_type = terminal_type.empty();
    if(still_dont_know_term_type) {
      terminal_type = "qt";
    }
  }
  return terminal_type;
}

bool QtBackend::terminal_is_available(std::string_view term)
{
  std::string msg = matplot::run_and_get_output("gnuplot -e \"set terminal " + std::string(term.data()) + "\" 2>&1");
  return msg.empty();
}

std::tuple<int, int, int> QtBackend::gnuplot_version()
{
  static std::tuple<int, int, int> version{0, 0, 0};
  const bool dont_know_gnuplot_version_yet = version == std::tuple<int, int, int>({0, 0, 0});
  if(dont_know_gnuplot_version_yet) {
    std::string version_str   = matplot::run_and_get_output("gnuplot --version 2>&1");
    std::string version_major = std::regex_replace(version_str, std::regex("[^]*gnuplot (\\d+)\\.\\d+ patchlevel \\d+ *"), "$1");
    std::string version_minor = std::regex_replace(version_str, std::regex("[^]*gnuplot \\d+\\.(\\d+) patchlevel \\d+ *"), "$1");
    std::string version_patch = std::regex_replace(version_str, std::regex("[^]*gnuplot \\d+\\.\\d+ patchlevel (\\d+) *"), "$1");
    try {
      std::get<0>(version) = std::stoi(version_major);
    } catch(...) {
      std::get<0>(version) = 0;
    }
    try {
      std::get<1>(version) = std::stoi(version_minor);
    } catch(...) {
      std::get<1>(version) = 0;
    }
    try {
      std::get<2>(version) = std::stoi(version_patch);
    } catch(...) {
      std::get<2>(version) = 0;
    }
    const bool still_dont_know_gnuplot_version = version == std::tuple<int, int, int>({0, 0, 0});
    if(still_dont_know_gnuplot_version) {
      // assume it's 5.2.6 by convention
      version = std::tuple<int, int, int>({5, 2, 6});
    }
  }
  return version;
}

bool QtBackend::terminal_has_title_option(const std::string &t)
{
  SV_CONSTEXPR std::string_view whitelist[] = {"qt", "aqua", "caca", "canvas", "windows", "wxt", "x11"};
  return std::find(std::begin(whitelist), std::end(whitelist), t) != std::end(whitelist);
}

bool QtBackend::terminal_has_size_option(const std::string &t)
{
  // Terminals that have the size option *in the way we expect it to work*
  // This includes only the size option with {width, height} and not
  // the size option for cropping or scaling
  SV_CONSTEXPR std::string_view whitelist[] = {"qt",  "aqua", "caca",     "canvas",  "eepic",    "emf",     "gif", "jpeg",
                                               "pbm", "png",  "pngcairo", "sixelgd", "tkcanvas", "windows", "wxt", "svg"};
  return std::find(std::begin(whitelist), std::end(whitelist), t) != std::end(whitelist);
}

bool QtBackend::terminal_has_position_option(const std::string &t)
{
  SV_CONSTEXPR std::string_view whitelist[] = {"qt", "windows", "wxt"};
  return std::find(std::begin(whitelist), std::end(whitelist), t) != std::end(whitelist);
}

bool QtBackend::terminal_has_enhanced_option(const std::string &t)
{
  SV_CONSTEXPR std::string_view whitelist[] = {"canvas",   "postscript", "qt",      "aqua",     "caca", "canvas", "dumb",     "emf",
                                               "enhanced", "jpeg",       "pdf",     "pdfcairo", "pm",   "png",    "pngcairo", "postscript",
                                               "sixelgd",  "tkcanvas",   "windows", "wxt",      "x11",  "ext",    "wxt"};
  return std::find(std::begin(whitelist), std::end(whitelist), t) != std::end(whitelist);
}

bool QtBackend::terminal_has_color_option(const std::string &t)
{
  SV_CONSTEXPR std::string_view whitelist[] = {"postscript", "aifm",     "caca",       "cairolatex", "context", "corel", "eepic",  "emf",
                                               "epscairo",   "epslatex", "fig",        "lua tikz",   "mif",     "mp",    "pbm",    "pdf",
                                               "pdfcairo",   "pngcairo", "postscript", "pslatex",    "pstex",   "tgif",  "windows"};
  return std::find(std::begin(whitelist), std::end(whitelist), t) != std::end(whitelist);
}

bool QtBackend::terminal_has_font_option(const std::string &t)
{
  // This includes terminals that don't have the font option
  // and terminals for which we want to use only the default fonts
  // We prefer a blacklist because it's better to get a warning
  // in a false positive than remove the fonts in a false negative.
  SV_CONSTEXPR std::string_view blacklist[] = {
      "dxf",      "eepic",    "emtex",    "hpgl",  "latex",   "mf",     "pcl5",     "pslatex", "pstex",   "pstricks", "qms",   "tek40xx",
      "tek410x",  "texdraw",  "tkcanvas", "vttek", "xterm",   "jpeg",   "dumb",     "tpic",    "sixelgd", "png",      "lua",   "jpeg",
      "epscairo", "epslatex", "canvas",   "dumb",  "dxy800a", "emxvga", "pdfcairo", "cgi",     "gif",     "gpic",     "grass", "hp2623a",
      "hp2648",   "hp500c",   "hpgl",     "pcl5",  "hpljii",  "hppj",   "imagen",   "linux",   "lua",     "lua tikz", "mf",    "mif",
      "mp",       "pbm",      "pm",       "qms",   "regis",   "svga",   "texdraw",  "tikz",    "tpic",    "vgagl",    "vws",   "pdf"};
  return std::find(std::begin(blacklist), std::end(blacklist), t) == std::end(blacklist);
}

}    // namespace joda::ui::gui
