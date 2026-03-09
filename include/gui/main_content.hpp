#ifndef RESONATE_MAIN_CONTENT_HPP
#define RESONATE_MAIN_CONTENT_HPP

#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class MainContent;
}
QT_END_NAMESPACE

class MainContent : public QWidget {
  Q_OBJECT

 public:
  explicit MainContent(QWidget *parent = nullptr);
  ~MainContent() override;

 public slots:
  void set_current_page(int index);

 private:
  Ui::MainContent *ui;
};
}  // namespace res

#endif  // RESONATE_MAIN_CONTENT_HPP
