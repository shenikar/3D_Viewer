#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QColorDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QtOpenGL>
#include <iostream>

#include "glview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void setProjectionType();
  ObjData data_obj{0, 0, nullptr, nullptr, 0, 0};
  bool needToFree = false;

 private slots:
  void on_pushButton_chooseFile_clicked();

  void on_pushButton_apply_clicked();

  void on_background_color_clicked();

  void on_vertex_color_clicked();

  void on_edges_color_clicked();

  void on_central_clicked();

  void on_parallel_clicked();

  void widget_state();

  void on_screenshot_clicked();

  void on_gif_clicked();

  void gif_create(QString fileName);

 private:
  Ui::MainWindow *ui;
};
#endif  // MAINWINDOW_H
