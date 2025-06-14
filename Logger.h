#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

/**
 * 使用方法：
 * 
 * // 初始化（程序启动时调用一次）
 * Logger::getInstance().start("logs", true);  // para1. 需要存放的日志所在目录名  para2. 控制台同步打印
 * 
 * // 记录日志
 * Logger::getInstance().log("Application started.");
 * Logger::getInstance().log("This is a warning!", LogLevel::WARN);
 * Logger::getInstance().log("Something went wrong!", LogLevel::ERROR);
 * 
 * // 支持多线程调用 log，线程安全
 * 
 * // 程序退出前关闭日志系统
 * Logger::getInstance().stop();
 * 
 * 说明：
 * - 日志文件按天分文件 logs/log_YYYY-MM-DD.log
 * - 日志等级保留 INFO/WARN/ERROR，但当前版本全部记录（方便排查）
 * - 控制台同步打印可配置
 */

enum class LogLevel { INFO, WARN, ERROR };

class Logger {
public:
    static Logger& getInstance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void start(const std::string& logDir, bool consoleOutput = true);
    void stop();

    void log(const std::string& message, LogLevel level = LogLevel::INFO);
    void log(const std::string& message, const std::string& tag = "INFO");

private:
    Logger();
    ~Logger();

    void threadFunc();

    std::string getCurrentDate();
    std::string getCurrentTime();
    std::string logLevelToString(LogLevel level);
    void rollFileIfNeeded();

    // 日志项结构
    struct LogItem {
        std::string text;
    };

    // 有界队列大小（防止 OOM）
    static constexpr size_t MAX_QUEUE_SIZE = 100000;

    std::ofstream logFile;
    std::string currentDate;
    std::string logDirectory;

    std::mutex queueMutex;
    std::condition_variable cv;
    std::queue<LogItem> logQueue;

    std::thread workerThread;
    std::atomic<bool> running;

    bool consoleOutputEnabled;
};
