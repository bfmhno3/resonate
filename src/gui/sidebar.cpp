#include "gui/sidebar.hpp"

#include <QStringList>
#include <QListWidgetItem>

#include "ui_Sidebar.h"

namespace res {
Sidebar::Sidebar(QWidget *parent) : QWidget(parent), ui(new Ui::Sidebar) {
  ui->setupUi(this);

  // 连接按钮信号
  connect(ui->btn_close, &QPushButton::clicked, this, &Sidebar::handle_close);
  connect(ui->btn_min, &QPushButton::clicked, this, &Sidebar::handle_minimize);
  connect(ui->btn_max, &QPushButton::clicked, this, &Sidebar::handle_maximize);

  QStringList nav_items = {"Recommend", "Likes", "Recent", "Local"};

  for (int i = 0; i < nav_items.size(); ++i) {
    QListWidgetItem* item = new QListWidgetItem(nav_items.at(i), ui->list_navigation);
  }

  // 默认选中第一项
  ui->list_navigation->setCurrentRow(0);
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
