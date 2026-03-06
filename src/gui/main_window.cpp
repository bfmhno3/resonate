#include "gui/main_window.hpp"

#include <QMouseEvent>
#include <QWidget>

#include "ui_main_window.h"

namespace res {
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // 移除窗口边框和标题栏
  setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    move(event->globalPos() - drag_pos_);
  }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    drag_pos_ = event->globalPos() - frameGeometry().topLeft();
  }
}

}  // namespace res
