#include "mainwindow.h"

#include "ui_mainwindow.h"

extern "C" {
#include "gif.h"
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setFixedWidth(1411);
  setFixedHeight(814);
  ui->openGLWidget->load_settings();
  widget_state();
}

MainWindow::~MainWindow() {
  removeData(&data_obj);
  delete ui;
}

void MainWindow::on_pushButton_chooseFile_clicked() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Choose file", "../objFiles", ".obj files (*.obj)");

  if (!fileName.isEmpty()) {
    if ((data_obj.facet_array || data_obj.vertex_array) && needToFree) {
      removeData(&data_obj);
    }

    std::string expression = fileName.toStdString();
    const char *str = expression.c_str();

    int code = parserObj(str, &data_obj);
    if (code == RESULT_FILE_NOT_FOUND) {
      ui->label_FileName->setText("File not found");
      needToFree = false;
    } else if (code == RESULT_MALLOC_ERROR) {
      ui->label_FileName->setText("MALLOC ERROR");
      needToFree = false;
    } else if (code) {
      ui->label_FileName->setText("Error! Can't open file.");
      needToFree = false;
    } else {
      ui->label_FileName->setText(str);
      ui->label_NumberOfFacets->setText(
          QString::number(data_obj.facet_count_ind));
      ui->label_NumberOfVertexes->setText(
          QString::number(data_obj.vertex_count));
      ui->openGLWidget->data_obj = data_obj;
      ui->openGLWidget->update();
      needToFree = true;
    }
  }
}

void MainWindow::on_pushButton_apply_clicked() {
  move_obj(&data_obj, ui->doubleSpinBox_move_x->value(),
           ui->doubleSpinBox_move_y->value(),
           ui->doubleSpinBox_move_z->value());
  rotate_obj(&data_obj, ui->doubleSpinBox_rotate_y->value(),
             ui->doubleSpinBox_rotate_x->value(),
             ui->doubleSpinBox_rotate_z->value());
  scale_obj(&data_obj, ui->doubleSpinBox_scale->value());

  ui->openGLWidget->index_line = ui->edge_type->currentIndex();
  ui->openGLWidget->width_line = ui->thickness->value();
  ui->openGLWidget->index_vert = ui->vertex_type->currentIndex();
  ui->openGLWidget->move_x = ui->doubleSpinBox_move_x->value();
  ui->openGLWidget->move_y = ui->doubleSpinBox_move_y->value();
  ui->openGLWidget->move_z = ui->doubleSpinBox_move_z->value();
  ui->openGLWidget->rotate_x = ui->doubleSpinBox_rotate_x->value();
  ui->openGLWidget->rotate_y = ui->doubleSpinBox_rotate_y->value();
  ui->openGLWidget->rotate_z = ui->doubleSpinBox_rotate_z->value();
  ui->openGLWidget->scale = ui->doubleSpinBox_scale->value();

  if (ui->vertex_type->currentIndex() == 0) {
    ui->openGLWidget->size_point = 0;
    ui->openGLWidget->index_vert = 0;
    std::cout << ui->vertex_type->currentIndex() << std::endl;
  } else {
    ui->openGLWidget->size_point = ui->size_vertex->value();
  }
  ui->openGLWidget->update();
}

void MainWindow::on_background_color_clicked() {
  ui->openGLWidget->background_color = QColorDialog::getColor(
      Qt::red, this, tr("colors"), QColorDialog::DontUseNativeDialog);
  ui->openGLWidget->update();
}

void MainWindow::on_vertex_color_clicked() {
  ui->openGLWidget->vertex_color = QColorDialog::getColor(
      Qt::red, this, tr("colors"), QColorDialog::DontUseNativeDialog);
  ui->openGLWidget->update();
}

void MainWindow::on_edges_color_clicked() {
  ui->openGLWidget->edge_color = QColorDialog::getColor(
      Qt::red, this, tr("colors"), QColorDialog::DontUseNativeDialog);
  ui->openGLWidget->update();
}

void MainWindow::on_central_clicked() { ui->openGLWidget->projection_type = 0; }

void MainWindow::on_parallel_clicked() {
  ui->openGLWidget->projection_type = 1;
}

void MainWindow::widget_state() {
  if (ui->openGLWidget->projection_type == 0) {
    ui->central->setChecked(true);
  } else {
    ui->parallel->setChecked(true);
  }
  ui->edge_type->setCurrentIndex(ui->openGLWidget->index_line);
  ui->vertex_type->setCurrentIndex(ui->openGLWidget->index_vert);
  ui->thickness->setValue(ui->openGLWidget->width_line);
  ui->size_vertex->setValue(ui->openGLWidget->size_point);
  ui->doubleSpinBox_move_x->setValue(ui->openGLWidget->move_x);
  ui->doubleSpinBox_move_y->setValue(ui->openGLWidget->move_y);
  ui->doubleSpinBox_move_z->setValue(ui->openGLWidget->move_z);
  ui->doubleSpinBox_rotate_x->setValue(ui->openGLWidget->rotate_x);
  ui->doubleSpinBox_rotate_y->setValue(ui->openGLWidget->rotate_y);
  ui->doubleSpinBox_rotate_z->setValue(ui->openGLWidget->rotate_z);
  ui->doubleSpinBox_scale->setValue(ui->openGLWidget->scale);
}

void MainWindow::on_screenshot_clicked() {
  QPixmap screenshot = ui->openGLWidget->grab();
  QString filePath = QFileDialog::getSaveFileName(this, "Save Screenshot", "",
                                                  "Images (*.bmp *.jpg)");
  if (!filePath.isEmpty()) {
    screenshot.save(filePath);
  }
}

void MainWindow::on_gif_clicked() {
  QString tmp = QCoreApplication::applicationDirPath();
  tmp.resize(tmp.size() - 38);
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Gif"), tmp + "/images", tr("Gif (*.gif)"));

  if (!fileName.isEmpty()) {
    gif_create(fileName);
  } else {
    QMessageBox::warning(this, "", "Failed to save gif.");
  }
}

void MainWindow::gif_create(QString fileName) {
  QImage img(ui->openGLWidget->size(), QImage::Format_RGB32), img640_480;
  QPainter painter(&img);
  QTime dieTime;
  GifWriter gif;
  QByteArray ba = fileName.toLocal8Bit();
  const char *str = ba.data();
  GifBegin(&gif, str, 640, 480, 10);

  for (int i = 1; i <= 50; ++i) {
    if (i % 10 == 0) ui->gif->setText(QString::number(i / 10) + "s");
    ui->openGLWidget->render(&painter);
    img640_480 = img.scaled(QSize(640, 480));
    GifWriteFrame(&gif, img640_480.bits(), 640, 480, 10);
    dieTime = QTime::currentTime().addMSecs(100);
    while (QTime::currentTime() < dieTime)
      QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
  }
  ui->gif->setText("GIF");
  GifEnd(&gif);
  QMessageBox::information(this, "GIF READY", "GIF saved successfully.");
}
