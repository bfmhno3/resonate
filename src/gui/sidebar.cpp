#include "gui/sidebar.hpp"

#include <QIcon>
#include <QListWidgetItem>
#include <QStringList>

#include "gui/sidebar_item.hpp"
#include "ui_Sidebar.h"

namespace res {
Sidebar::Sidebar(QWidget* parent) : QWidget(parent), ui(new Ui::Sidebar) {
  ui->setupUi(this);

  ui->label_logo->setText(QString());
  ui->label_logo->setStyleSheet("background-color: transparent;");
  ui->label_logo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  ui->label_logo->setPixmap(QIcon(":/assets/icons/logo.svg").pixmap(120, 28));

  // 连接按钮信号
  connect(ui->btn_close, &QPushButton::clicked, this, &Sidebar::handle_close);
  connect(ui->btn_min, &QPushButton::clicked, this, &Sidebar::handle_minimize);
  connect(ui->btn_max, &QPushButton::clicked, this, &Sidebar::handle_maximize);

  connect(ui->list_navigation, &QListWidget::currentRowChanged,
          [this](int index) { emit page_changed(index); });

  QStringList nav_items = {"Recommend", "Likes", "Recent", "Local"};
  QStringList icon_paths = {
    ":/assets/icons/sidebar/recommend.svg",
    ":/assets/icons/sidebar/likes.svg",
    ":/assets/icons/sidebar/recent.svg",
    ":/assets/icons/sidebar/local.svg",
  };

  for (int i = 0; i < nav_items.size(); ++i) {
    QListWidgetItem* item = new QListWidgetItem(ui->list_navigation);
    res::SidebarItem* custom_item = new res::SidebarItem(
        ui->list_navigation, QString(icon_paths[i]), nav_items[i], 100 * i);
    ui->list_navigation->setItemWidget(item, custom_item);
    item->setSizeHint(QSize(ui->list_navigation->width(), custom_item->sizeHint().height()));
  }

  // 默认选中第一项
  ui->list_navigation->setCurrentRow(0);
}

Sidebar::~Sidebar() { delete ui; }

void Sidebar::handle_minimize() { emit minimize_requested(); }

void Sidebar::handle_maximize() { emit maximize_requested(); }

void Sidebar::handle_close() { emit close_requested(); }

}  // namespace res
