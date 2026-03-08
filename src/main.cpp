#include "main.hpp"

#include <QApplication>
#include <QGuiApplication>

#include "gui/main_window.hpp"
#include "utils/logger.hpp"

int main(int argc, char* argv[]) {
  // 启用高 DPI 设置
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  // 使用高 DPI 图标/像素图
  QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  QApplication app(argc, argv);

  // 绑定日志处理函数，确保所有 Qt 输出都能被 Logger 捕获
  qInstallMessageHandler(res::Logger::messageHandler);

  res::MainWindow window;
  window.show();

  return app.exec();
}
