#include "gui/clickable_slider.hpp"

#include <QMouseEvent>
#include <QSlider>
#include <QStyle>

namespace res {
ClickableSlider::ClickableSlider(QWidget *parent) : QSlider(parent) {
  setFocusPolicy(Qt::StrongFocus);
}

void ClickableSlider::mousePressEvent(QMouseEvent *event) {
  // 仅响应左键点击
  if (event->button() == Qt::LeftButton) {
    int value;
    // 根据水平会垂直方强计算位置对应的数值
    if (orientation() == Qt::Horizontal) {
      value = QStyle::sliderValueFromPosition(minimum(), maximum(),
                                              event->pos().x(), width());
    } else {
      // 注意垂直方向在 Qt 中坐标默认是反的（上小下大），需要处理一下
      value = QStyle::sliderValueFromPosition(minimum(), maximum(),
                                              event->pos().y(), height(), true);
    }

    setValue(value);

    // 发送 slider_moved 信号，以便在外部捕获并处理
    emit slider_moved(value);
  }

  // 把事件传递给父类，保证按住滑块拖动的默认行为不受影响
  QSlider::mousePressEvent(event);
}

}  // namespace res
