#include "gui/main_window.hpp"

#include <QMouseEvent>
#include <QWidget>
#include <QWindow>

#include "ui_main_window.h"

namespace res {
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // 移除窗口边框和标题栏
  setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  // 仅作为回退方案。正常情况下 startSystemMove 运行期间不会进入此函数
  // 只有在 startSystemMove 未被调用或失败时才实行手动移动
  if (event->buttons() & Qt::LeftButton) {
    move(event->globalPos() - drag_pos_);
  }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    // 委托给厂做系统处理窗口移动
    // 能完美解决多显示器 DPI 不一致时的跳动和报错问题
    // 并且支持 Windows 的 Snap（如拖动到屏幕边缘自动半屏）
    if (windowHandle()) {
      windowHandle()->startSystemMove();
    }

    drag_pos_ = event->globalPos() - frameGeometry().topLeft();
  }
}

}  // namespace res
