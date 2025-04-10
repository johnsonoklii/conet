#include "conet/base/util/waitgroup.h"

namespace conet {
    
WaitGroup::WaitGroup(int count)
: m_count(count), m_mutex(), m_cv() {

}

void WaitGroup::wait() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_count > 0) {
        m_cv.wait(lock);
    }
}

void WaitGroup::add(int num) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_count += num;
}

int32_t WaitGroup::getCount() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_count;
}

void WaitGroup::done() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_count--;
    if (m_count == 0) {
      m_cv.notify_one();
    }
}

}