#ifndef RESONATE_CLICKABLE_SLIDER_HPP
#define RESONATE_CLICKABLE_SLIDER_HPP

#include <QMouseEvent>
#include <QSlider>

namespace res {
class ClickableSlider : public QSlider {
  Q_OBJECT

 public:
  explicit ClickableSlider(QWidget *parent = nullptr);

 signals:
  void slider_moved(int value);

 protected:
  void mousePressEvent(QMouseEvent *event) override;
};
}  // namespace res

#endif  // RESONATE_CLICKABLE_SLIDER_HPP
