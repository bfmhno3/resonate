#include "gui/main_window.hpp"

#include <QColor>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QWindow>

#include "ui_main_window.h"

namespace res {
MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // 移除窗口边框和标题栏
  setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

  // 背景透明，让圆角区域真正镂空
  setAttribute(Qt::WA_TranslucentBackground);

  // 为内容容器添加阴影
  auto *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(kShadowWidth);
  shadow->setColor(QColor(0, 0, 0, 255));
  shadow->setOffset(0, 0);  // 居中阴影
  ui->content->setGraphicsEffect(shadow);

  // 连接 Sidebar 的信号
  connect(ui->sidebar, &res::Sidebar::close_requested, this, &MainWindow::close);
  connect(ui->sidebar, &res::Sidebar::minimize_requested, this, &MainWindow::showMinimized);
  connect(ui->sidebar, &res::Sidebar::maximize_requested, this, [this]() {
    isMaximized() ? showNormal() : showMaximized();
  });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  // 仅作为回退方案。正常情况下 startSystemMove 运行期间不会进入此函数
  // 只有在 startSystemMove 未被调用或失败时才实行手动移动
  // if (event->buttons() & Qt::LeftButton) {
  //   qDebug() << "Manual move triggered at position: " << event->globalPos();
  //   move(event->globalPos() - drag_pos_);
  // }

  // 弃用 mouseMoveEvent 的手动移动逻辑，完全依赖 startSystemMove 来处理窗口移动
  // startSystemMove 处理结束后会再次调用 mouseMoveEvent
  // 会导致再在不同缩放级别的显示器上移动窗口时出现跳动的问题
  Q_UNUSED(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    // 委托给厂做系统处理窗口移动
    // 能完美解决多显示器 DPI 不一致时的跳动和报错问题
    // 并且支持 Windows 的 Snap（如拖动到屏幕边缘自动半屏）
    if (windowHandle()) {
      windowHandle()->startSystemMove();
    }
  }
}

}  // namespace res
