#ifndef RESONATE_MAIN_WINDOW_HPP
#define RESONATE_MAIN_WINDOW_HPP

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
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

 private:
  Ui::MainWindow *ui;
};
}  // namespace res

#endif  // RESONATE_MAIN_WINDOW_HPP
