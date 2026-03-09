#include "gui/main_content.hpp"

#include "ui_main_content.h"

namespace res {
MainContent::MainContent(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainContent) {
  ui->setupUi(this);
}

MainContent::~MainContent() { delete ui; }

void MainContent::set_current_page(int index) {
  ui->stacked_widget_main->setCurrentIndex(index);
}

}  // namespace res
