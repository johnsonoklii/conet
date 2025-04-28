#include "conet/net/EventLoop.h"
#include "conet/base/coroutine/coroutine.h"
#include "conet/base/log/logger.h"
#include "conet/base/util/timestamp.h"

#include <iostream>

void test1() {
    conet::Coroutine::getMainCoroutine();
    conet::net::EventLoop loop;
    
    loop.runAfter(1000, []() {
        LOG_INFO("runAfter: 1s.");
    });

    loop.runAfter(3000, []() {
        LOG_INFO("runAfter: 3s.");
    });


    loop.runAfter(5000, []() {
        LOG_INFO("runAfter: 5s.");
    });

    loop.loop();
}

void test2() {
    conet::Coroutine::getMainCoroutine();
    conet::net::EventLoop loop;
    
    loop.runEvery(1000, []() {
        LOG_INFO("runEvery: 1s.");
    });

    loop.runAfter(3000, []() {
        LOG_INFO("runAfter: 3s.");
    });

    loop.loop();
}

void test3() {
    conet::Coroutine::getMainCoroutine();
    conet::net::EventLoop loop;
    
    loop.runEvery(1000, []() {
        LOG_INFO("runEvery: 1s.");
    });

    loop.runAfter(3000, []() {
        LOG_INFO("runAfter: 3s.");
    });

    loop.runAt(conet::Timestamp::addTimeSecond(conet::Timestamp::now(), 5), []() {
        LOG_INFO("runAt: 5s.");
    });

    loop.loop();
}

int main() {
    test3();
}