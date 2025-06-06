#include "Logger.h"
#include <thread>

// Example
int main() {
    Logger::getInstance().start("logs", true);  // para1. 需要存放的日志所在目录名  para2. 控制台同步打印

    Logger::getInstance().log("Application started.");
    Logger::getInstance().log("This is a warning!", LogLevel::WARN);
    Logger::getInstance().log("Something went wrong!", LogLevel::ERROR);

    std::thread t1([]() {
        for (int i = 0; i < 10; ++i) {
            Logger::getInstance().log("Thread 1 logging " + std::to_string(i));
        }
    });

    std::thread t2([]() {
        for (int i = 0; i < 10; ++i) {
            Logger::getInstance().log("Thread 2 logging " + std::to_string(i));
        }
    });

    t1.join();
    t2.join();

    Logger::getInstance().stop();
    return 0;
}
