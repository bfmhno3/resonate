#include "gui/header.hpp"

#include <QIcon>
#include <QPixmap>

#include "ui_Header.h"

namespace res {
Header::Header(QWidget *parent) : QWidget(parent), ui(new Ui::Header) {
  ui->setupUi(this);

  auto setup_icon_button = [](QPushButton *button, const char *icon_path,
                              const int icon_size) {
    button->setText("");
    button->setIcon(QIcon(icon_path));
    button->setIconSize(QSize(icon_size, icon_size));
  };

  setup_icon_button(ui->btn_back, ":/assets/icons/header/back.svg", 18);
  setup_icon_button(ui->btn_forward, ":/assets/icons/header/forward.svg", 18);
  setup_icon_button(ui->btn_refresh, ":/assets/icons/header/refresh.svg", 18);
  setup_icon_button(ui->btn_settings, ":/assets/icons/header/settings.svg", 18);

  ui->line_edit_search->addAction(QIcon(":/assets/icons/header/search.svg"),
                                  QLineEdit::LeadingPosition);

  ui->label_avatar->setText("");
  const QPixmap avatar_pixmap(":/assets/icons/header/User.svg");
  ui->label_avatar->setPixmap(
      avatar_pixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  ui->label_avatar->setAlignment(Qt::AlignCenter);
}

Header::~Header() { delete ui; }
}  // namespace res
