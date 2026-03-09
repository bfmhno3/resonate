#include "gui/main_window.hpp"

#include <QColor>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#include <windowsx.h>
#endif

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
  connect(ui->sidebar, &res::Sidebar::close_requested, this,
          &MainWindow::close);
  connect(ui->sidebar, &res::Sidebar::minimize_requested, this,
          &MainWindow::showMinimized);
  connect(ui->sidebar, &res::Sidebar:: maximize_requested, this,
          [this]() { isMaximized() ? showNormal() : showMaximized(); });

  connect(ui->sidebar, &res::Sidebar::page_changed, ui->main_content,
          &res::MainContent::set_current_page);
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

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::WindowStateChange) {
    if (isMaximized()) {
      if (layout()) {
        layout()->setContentsMargins(0, 0, 0, 0);  // 最大化时去除阴影
      }

      if (ui->content->graphicsEffect()) {
        ui->content->graphicsEffect()->setEnabled(false);  // 禁用阴影效果
      }
    } else {
      if (layout()) {
        layout()->setContentsMargins(kShadowWidth, kShadowWidth, kShadowWidth,
                                     kShadowWidth);  // 恢复阴影
      }

      if (ui->content->graphicsEffect()) {
        ui->content->graphicsEffect()->setEnabled(true);  // 开启阴影效果
      }
    }
  }

  QWidget::changeEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray &event_type, void *message, long *result) {
#ifdef Q_OS_WIN
  if (event_type == "windows_generic_MSG") {
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_NCHITTEST) {
      // 获取鼠标坐标
      int x = GET_X_LPARAM(msg->lParam);
      int y = GET_Y_LPARAM(msg->lParam);
      QPoint pt = mapFromGlobal(QPoint(x, y));

      // 定义边距距离
      constexpr int border_width = 5;

      // 水平测试
      bool left = pt.x() < border_width;
      bool right = pt.x() >= width() - border_width;

      // 垂直测试
      bool top = pt.y() < border_width;
      bool bottom = pt.y() >= height() - border_width;

      if (top && left) {
        *result = HTTOPLEFT;
        return true;
      }
      if (top && right) {
        *result = HTTOPRIGHT;
        return true;
      }
      if (bottom && left) {
        *result = HTBOTTOMLEFT;
        return true;
      }
      if (bottom && right) {
        *result = HTBOTTOMRIGHT;
        return true;
      }
      if (left) {
        *result = HTLEFT;
        return true;
      }
      if (right) {
        *result = HTRIGHT;
        return true;
      }
      if (top) {
        *result = HTTOP;
        return true;
      }
      if (bottom) {
        *result = HTBOTTOM;
        return true;
      }

      // If we are not on a border, we might want to return HTCAPTION to allow dragging
      // via the title bar area, or HTCLIENT for the rest.
      // Since you handle dragging via mousePressEvent separately, you can just fall through
      // or return HTCLIENT here. However, returning HTCAPTION here for the title bar area
      // is often smoother than manual startSystemMove calls.
    }
  }
#endif
  return QWidget::nativeEvent(event_type, message, result);
}

}  // namespace res
