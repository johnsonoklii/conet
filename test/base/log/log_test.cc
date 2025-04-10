#define CONET_DEBUG
#include "conet/base/log/logger.h"

using namespace conet::log;
// using namespace conet;

void func1() {
    GLOB_LOG_OPTION.setLevel(conet::log::DEBUG);
    GLOB_LOG_OPTION.setFileDir("/log/conet");
    GLOB_LOG_OPTION.setFileName("test");
    
    LOG_INFO("hello world1");
    LOG_INFO("hello world2");
}

void func2() {
    conet::log::Logger logger(GLOB_LOG_OPTION);
    LOG_DEBUG("hello world5");
}

int main() {
    func2();
}