#include "conet/base/log/logger.h"

using namespace conet::log;
// using namespace conet;

void func1() {
    GLOB_LOG_OPTION.setLevel(LogLevel::DEBUG);
    GLOB_LOG_OPTION.setFileDir("/log/conet");
    GLOB_LOG_OPTION.setFileName("test");
    
    LOG_INFO("hello world %d", 1);
    LOG_INFO("hello world %d", 2);
}

void func2() {
    Logger logger(GLOB_LOG_OPTION);
    log_debug(logger, "hello world5");
}

void func3() {
    LogOption option;
    option.setLevel(LogLevel::DEBUG);
    option.setFileDir("/log/conet");
    option.setFileName("test");

    Logger logger(option);
    log_debug(logger, "hello %s", "debug");
    log_info(logger, "hello %s", "info");
    log_warn(logger, "hello %s", "warn");
    log_error(logger, "hello %s", "error");
    log_fatal(logger, "hello %s", "fatal");
}

int main() {
    func3();
}