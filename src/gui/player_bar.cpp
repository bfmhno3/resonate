#include "gui/player_bar.hpp"

#include <QIcon>

#include "ui_player_bar.h"

namespace res {
PlayerBar::PlayerBar(QWidget *parent) : QWidget(parent), ui(new Ui::PlayerBar) {
  ui->setupUi(this);

  auto setup_icon_button = [](QPushButton *button, const char *icon_path,
                              int icon_size) {
    button->setText("");
    button->setStyleSheet("QPushButton { border: none; background: transparent; }");
    button->setIcon(QIcon(icon_path));
    button->setIconSize(QSize(icon_size, icon_size));
  };

  setup_icon_button(ui->btn_like, ":/assets/icons/player/like.svg", 18);
  setup_icon_button(ui->btn_play_mode, ":/assets/icons/player/repeat.svg", 20);
  setup_icon_button(ui->btn_prev, ":/assets/icons/player/previous.svg", 20);
  setup_icon_button(ui->btn_play_pause, ":/assets/icons/player/play.svg", 24);
  setup_icon_button(ui->btn_next, ":/assets/icons/player/next.svg", 20);
  setup_icon_button(ui->btn_volume, ":/assets/icons/player/volume_high.svg", 20);
  setup_icon_button(ui->btn_device, ":/assets/icons/player/device.svg", 20);
  setup_icon_button(ui->btn_desktop_lyric,
                    ":/assets/icons/player/desktop_lyric.svg", 20);
  setup_icon_button(ui->btn_playlist, ":/assets/icons/player/playlist.svg", 20);
}

PlayerBar::~PlayerBar() { delete ui; }
}  // namespace res
