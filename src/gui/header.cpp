//
// Created by amara on 3/8/2026.
//

// You may need to build the project (run Qt uic code generator) to get
// "ui_Header.h" resolved

#include "../../include/gui/header.hpp"

#include "ui_Header.h"

namespace res {
Header::Header(QWidget *parent) : QWidget(parent), ui(new Ui::Header) {
  ui->setupUi(this);
}

Header::~Header() { delete ui; }
}  // namespace res
