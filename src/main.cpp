#include "main.hpp"

#include <QApplication>

#include "gui/main_window.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  res::MainWindow window;
  window.show();

  return app.exec();
}
