#include "gui/sidebar.hpp"

#include "ui_Sidebar.h"

namespace res {
Sidebar::Sidebar(QWidget *parent) : QWidget(parent), ui(new Ui::Sidebar) {
  ui->setupUi(this);
}

Sidebar::~Sidebar() { delete ui; }
}  // namespace res
