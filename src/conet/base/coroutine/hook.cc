#include "conet/base/coroutine/hook.h"
#include "conet/base/coroutine/coroutine.h"
#include "conet/net/EventLoop.h"
#include "conet/net/Channel.h"
#include "conet/base/log/logger.h"

#include <dlfcn.h>

using namespace conet::log;
using namespace conet::net;

#define HOOK_SYS_FUNC(name) name##_fun_ptr_t g_sys_##name##_fun = (name##_fun_ptr_t)dlsym(RTLD_NEXT, #name);

HOOK_SYS_FUNC(accept);
HOOK_SYS_FUNC(readv);

namespace conet {

static bool g_hook = true;

void setHook() {
    g_hook = true;
}

int accept_hook(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    LOG_DEBUG("accept_hook(): this is accept hook.")
    if (Coroutine::isMainCoroutine()) {
        LOG_DEBUG("accept_hook(): accept_hook is main coroutine.")
        return g_sys_accept_fun(sockfd, addr, addrlen);
    }

    int ret = g_sys_accept_fun(sockfd, addr, addrlen);
    if (ret > 0) {
        return ret;
    }

    // 1. 获取当前线程的loop
    EventLoop* loop = EventLoop::getEventLoop();
    // 2. 获取channel
    Channel* channel = ChannelManager::getInstance().getChannel(sockfd);
    // 3. 当前协程
    Coroutine::sptr co = Coroutine::getCurrentCoroutine();
    channel->setCoroutine(co);  // COMMENT: coroutine的生命周期由channel管理
    // 4. 注册可读事件
    channel->enableRead();
    loop->updateChannel(channel);

    LOG_DEBUG("accept_hook(): yield.");
    Coroutine::yield();

    return g_sys_accept_fun(sockfd, addr, addrlen);
}

size_t readv_hook(int sockfd, const struct iovec *iovec, int count) {
    LOG_DEBUG("readv_hook(): this is reav hook.")
    if (Coroutine::isMainCoroutine()) {
        LOG_DEBUG("readv_hook(): readv_hook is main coroutine.")
        return g_sys_readv_fun(sockfd, iovec, count);
    }

    int ret = g_sys_readv_fun(sockfd, iovec, count);
    if (ret > 0) {
        return ret;
    }

    // 1. 获取当前线程的loop
    EventLoop* loop = EventLoop::getEventLoop();
    // 2. 获取channel
    Channel* channel = ChannelManager::getInstance().getChannel(sockfd);
    // 3. 当前协程
    Coroutine::sptr co = Coroutine::getCurrentCoroutine();
    channel->setCoroutine(co);
    // 4. 注册可读事件
    channel->enableRead();
    loop->updateChannel(channel);

    LOG_DEBUG("readv_hook(): yield.");
    Coroutine::yield();

    return g_sys_readv_fun(sockfd, iovec, count);
}

extern "C" {
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if (!conet::g_hook) {
        return g_sys_accept_fun(sockfd, addr, addrlen);
    } else {
        return conet::accept_hook(sockfd, addr, addrlen);
    }
}

size_t readv(int sockfd, const struct iovec *iovec, int count) {
    if (!conet::g_hook) {
        return g_sys_readv_fun(sockfd, iovec, count);
    } else {
        return conet::readv_hook(sockfd, iovec, count);
    }
}
} // extern "C"

} // namespace conet