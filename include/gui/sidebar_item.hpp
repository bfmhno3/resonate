#ifndef RESONATE_SIDEBAR_ITEM_HPP
#define RESONATE_SIDEBAR_ITEM_HPP

#include <QString>
#include <QWidget>

namespace res {
QT_BEGIN_NAMESPACE
namespace Ui {
class SidebarItem;
}
QT_END_NAMESPACE

class SidebarItem : public QWidget {
  Q_OBJECT

 public:
  explicit SidebarItem(QWidget *parent = nullptr);
  explicit SidebarItem(QWidget *parent = nullptr,
                       const QString &icon_path = QString(),
                       const QString &name = QString(),
                       const int count = int{0});
  ~SidebarItem() override;

  void set_item_data(const QString &icon_path = QString(),
                     const QString &name = QString(),
                     const int count = int{0}) const noexcept;
  void set_icon(const QString &icon_path = QString()) const noexcept;
  void set_name(const QString &name = QString()) const noexcept;
  void set_count(int count = int{0}) const noexcept;

 private:
  Ui::SidebarItem *ui;
};
}  // namespace res

#endif  // RESONATE_SIDEBAR_ITEM_HPP
