#ifndef CONET_COROUTINE_H
#define CONET_COROUTINE_H

#include "conet/base/coroutine/coctx.h"
#include "conet/base/util/nocopyable.h"

#include <functional>

namespace conet {

class Coroutine: public nocopyable {
public:
    Coroutine(int stack_size);
    Coroutine(int stack_size, std::function<void()> cb);
    ~Coroutine();

    void setCallback(std::function<void()> cb);

public:
    static Coroutine* getCurrentCoroutine();
    static Coroutine* getMainCoroutine();

    static void resume(Coroutine* co);
    static void yield();

private:
    // only for main coroutine
    Coroutine();

private:
    int m_cor_id{0};
    int m_stack_size{0};
    char* m_stack_sp{nullptr};
    coctx m_coctx;

public:
    std::function<void()> m_callback;
};

} // namespace conet

#endif