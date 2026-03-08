#ifndef RESONATE_LOGGER_H
#define RESONATE_LOGGER_H

#include <QFile>
#include <QMutex>
#include <QString>
#include <QTextStream>

namespace res {

class Logger {
 public:
  static Logger &instance();
  // 金泰回调函数，用于绑定到 qInstallMessageHandler
  static void messageHandler(QtMsgType type, const QMessageLogContext &context,
                             const QString &msg);

  // 禁用拷贝构造和赋值操作，确保单例的绝对唯一性
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

 private:
  // 私有构造和析构，防止外部实例化
  Logger();
  ~Logger();

  // 实际执行写入的内部方法
  void write(QtMsgType type, const QMessageLogContext &context,
             const QString &msg);

 private:
  QFile file_;
  QTextStream text_stream_;
  QMutex mutex_;
};

}  // namespace res

#endif  // RESONATE_LOGGER_H
