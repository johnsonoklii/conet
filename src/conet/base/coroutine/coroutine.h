#ifndef CONET_COROUTINE_H
#define CONET_COROUTINE_H

#include "conet/base/coroutine/coctx.h"
#include "conet/base/util/nocopyable.h"

#include <functional>
#include <memory>   

namespace conet {

static constexpr int kDefaultStackSize = 1024 * 1024; // WARNING: 协程栈空间不能太小

class Coroutine: public nocopyable {
public:
    using sptr = std::shared_ptr<Coroutine>;

    enum State {
        kReady,
        kRunning,
        kSuspend,
        kFinished
    };

    // only for main coroutine
    Coroutine();
    Coroutine(int stack_size);
    Coroutine(int stack_size, std::function<void()> cb);
    ~Coroutine();

    void setCallback(std::function<void()> cb);

public:
    static Coroutine::sptr getCurrentCoroutine();
    static Coroutine::sptr getMainCoroutine();
    static bool isMainCoroutine();

    State  getState() const { return m_state; }
    void setState(State state) { m_state = state; }

    static void resume(const Coroutine::sptr& co);
    static void yield();
    
private:
    int m_cor_id{0};
    int m_stack_size{kDefaultStackSize};
    char* m_stack_sp{nullptr};
    State m_state{kReady};

    coctx m_coctx;

public:
    std::function<void()> m_callback;
};

} // namespace conet

#endif