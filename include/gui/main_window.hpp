#ifndef RESONATE_MAIN_WINDOW_HPP
#define RESONATE_MAIN_WINDOW_HPP

#include <QMouseEvent>
#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget {
  Q_OBJECT

 public:
  static constexpr int kCornerRadius = 10;  // Big Sur 圆角半径
  static constexpr int kShadowWidth = 20;

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

 private:
  Ui::MainWindow* ui;
};
}  // namespace res

#endif  // RESONATE_MAIN_WINDOW_HPP
