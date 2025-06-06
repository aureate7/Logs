# 使用方法
## 初始化（程序启动时调用一次）
``` c++
Logger::getInstance().start("logs", true);  // 日志目录 + 是否控制台打印
```
## 记录日志
``` c++
Logger::getInstance().log("Application started.", LogLevel::INFO); // 默认INFO级别，此处参数可不写
Logger::getInstance().log("This is a warning!", LogLevel::WARN);
Logger::getInstance().log("Something went wrong!", LogLevel::ERROR);
```
Tips: 支持多线程调用 log，线程安全
## 程序退出前关闭日志系统
``` c++
Logger::getInstance().stop();
```
## 说明
- 日志文件按天分文件 logs/log_YYYY-MM-DD.log
- 日志等级保留 INFO/WARN/ERROR，但当前版本全部记录（方便排查）
- 控制台同步打印可配置
 
## 快速启动
``` bash
g++ -std=c++17 -o app main.cpp Logger.cpp -lpthread
./app
```

# Usage
## Initialization（called once when the program starts）
``` c++
Logger::getInstance().start("logs", true);  // file directory of logs + whether to print on the console
```
## Record logs
``` c++
Logger::getInstance().log("Application started.", LogLevel::INFO); // default level is INFO, this parameter can be ignored
Logger::getInstance().log("This is a warning!", LogLevel::WARN);
Logger::getInstance().log("Something went wrong!", LogLevel::ERROR);
```
Tips: Support multi-threading call logs，thread safety
## Close the Log System before the program exits
``` c++
Logger::getInstance().stop();
```
## Function Description
- Log files by day: logs/log_YYYY-MM-DD.log
- The log level retains INFO/Warning/Error, all current versions are recorded for easy troubleshooting
- Console synchronous printing configurable
## QuickStart
``` bash
g++ -std=c++17 -o app main.cpp Logger.cpp -lpthread
./app
```