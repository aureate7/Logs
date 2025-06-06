#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <filesystem>

 // Implementation

Logger::Logger() : running(false), consoleOutputEnabled(true) {}

Logger::~Logger() {
    stop();
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::start(const std::string& logDir, bool consoleOutput) {
    logDirectory = logDir;
    std::filesystem::create_directories(logDirectory);

    consoleOutputEnabled = consoleOutput;

    running = true;
    workerThread = std::thread(&Logger::threadFunc, this);
}

void Logger::stop() {
    if (running) {
        running = false;
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
        if (logFile.is_open()) {
            logFile.close();
        }
    }
}

void Logger::log(const std::string& message, LogLevel level) {
    std::ostringstream oss;
    oss << "[" << getCurrentTime() << "] "
        << "[" << logLevelToString(level) << "] "
        << message;

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (logQueue.size() < MAX_QUEUE_SIZE) {
            logQueue.push({oss.str()});
        } else {
            // 丢弃，防止内存撑爆，实际可考虑打报警日志
            std::cerr << "Logger queue full! Dropping log message." << std::endl;
        }
    }
    cv.notify_one();
}

void Logger::threadFunc() {
    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this]() { return !logQueue.empty() || !running; });

        while (!logQueue.empty()) {
            rollFileIfNeeded();

            LogItem item = logQueue.front();
            logQueue.pop();

            lock.unlock();

            if (logFile.is_open()) {
                logFile << item.text << std::endl;
            }

            if (consoleOutputEnabled) {
                std::cout << item.text << std::endl;
            }

            lock.lock();
        }
    }

    // 清理剩余日志
    while (!logQueue.empty()) {
        rollFileIfNeeded();
        LogItem item = logQueue.front();
        logQueue.pop();
        if (logFile.is_open()) {
            logFile << item.text << std::endl;
        }
        if (consoleOutputEnabled) {
            std::cout << item.text << std::endl;
        }
    }
}

std::string Logger::getCurrentDate() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::rollFileIfNeeded() {
    std::string today = getCurrentDate();
    if (today != currentDate) {
        currentDate = today;
        if (logFile.is_open()) {
            logFile.close();
        }
        std::ostringstream filename;
        filename << logDirectory << "/log_" << currentDate << ".log";
        logFile.open(filename.str(), std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << filename.str() << std::endl;
        }
    }
}
