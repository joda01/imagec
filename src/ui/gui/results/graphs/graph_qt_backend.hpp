///
/// \file      graph_qt_backend.hpp
/// \author    Joachim Danmayr
/// \date      2025-06-15
///
/// \copyright Copyright 2019 Joachim Danmayr
///            This software is licensed for **non-commercial** use only.
///            Educational, research, and personal use are permitted.
///            For **Commercial** please contact the copyright owner.
///

#pragma once

#include <matplot/backend/backend_interface.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglversionfunctions.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <iostream>
#include <vector>

namespace joda::ui::gui {

class QtBackend : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core, public matplot::backend::backend_interface
{
  Q_OBJECT
public:
  QtBackend()
  {
    // setFixedSize(500, 500);
  }

  ~QtBackend();

  bool is_interactive() override;

  void create_shaders();

  /// \brief If non-interactive, get the file where we should output
  /// our data
  const std::string &output() override;

  /// \brief If non-interactive, get the format in which we should
  /// output our data
  const std::string &output_format() override;

  /// \brief If non-interactive, set the file where we should output
  /// our data This function can use the file extension to
  /// automatically set the output format
  bool output(const std::string &filename) override;
  /// \brief If non-interactive, set the file and the file format for
  /// outputting data
  bool output(const std::string &filename, const std::string &file_format) override;
  /// Get the current width
  /// The user might have changed the image width manually.
  /// Matplot++ needs to be aware of that.
  unsigned int width() override;

  /// \brief Get height
  unsigned int height() override;

  /// \brief Set width
  /// For when the user programmatically sets the width
  void width(unsigned int new_width) override;
  /// \brief Set height
  void height(unsigned int new_height) override;
  /// \brief Get the current position_x (for interactive backends)
  /// The user might have changed the image position_x manually.
  /// Matplot++ needs to be aware of that.
  unsigned int position_x() override;
  /// \brief Get position_y (for interactive backends)
  unsigned int position_y() override;
  /// \brief Set position_x (for interactive backends)
  /// For when the user programmatically sets the position_x
  void position_x(unsigned int new_position_x) override;
  /// \brief Set position_y (for interactive backends)
  void position_y(unsigned int new_position_y) override;
  /// \brief Set window title
  void window_title(const std::string &title)
  {
    setWindowTitle(title.data());
  }

  /// \brief Get window title
  virtual std::string window_title()
  {
    return windowTitle().toStdString();
  }

  /// \brief Tell the backend we are about to draw a new image
  /// The backend might reject starting this new image
  /// For instance, the user already closed the window
  /// and there's no point in feeding commands to the backend
  bool new_frame();
  /// \brief Tell the backend this new image is over
  /// The backend is free to plot whatever it's been
  /// buffering
  /// \return True if everything is ok
  bool render_data() override;

  /// \brief Tell the backend to wait for user interaction
  /// Until then, the backend should block execution if possible
  /// Figures use this in the show function
  void show(matplot::figure_type *) override;

  /// \brief True if the user requested to close the window
  /// This function allows the backend to send a signal
  /// indicating the user has asked to close the window
  bool should_close() override;

  /// \brief True if the backend supports fonts
  /// We can avoid some commands if it doesn't
  bool supports_fonts() override;

  /// Public functions you need to override to create a new
  /// backend based on vertices, such as OpenGL, Agg, etc...
  /// These functions are likely to change as we come up
  /// with concrete implementations based on vertices.
  /// \see
  /// https://github.com/matplotlib/matplotlib/blob/master/src/_backend_agg.h
  /// \see https://github.com/ocornut/imgui/tree/master/examples
public:
  /// \brief Draws background on the image
  virtual void draw_background(const std::array<float, 4> &color);

  /// \brief Draws rectangle on the image
  virtual void draw_rectangle(const double x1, const double x2, const double y1, const double y2, const std::array<float, 4> &color);

  /// \brief Draw a path on the image
  /// Many backends will require the path to be floats
  /// but Matplot++ words with doubles, so it's up to
  /// the backend to implement this conversion however
  /// it seems more efficient
  virtual void draw_path(const std::vector<double> &x, const std::vector<double> &y, const std::array<float, 4> &color);

  /// \brief Draw markers on the image
  virtual void draw_markers(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z = {});

  /// \brief Draw text on the image
  virtual void draw_text(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z = {});

  /// \brief Draw image matrix on the image
  virtual void draw_image(const std::vector<std::vector<double>> &x, const std::vector<std::vector<double>> &y,
                          const std::vector<std::vector<double>> &z = {});

  /// \brief Draw rectangle on the image
  virtual void draw_triangle(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z = {});

  /// We can certainly include more functions here, such as
  /// draw_mesh, draw_rectangle, etc...
  /// However, these functions should have a default implementation
  /// that would recur to more primitive functions.
  /// For instance, draw_rectangle should have a default
  /// implementation to draw a rectangle based on two calls to
  /// draw_triangle (for filled rectangles) or an implementation based
  /// on draw_path (for unfilled rectangles). Otherwise, it would be
  /// very expensive to start a new backend. These function should be
  /// complementary functions that would be used to improve
  /// performance on existing backends.

  /// Public functions you need to override only if your backend
  /// is based on gnuplot
  /// If not, just leave it as it is and the default implementations
  /// should do just fine.
public:
  /// \brief If true, this backend does not work by consuming vertices
  /// Most functions above will be ignored.
  /// The figure object will send gnuplot commands to this backend
  /// instead of vertices.
  /// The default implementation returns false.
  bool consumes_gnuplot_commands()
  {
    return false;
  }

signals:
  void resizeEvent(int w, int h);

private:
  void initializeGL() override
  {
    initializeOpenGLFunctions();
    create_shaders();
  }
  void resizeGL(int w, int h) override
  {
    glViewport(0, 0, w, h);
    emit resizeEvent(w, h);
  }
  void paintGL() override;
  void paintRectGL(const double x1, const double x2, const double y1, const double y2, const std::array<float, 4> &color);
  void paintPathGL(const std::vector<double> &x, const std::vector<double> &y, const std::array<float, 4> &color);

  static constexpr unsigned int default_screen_width  = 560;
  static constexpr unsigned int default_screen_height = 420;

  unsigned int draw_2d_single_color_shader_program_;
  int n_vertex_attributes_available_;
  unsigned int height_{default_screen_height};
  unsigned int width_{default_screen_width};

  struct DrawRectangle
  {
    double x1 = 0;
    double x2 = 0;
    double y1 = 0;
    double y2 = 0;
    std::array<float, 4> color;
  };
  std::vector<DrawRectangle> mRects;

  struct DrawPath
  {
    const std::vector<double> x;
    const std::vector<double> y;
    const std::array<float, 4> color;
  };
  std::vector<DrawPath> mPaths;
  std::array<float, 4> mBackground;
};
}    // namespace joda::ui::gui
