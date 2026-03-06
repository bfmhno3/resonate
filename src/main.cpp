#include "main.hpp"

#include <QApplication>
#include <QGuiApplication>

#include "gui/main_window.hpp"

int main(int argc, char* argv[]) {
  // 启用高 DPI 设置
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  // 使用高 DPI 图标/像素图
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  QApplication app(argc, argv);

  res::MainWindow window;
  window.show();

  return app.exec();
}
