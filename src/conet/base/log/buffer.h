#ifndef CONET_LOG_BUFFER_H
#define CONET_LOG_BUFFER_H

#include "conet/base/log/log_context.h"
#include "conet/base/util/nocopyable.h"

#include <cstddef>

#include <string>
#include <memory>

namespace conet {
namespace log {

struct inner_message {
    LogContext ctx;

    inner_message() {}
    inner_message(const LogContext& c) : ctx(c) { }
    inner_message(LogContext&& c) : ctx(std::move(c)) { }
    inner_message(const inner_message& other) : ctx(other.ctx) { }
    inner_message(inner_message&& other): ctx(std::move(other.ctx)){ }
    inner_message& operator=(inner_message&& other) {
        ctx = std::move(other.ctx);
        return *this;
    }
};

enum { kSmallBuffer = 4096, kLargeBuffer = 65536 };

template <int SIZE>
class LogBuffer: public nocopyable {
public:
    LogBuffer() : m_data(new inner_message[SIZE]), m_current(m_data){}
    
    ~LogBuffer() { delete[] m_data; }

    LogBuffer(const LogBuffer& other) = default;

    LogBuffer(LogBuffer&& other) noexcept
    : m_data(other.m_data), m_current(other.m_current) {
        other.m_data = nullptr;
        other.m_current = nullptr;
    }

    LogBuffer& operator=(LogBuffer&& other) noexcept {
        m_data = other.m_data;
        m_current = other.m_current;
        other.m_data = nullptr;
        other.m_current = nullptr;
        return *this;
    }

    bool valid() const { return m_data != nullptr; }

    int avail() { return static_cast<int>(endAddr() - m_current); }

    bool hasData() const { return m_current != m_data; }
    
    void push(const inner_message& msg) {
        if (avail() > 0) {
            *m_current = msg;
            m_current++;
        }
    }

    void push(inner_message&& msg) {
        if (avail() > 0) {
            *m_current = std::move(msg);  
            m_current++;
        }
    }

    inner_message* begin() const { 
        return m_data; 
    }

    inner_message* end() const { return m_current; }

    void reset() { m_current = m_data; }

private:
    inner_message* endAddr() { return m_data + SIZE; }

    inner_message* m_data;
    inner_message* m_current;
};

} // namespace log
} // namespace conet

#endif