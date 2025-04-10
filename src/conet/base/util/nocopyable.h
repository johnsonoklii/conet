#ifndef CONET_NOCOPYABLE_H
#define CONET_NOCOPYABLE_H

namespace conet {

class nocopyable {
public:
    nocopyable() = default;
    nocopyable(const nocopyable&) = delete;
    nocopyable& operator=(const nocopyable&) = delete;
};

} // namespace conet

#endif