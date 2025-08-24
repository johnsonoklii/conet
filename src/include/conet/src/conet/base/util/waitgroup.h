#ifndef CONET_WAITGROUP_H
#define CONET_WAITGROUP_H

#include "conet/base/util/nocopyable.h"

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace conet {

class WaitGroup : public nocopyable {
    public:
        WaitGroup() = default;
        explicit WaitGroup(int count);
        void wait();
        void done();
        void add(int num);
        int32_t getCount();
    
    private:
        int32_t m_count = 0;
        std::mutex m_mutex;
        std::condition_variable m_cv;
    };
    
} // namespace conet

#endif