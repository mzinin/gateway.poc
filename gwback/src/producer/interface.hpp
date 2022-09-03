#pragma once

#include <message/message.hpp>

#include <cstdint>


class IProducer
{
public:
    virtual ~IProducer() = default;

    virtual Messages getNext(uint32_t limit) = 0;
    virtual void markConsumed(const Messages& messages) = 0;
};
