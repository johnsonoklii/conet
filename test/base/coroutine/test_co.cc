#include "conet/base/coroutine/coroutine.h"

#include <stdio.h>

void func1() {
    printf("func1 start...\n");
    conet::Coroutine::yield();
    printf("func1 end...\n");
}

int main() {
    // 创建主协程
    conet::Coroutine::getMainCoroutine();
    conet::Coroutine co(8192, func1); 

    printf("main start...\n");
    conet::Coroutine::resume(&co);

    printf("main end...\n");
    conet::Coroutine::resume(&co);

    return 0;
}