#ifndef GLVIEW_H
#define GLVIEW_H

#include <QOpenGLWidget>
#include <QtOpenGL>

#ifdef __cplusplus
extern "C" {
#endif

#include "../viewer.h"

#ifdef __cplusplus
}
#endif

class glView : public QOpenGLWidget {
 public:
  glView(QWidget *parent);
  QSettings *settings;
  ObjData data_obj{0, 0, nullptr, nullptr, 0, 0};
  int projection_type = 0;
  GLfloat normalize_coef;
  int width_line = 1;
  int index_line = 0;
  int index_vert = 0;
  int size_point = 0;
  double move_x = 0;
  double move_y = 0;
  double move_z = 0;
  double rotate_x = 0;
  double rotate_y = 0;
  double rotate_z = 0;
  double scale = 1;
  QColor edge_color = QColor(255, 255, 255);
  QColor background_color = QColor(128, 0, 128);
  QColor vertex_color;

  void setGLProjection();
  void setGLEdgeType();
  void paintVertices();
  int getdata_obj(const char *fileName);
  void record_settings();
  void load_settings();

 protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
};

#endif  // GLVIEW_H
