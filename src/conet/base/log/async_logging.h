#ifndef CONET_ASYNC_LOGGING_H
#define CONET_ASYNC_LOGGING_H

#include "conet/base/log/buffer.h"
#include "conet/base/log/log_context.h"
#include "conet/base/util/thread.h"
#include "conet/base/util/nocopyable.h"

#include <unistd.h>

#include <functional>
#include <string>
#include <vector>

namespace conet {
namespace log {

class AsyncLogging : public nocopyable {
public:
    using FwriteCallback = std::function<void(const char*, size_t)>;
    using FlushCallback = std::function<void()>;

    AsyncLogging(FwriteCallback&& cb, int flush_interval);
    ~AsyncLogging();

    void setFwriteCallback(FwriteCallback&& cb) { 
        m_fwrite_cb = std::move(cb);
    }

    void setFlushCallback(FlushCallback&& cb) { 
        m_flush_cb = std::move(cb);
    }

    void start();
    void pushMsg(const LogContext& ctx);
    
private:
    void doDone();
    void doLast();
    void threadWorker();

private:
    using Buffer = LogBuffer<kLargeBuffer>;

    int m_flush_interval;
    std::atomic<bool> m_running;
    WaitGroup m_waitGroup;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::unique_ptr<Thread> m_thread;
    FwriteCallback m_fwrite_cb;
    FlushCallback m_flush_cb;

    Buffer m_curBuffer;    // 当前缓冲区
    Buffer m_nextBuffer;   // 预留，减少内存分配
    std::vector<Buffer> m_buffers;
};

} // namespace log
} // namespace conet

#endif