#include "gui/sidebar_item.hpp"

#include <QPixmap>
#include <QString>

#include "ui_sidebar_item.h"

namespace res {
SidebarItem::SidebarItem(QWidget *parent)
    : QWidget(parent), ui(new Ui::SidebarItem) {
  ui->setupUi(this);
  setAttribute(Qt::WA_TranslucentBackground);
}

SidebarItem::SidebarItem(QWidget *parent, const QString &icon_path,
                         const QString &name, const int count)
    : QWidget(parent), ui(new Ui::SidebarItem) {
  ui->setupUi(this);
  setAttribute(Qt::WA_TranslucentBackground);

  set_item_data(icon_path, name, count);
}

SidebarItem::~SidebarItem() { delete ui; }

void SidebarItem::set_item_data(const QString &icon_path, const QString &name,
                                const int count) const noexcept {
  set_icon(icon_path);
  set_name(name);
  set_count(count);
}

void SidebarItem::set_icon(const QString &icon_path) const noexcept {
  if (!icon_path.isEmpty()) {
    QPixmap pixmap(icon_path);
    ui->label_icon->setPixmap(
        pixmap.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  } else {
    ui->label_icon->clear();
  }
}

void SidebarItem::set_name(const QString &name) const noexcept {
  ui->label_name->setText(name);
}

void SidebarItem::set_count(const int count) const noexcept {
  ui->label_count->setText(QString::number(count));
}

}  // namespace res
