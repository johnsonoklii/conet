#include "conet/base/util/thread.h"
#include "conet/base/util/process.h"

#include <cassert>

namespace conet {

std::atomic_int Thread::m_num_created{0};

Thread::Thread(const std::string& name, ThreadFunc&& func)
: m_name(name)
, m_func(std::move(func))
, m_wg(1) {
    setDefaultName();
}

Thread::~Thread() {
    if (m_started && !m_joined) {
        m_thread.detach();
    }
}

void Thread::start() {
    assert(!m_started);
    m_started = true;
    m_thread = std::thread([this] { runInThread(); });
    m_wg.wait();
}

void Thread::join() {
    assert(m_started);
    assert(!m_joined);
    m_joined = true;
    m_thread.join();
}

void Thread::setDefaultName() {
    if (m_name.empty()) {
        int num = m_num_created.fetch_add(1);
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread-%d", num);
        m_name = buf;
    }
}

void Thread::runInThread() {
    m_tid = ProcessInfo::tid();
    m_wg.done();
    m_func();
}

}