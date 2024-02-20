#include "glview.h"

glView::glView(QWidget *parent) : QOpenGLWidget(parent) {
  settings =
      new QSettings(QCoreApplication::applicationDirPath() + "/settings.conf",
                    QSettings::IniFormat);
}

void glView::initializeGL() { glEnable(GL_DEPTH_TEST); }
void glView::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void glView::paintGL() {
  setGLProjection();
  glClearColor(background_color.red() / 255.0, background_color.green() / 255.0,
               background_color.blue() / 255.0, 0);  // Цвет фона;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(scale / 2, scale / 2, scale / 2);
  glEnableClientState(GL_VERTEX_ARRAY);
  if (data_obj.vertex_count) {
    glVertexPointer(3, GL_DOUBLE, 0, data_obj.vertex_array);
  }
  paintVertices();
  setGLEdgeType();
  if (data_obj.vertex_count && data_obj.facet_count) {
    glBegin(GL_LINES);
    for (int i = 0; i < data_obj.facet_count; i++) {
      bool drawFacet = true;
      for (int j = 0; j < 3; j++) {
        int vertexIndex = 0;
        vertexIndex = data_obj.facet_array[i * 3 + j];
        if (vertexIndex < 0 || vertexIndex >= data_obj.vertex_count) {
          drawFacet = false;
          break;
        }
      }

      if (drawFacet) {
        for (int j = 0; j < 3; j++) {
          int vertexIndex = 0;
          vertexIndex = data_obj.facet_array[i * 3 + j];

          glVertex3dv(&data_obj.vertex_array[vertexIndex * 3]);
        }
      }
    }
    glEnd();
  }
  glDisableClientState(GL_VERTEX_ARRAY);
  record_settings();
}

void glView::setGLProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (projection_type == 0) {  // Центральная проекция
    glFrustum(-1 * normalize_coef, 1 * normalize_coef, -1 * normalize_coef,
              1 * normalize_coef, normalize_coef, 1000 * normalize_coef);
    glTranslatef(0, 0, -2 * normalize_coef);
    glRotatef(30, 1, 0, 0);
  } else {  // Паралельная проекция
    glOrtho(-1 * normalize_coef, 1 * normalize_coef, -1 * normalize_coef,
            1 * normalize_coef, -1 * normalize_coef, 1000 * normalize_coef);
    glTranslatef(0, -normalize_coef / 2, 0);
  }
}

void glView::setGLEdgeType() {
  glColor3d(edge_color.red() / 255.0, edge_color.green() / 255.0,
            edge_color.blue() / 255.0);
  glLineWidth(width_line);

  if (index_line == 1) {
    glEnable(GL_LINE_STIPPLE);  // Включаем режим пунктирных линий
    glLineStipple(2, 0X00FF);  // Устанавливаем шаблон для пунктирных линий
  } else {
    glDisable(GL_LINE_STIPPLE);  // Выключаем режим пунктирных линий
  }

  glDisable(GL_POINT_SMOOTH);
}

void glView::paintVertices() {
  glColor3d(vertex_color.red() / 255.0, vertex_color.green() / 255.0,
            vertex_color.blue() / 255.0);
  if (index_vert == 1) {
    glDisable(GL_POINT_SMOOTH);
    glPointSize(size_point);
  } else if (index_vert == 2) {
    glEnable(GL_POINT_SMOOTH);
    glPointSize(size_point);
  }
  if (index_vert != 0) {
    glDrawArrays(GL_POINTS, 0, data_obj.vertex_count);
  }
}

void glView::record_settings() {
  settings->beginGroup("settings");

  settings->setValue("edge_color", edge_color);
  settings->setValue("background_color", background_color);
  settings->setValue("vertex_color", vertex_color);
  settings->setValue("projection_type", projection_type);

  settings->setValue("index_line", index_line);
  settings->setValue("index_vert", index_vert);
  settings->setValue("width_line", width_line);
  settings->setValue("size_point", size_point);
  settings->setValue("move_x", move_x);
  settings->setValue("move_y", move_y);
  settings->setValue("move_z", move_z);
  settings->setValue("rotate_x", rotate_x);
  settings->setValue("rotate_y", rotate_y);
  settings->setValue("rotate_z", rotate_z);
  settings->setValue("scale", scale);

  settings->endGroup();
}

void glView::load_settings() {
  settings->beginGroup("settings");

  edge_color = settings->value("edge_color", edge_color).value<QColor>();
  background_color =
      settings->value("background_color", background_color).value<QColor>();
  vertex_color = settings->value("vertex_color").value<QColor>();

  projection_type = settings->value("projection_type", 0).toInt();

  index_line = settings->value("index_line", 0).toInt();
  index_vert = settings->value("index_vert", 0).toInt();
  width_line = settings->value("width_line", 1).toInt();
  size_point = settings->value("size_point", 0).toInt();
  move_x = settings->value("move_x", 0).toDouble();
  move_y = settings->value("move_y", 0).toDouble();
  move_z = settings->value("move_z", 0).toDouble();
  rotate_x = settings->value("rotate_x", 0).toDouble();
  rotate_y = settings->value("rotate_y", 0).toDouble();
  rotate_z = settings->value("rotate_z", 0).toDouble();
  scale = settings->value("scale", 1).toDouble();

  settings->endGroup();
}
