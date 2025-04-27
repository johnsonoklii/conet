#ifndef LEMON_BUFFER_H
#define LEMON_BUFFER_H

#include <vector>
#include <algorithm>
#include <string>

#include <string.h>
#include <stdint.h>
#include <cassert>

namespace conet {
namespace net {

class Socket;

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initial_size = kInitialSize)
    : m_buffer(kCheapPrepend + initial_size)
     , m_reader_index(kCheapPrepend)
     , m_writer_index(kCheapPrepend) {
    
    }

    void swap(Buffer& rhs) {
        if (this == &rhs) return;

        m_buffer.swap(rhs.m_buffer);
        std::swap(m_reader_index, rhs.m_reader_index);
        std::swap(m_writer_index, rhs.m_writer_index);
    }

    size_t readableBytes() const {
        return m_writer_index - m_reader_index;
    }

    size_t writableBytes() const {
        return m_buffer.size() - m_writer_index;
    }

    size_t prependableBytes() const {
        return m_reader_index;
    }

    const char* peek() const {
        return begin() + m_reader_index;
    }

    const char* findCRLF() const {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? nullptr : crlf;
    }

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if (len < readableBytes()) {
            m_reader_index += len;
        } else {
            retrieveAll();
        }
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void retrieveInt64() {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32() {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16() {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8() {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll() {
        m_reader_index = kCheapPrepend;
        m_writer_index = kCheapPrepend;
    }

    void retrieveUntil(const char* end) {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void append(const char* data, size_t len) {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    char* beginWrite() {
        return begin() + m_writer_index;
    }

    const char* beginWrite() const {
        return begin() + m_writer_index;
    }

    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        m_writer_index += len;
    }

    void ensureWritableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }

        assert(writableBytes() >= len);
    }

    ssize_t readSocket(const Socket& socket, int* save_errno);

    std::string toString() const {
        return std::string(peek(), readableBytes());
    }

private:
    char* begin() { return m_buffer.data(); }

    const char* begin() const { return m_buffer.data(); }

    void makeSpace(size_t len) {
        if (len - writableBytes() > prependableBytes() - kCheapPrepend) {
            // writeable + 已经读了的区域  < len
            m_buffer.resize(m_writer_index + len);
        } else {
            // 移动已经读了的区域
            assert(kCheapPrepend < m_reader_index);
            size_t readale = readableBytes();
            std::copy(begin()+m_reader_index,
                      begin()+m_writer_index,
                      begin()+kCheapPrepend);

            m_reader_index = kCheapPrepend;
            m_writer_index = m_reader_index + readale;
            assert(readale == readableBytes());
        }
    }

private:
    std::vector<char> m_buffer;
    size_t m_reader_index;
    size_t m_writer_index;

    static const char kCRLF[];
};

} // namespace net
} // namespace lemon

#endif