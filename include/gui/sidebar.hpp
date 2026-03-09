#ifndef RESONATE_SIDEBAR_HPP
#define RESONATE_SIDEBAR_HPP

#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class Sidebar;
}
QT_END_NAMESPACE

class Sidebar : public QWidget {
  Q_OBJECT

 public:
  explicit Sidebar(QWidget *parent = nullptr);
  ~Sidebar() override;

 signals:
  void minimize_requested();
  void maximize_requested();
  void close_requested();
  void page_changed(int index);

 private slots:
  void handle_minimize();
  void handle_maximize();
  void handle_close();

 private:
  Ui::Sidebar *ui;
};
}  // namespace res

#endif  // RESONATE_SIDEBAR_HPP
