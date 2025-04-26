#include <cstring>
#include <chrono>

#include <iostream>

#include "conet/base/log/logger.h"

using namespace std::chrono;

int main() {
    int num = 1000000;

    GLOB_LOG_OPTION.setFileDir("./bench");
    GLOB_LOG_OPTION.setFileName("bench");
    GLOB_LOG_OPTION.setConsole(false);
    GLOB_LOG_OPTION.setRollSize(1024 * 1024 * 64);

    auto start = high_resolution_clock::now();

    conet::log::Logger logger = conet::log::Logger::getInstance();

    for (int i = 0; i < num; ++i) {
        LOG_INFO("asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL%d", i);
    }

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start);

    std::cout << "程序运行时间: " << duration.count() << " 毫秒" << std::endl;

    return 0;
}