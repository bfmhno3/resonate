#include "utils/logger.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

namespace res {
// Meyers' Singleton 实现
Logger &Logger::instance() {
  static Logger inst;
  return inst;
}

// 桥接函数：将 Qt 的回调转发给单例的实例方法
void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context,
                            const QString &msg) {
  Logger::instance().write(type, context, msg);
}

Logger::Logger() {
  // 初始化日志目录
  const QString log_dir_path = QCoreApplication::applicationDirPath() + "/logs";
  if (const QDir dir; !dir.exists(log_dir_path)) {
    (void)dir.mkpath(log_dir_path);
  }

  // 初始化日志文件名
  QString file_name =
      QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".log";
  QString file_path = log_dir_path + "/" + file_name;

  // 打开文件并绑定文本流
  file_.setFileName(file_path);
  // 使用 Text 标志会自动处理平台换行符（Windows 下的 \r\n）
  if (file_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    text_stream_.setDevice(&file_);
  }
}

Logger::~Logger() {
  if (file_.isOpen()) {
    file_.close();
  }
}

void Logger::write(QtMsgType type, const QMessageLogContext &context,
                   const QString &msg) {
  QMutexLocker locker(&mutex_);

  if (!file_.isOpen()) {
    return;
  }

  QString current_time =
      QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
  QString msg_type;

  switch (type) {
    case QtDebugMsg:
      msg_type = "[DEBUG]";
      break;
    case QtInfoMsg:
      msg_type = "[INFO]";
      break;
    case QtWarningMsg:
      msg_type = "[WARNING]";
      break;
    case QtCriticalMsg:
      msg_type = "[ERROR]";
      break;
    case QtFatalMsg:
      msg_type = "[FATAL]";
      break;
  }

  // 格式化输出内容，包含文件名和行号
  // 仅在 Debug 模式下 context 会包含有效文件信息
  QString file_info;
  if (context.file) {
    // 提取文件名，去掉冗长的绝对路径
    QString file_path = QString(context.file);
    file_info = QString("(%1:%2)")
                    .arg(file_path.section('\\', -1).section('/', -1))
                    .arg(context.line);
  }

  QString log_message =
      QString("[%1] %2 %3 %4").arg(current_time, msg_type, msg, file_info);

  // 输出到控制台
  fprintf(stderr, "%s\n", log_message.toLocal8Bit().constData());

  text_stream_ << log_message << "\n";
  text_stream_.flush();  // 及时刷新缓冲区，防止程序崩溃时丢失最新日志
}
}  // namespace res