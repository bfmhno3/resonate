#ifndef RESONATE_PLAYER_BAR_HPP
#define RESONATE_PLAYER_BAR_HPP

#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class PlayerBar;
}
QT_END_NAMESPACE

class PlayerBar : public QWidget {
  Q_OBJECT

 public:
  explicit PlayerBar(QWidget *parent = nullptr);
  ~PlayerBar() override;

 private:
  Ui::PlayerBar *ui;
};
}  // namespace res

#endif  // RESONATE_PLAYER_BAR_HPP
