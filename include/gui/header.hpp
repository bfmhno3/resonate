#ifndef RESONATE_HEADER_HPP
#define RESONATE_HEADER_HPP

#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class Header;
}
QT_END_NAMESPACE

class Header : public QWidget {
  Q_OBJECT

 public:
  explicit Header(QWidget *parent = nullptr);
  ~Header() override;

 private:
  Ui::Header *ui;
};
}  // namespace res

#endif  // RESONATE_HEADER_HPP
