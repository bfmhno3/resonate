//
// Created by amara on 3/4/2026.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_main_window.h" resolved

#include "../../include/gui/main_window.hpp"

#include "ui_main_window.h"

namespace res {
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }
}  // namespace res
