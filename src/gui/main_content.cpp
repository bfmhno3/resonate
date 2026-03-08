#include "gui/main_content.hpp"

#include "ui_main_content.h"

namespace res {
MainContent::MainContent(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainContent) {
  ui->setupUi(this);
}

MainContent::~MainContent() { delete ui; }
}  // namespace res
