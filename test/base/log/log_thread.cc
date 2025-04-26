#include "conet/base/log/logger.h"

#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std::chrono;
void worker1() {
    int num = 200000;
    for (int i = 0; i < num; ++i) {
        LOG_INFO("worker-1 asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL-%d", i);
    }
}

void worker2() {
    int num = 200000;
    for (int i = 0; i < num; ++i) {
        LOG_INFO("worker-2 asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL-%d", i);
    }
}

void worker3() {
    int num = 200000;
    for (int i = 0; i < num; ++i) {
        LOG_INFO("worker-3 asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL-%d", i);
    }
}

void worker4() {
    int num = 200000;
    for (int i = 0; i < num; ++i) {
        LOG_INFO("worker-4 asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL-%d", i);
    }
}

void worker5() {
    int num = 200000;
    for (int i = 0; i < num; ++i) {
        LOG_INFO("worker-5 asdasdasfadsffadgdfjjdjlaljfsdhkagnfgjaflhgjdjagnlfpufdjhfkJFKL-%d", i);
    }
}


int main() {
    GLOB_LOG_OPTION.setFileDir("./bench");
    GLOB_LOG_OPTION.setFileName("bench");
    GLOB_LOG_OPTION.setConsole(false);
    GLOB_LOG_OPTION.setRollSize(1024 * 1024 * 64);

    auto start = high_resolution_clock::now();

    std::vector<std::thread> workers;
    workers.emplace_back(worker1);
    workers.emplace_back(worker2);
    workers.emplace_back(worker3);
    workers.emplace_back(worker4);
    workers.emplace_back(worker5);
    for (auto& worker : workers) {
        worker.join();
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "程序运行时间: " << duration.count() << " 毫秒" << std::endl;
}