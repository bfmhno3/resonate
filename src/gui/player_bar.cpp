#include "gui/player_bar.hpp"

#include "ui_player_bar.h"

namespace res {
PlayerBar::PlayerBar(QWidget *parent) : QWidget(parent), ui(new Ui::PlayerBar) {
  ui->setupUi(this);
}

PlayerBar::~PlayerBar() { delete ui; }
}  // namespace res
