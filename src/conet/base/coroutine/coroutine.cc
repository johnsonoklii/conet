#include "conet/base/coroutine/coroutine.h"
#include "conet/base/log/logger.h"

#include <cstring>
#include <cassert>

#include <atomic>

namespace conet {

static thread_local Coroutine::sptr t_main_coroutine = nullptr;
static thread_local Coroutine::sptr t_cur_coroutine = nullptr;

static std::atomic_int s_coroutine_count{0};
static std::atomic_int s_cur_coroutine_id{0};

int getNextCoroutineId() {
    return ++s_cur_coroutine_id;
}

Coroutine::sptr Coroutine::getCurrentCoroutine() {
    if (!t_cur_coroutine) {
        t_main_coroutine = std::make_shared<Coroutine>();
        t_cur_coroutine = t_main_coroutine;
    }
    return t_cur_coroutine;
}

Coroutine::sptr Coroutine::getMainCoroutine() {
    if (!t_main_coroutine) {
        t_main_coroutine = std::make_shared<Coroutine>();
        if (!t_cur_coroutine) {
            t_cur_coroutine = t_main_coroutine;
        }
    }
    return t_main_coroutine;
}

bool Coroutine::isMainCoroutine() {
    if (!t_main_coroutine || t_cur_coroutine == t_main_coroutine) {
        return true;
    }
    return false;
}

void coRun(Coroutine* co) {
    assert(co);
    co->m_callback();
    co->setState(Coroutine::kFinished);
    Coroutine::yield();
}

Coroutine::Coroutine() {
    m_cor_id = 0;
    s_coroutine_count++;
    std::memset(&m_coctx, 0, sizeof(m_coctx));
}

Coroutine::Coroutine(int stack_size): m_stack_size(stack_size) {
    m_cor_id = getNextCoroutineId();
    m_stack_sp = static_cast<char*>(malloc(stack_size));
    if (!m_stack_sp) {
        throw std::bad_alloc();
    }

    std::memset(&m_coctx, 0, sizeof(m_coctx));
    s_coroutine_count++;
}

Coroutine::Coroutine(int stack_size, std::function<void()> cb) 
: Coroutine(stack_size) {
    setCallback(cb);
}

Coroutine::~Coroutine() {
    // LOG_DEBUG("Coroutine::~Coroutine(): free stack sp.")
    if (m_stack_sp) {
        free(m_stack_sp);
    }
}

void Coroutine::setCallback(std::function<void()> cb) {
    assert(m_stack_sp);

    if (this == t_main_coroutine.get()) {
        LOG_ERROR("Coroutine::setCallback(): can't set callback for main coroutine.")
        return;
    }

    m_callback = cb;
    char* top = m_stack_sp + m_stack_size;
    // 16位对齐
    top = reinterpret_cast<char*>((reinterpret_cast<uintptr_t>(top)) & -16LL);
    
    std::memset(&m_coctx, 0, sizeof(m_coctx));
    m_coctx.regs[kRBP] = top;
    m_coctx.regs[kRSP] = top;
    m_coctx.regs[kRIP] = reinterpret_cast<char*>(coRun);
    m_coctx.regs[kRDI] = reinterpret_cast<char*>(this);
}

void Coroutine::resume(const Coroutine::sptr& co) {
    assert(co->getState() == kReady || co->getState() == kSuspend);
    if (!t_main_coroutine) {
        LOG_ERROR("Coroutine::resume(): main coroutine is nullptr.")
        return;
    }

    if (t_cur_coroutine != t_main_coroutine) {
        LOG_ERROR("Coroutine::resume(): current coroutine must be main coroutine.")
        return;
    }

    if (co == t_cur_coroutine) {
        LOG_DEBUG("Coroutine::resume(): current coroutine is pending cor, need't swap.")
        return;
    }

    co->setState(State::kRunning);

    t_cur_coroutine = co;
    coctx_swap(&t_main_coroutine->m_coctx, &co->m_coctx);
}

void Coroutine::yield() {
    
    if (!t_main_coroutine) {
        LOG_ERROR("Coroutine::yield(): main coroutine is nullptr.")
        return;
    }

    if (!t_cur_coroutine) {
        LOG_ERROR("Coroutine::yield(): current coroutine is nullptr.")
        return;
    }

    if (t_cur_coroutine == t_main_coroutine) {
        LOG_ERROR("Coroutine::yield(): current coroutine is main coroutine.")
        return;
    }

    Coroutine* co = t_cur_coroutine.get(); // COMMENT: 这里不能用Coroutine::sptr,会导致co无法释放
    t_cur_coroutine = t_main_coroutine;

    assert(co->getState() == kRunning || co->getState() == kFinished);
    if (co->getState() == kRunning) {
        co->setState(kSuspend);
    }
    
    coctx_swap(&co->m_coctx, &t_main_coroutine->m_coctx);
}

} // namespace conet