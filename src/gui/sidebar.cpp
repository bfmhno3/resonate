#include "gui/sidebar.hpp"

#include "ui_Sidebar.h"

namespace res {
Sidebar::Sidebar(QWidget *parent) : QWidget(parent), ui(new Ui::Sidebar) {
  ui->setupUi(this);

  // 连接按钮信号
  connect(ui->btn_close, &QPushButton::clicked, this, &Sidebar::handle_close);
  connect(ui->btn_min, &QPushButton::clicked, this, &Sidebar::handle_minimize);
  connect(ui->btn_max, &QPushButton::clicked, this, &Sidebar::handle_maximize);
}

Sidebar::~Sidebar() { delete ui; }

void Sidebar::handle_minimize() {
  emit minimize_requested();
}

void Sidebar::handle_maximize() {
  emit maximize_requested();
}

void Sidebar::handle_close() {
  emit close_requested();
}

}  // namespace res
