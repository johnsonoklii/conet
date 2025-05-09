#include "conet/base/log/async_logging.h"
#include "conet/base/util/timestamp.h"
#include "conet/base/util/util.h"

#include <cassert>
#include <cstring>

namespace conet {
namespace log {

AsyncLogging::AsyncLogging(FwriteCallback&& cb, int flush_interval)
: m_flush_interval(flush_interval)
, m_running(false)
, m_wait_group(1)
, m_fwrite_cb(std::move(cb)) {
}

AsyncLogging::~AsyncLogging() {
    if (!m_running.load()) return;
    doDone();
}

void AsyncLogging::doDone() {
    m_running.store(false);
    m_cond.notify_one();
   
    if (m_thread && !m_thread->joined()) {
        m_thread->join();
    }
}

void AsyncLogging::start() {
    m_thread = std::unique_ptr<Thread>(new Thread("AsyncLogging", std::bind(&AsyncLogging::threadWorker, this))); // FIXME: 为什么这里使用shared_from_this就不会释放呢
    m_thread->start();
    m_wait_group.wait();
}

void AsyncLogging::pushMsg(const LogContext& ctx) {
    assert(m_running);
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cur_buffer.avail() > 0) {
        m_cur_buffer.push(inner_message{ctx});
        return;
    }

    m_buffers.push_back(std::move(m_cur_buffer)); 
    if (m_next_buffer.valid()) {
        m_cur_buffer = std::move(m_next_buffer);
    } else {
        m_cur_buffer = Buffer();
    }
    m_cur_buffer.push(inner_message{ctx});
    m_cond.notify_one();
}

void AsyncLogging::threadWorker() {
    try {
        m_running.store(true);
        Buffer new_buffer1;                  // 备用缓存，减少内存分配
        Buffer new_buffer2;
        std::vector<Buffer> buffers_write;  // 减小临界区
        buffers_write.reserve(16);
        m_wait_group.done();

        while (m_running.load()) {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_buffers.empty()) {
                    m_cond.wait_for(lock, std::chrono::seconds(m_flush_interval));
                }

                // 因为到了刷盘时间，m_buffers可能是空，需要先将m_cur_buffer输出
                if (m_buffers.empty() && m_cur_buffer.hasData()) {
                    m_buffers.push_back(std::move(m_cur_buffer));
                    m_cur_buffer = std::move(new_buffer1);
                }

                buffers_write.swap(m_buffers);
                if (!m_next_buffer.valid()) {
                    m_next_buffer = std::move(new_buffer2);
                }
            }

            if (buffers_write.empty()) {
                continue;
            }

            // FIXME: buffert太大，一次性要移除这么多？可以加入告警
            if (buffers_write.size() > 100) {
                char buf[256];
                snprintf(buf, sizeof(buf),
                        "Dropped log messages at %s, %zd larger buffers\n",
                        Timestamp::now().toString().c_str(), buffers_write.size() - 2);
                fputs(buf, stderr);
                m_fwrite_cb(buf, strlen(buf));
                buffers_write.erase(buffers_write.begin() + 2, buffers_write.end());
            }
            
            for (const auto& buffer : buffers_write) {
                for (const auto& it : buffer) {
                    std::string&& msg = it.ctx.format();
                    m_fwrite_cb(msg.c_str(), msg.size());
                }
            }

            if (buffers_write.size() < 2) {
                buffers_write.resize(2);
            }

            if (!new_buffer1.valid()) {
                new_buffer1 = std::move(buffers_write.back());
                buffers_write.pop_back();
                new_buffer1.reset();
            }

            if (!new_buffer2.valid()) {
                new_buffer2 = std::move(buffers_write.back());
                buffers_write.pop_back();
                new_buffer2.reset();
            }

            buffers_write.clear(); 
            if (m_flush_cb) {
                m_flush_cb();
            }
        }

        // 收尾工作，当前缓冲区可能还有未输出的日志
        doLast();
    } catch (const std::exception& e) {
        fprintf(stderr, "Error log thread, err=%s\n", e.what());
        m_thread.reset();
    }
}

void AsyncLogging::doLast() {
    if (m_cur_buffer.hasData()) {
        m_buffers.push_back(std::move(m_cur_buffer));
    }
    for (auto& buffer : m_buffers) {
        for (const auto& it : buffer) {
            std::string&& msg = it.ctx.format();
            m_fwrite_cb(msg.c_str(), msg.size());
        }
    }
    if (m_flush_cb) {
        m_flush_cb();
    }
}

} // namespace log
} // namespace conet