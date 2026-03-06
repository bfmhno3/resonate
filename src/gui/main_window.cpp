#include "gui/main_window.hpp"

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
}  // namespace res
